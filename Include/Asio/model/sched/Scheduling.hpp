/**
 * @file Scheduling.hpp
 * @brief 调度器定义
 * @details 提供任务队列、工作线程、调度策略等功能
 */
#pragma once
#include "Unit.hpp"
#include "Rank.hpp"
#include "Worker.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace internals
{
    namespace structure_s{}
}
namespace internals::structure_s
{
    using namespace structure_w;
    using safety_worker_pointer = std::unique_ptr<structure_w::worker_ordinary>;
    class scheduler_ordinary
    {
    protected:
        safety_rank_pointer _unit_rank; // 任务队列
        std::vector<safety_worker_pointer> _workers; // 工作线程列表

        std::atomic<bool> _running{false}; // 调度器运行状态
        std::atomic<bool> _should_stop{false}; // 停止标志

        std::mutex _scaling_mutex; // 扩缩容互斥锁
        mutable std::shared_mutex _workers_mutex; // 工作线程读写锁

        std::condition_variable _scaling_cv; // 扩缩容条件变量

        std::unique_ptr<std::jthread> _monitor_thread; // 监控线程
        std::unique_ptr<std::jthread> _scaling_thread; // 扩缩容线程

        load_metrics _metrics; // 负载指标
        scheduling_tactics _policy; // 调度策略
        scaling_config _scaling_config; // 扩缩容配置
        expansion_strategy _scaling_policy; // 扩缩容策略

        std::function<void(const std::string &)> _event_callback; // 事件回调
        std::function<safety_worker_pointer(const std::string &)> _worker_factory; // 工作线程工厂

        std::chrono::steady_clock::time_point _start_time;       // 启动时间
        std::atomic<std::uint64_t> _total_tasks_scheduled{0};    // 总调度任务数
        std::atomic<std::uint64_t> _total_scaling_operations{0}; // 总扩缩容操作数
        /**
     * @brief 负载平滑与抗抖动状态
     */
        double _ema_load{0.0};                                    // 指数滑动平均负载
        std::size_t _last_queue_length{0};                        // 上次队列长度
        std::size_t _up_window_count{0};                          // 扩容滞后窗口计数
        std::size_t _down_window_count{0};                        // 缩容滞后窗口计数
        std::chrono::steady_clock::time_point _last_scale_time{}; // 上次扩缩容时间
    public:
        scheduler_ordinary(safety_rank_pointer rank, scheduling_tactics policy = scheduling_tactics::adaptive,
            expansion_strategy scaling_policy = expansion_strategy::reactive)
        :_unit_rank(std::move(rank)),_policy(policy),_scaling_policy(scaling_policy)
        {
            _start_time = std::chrono::steady_clock::now();
            _worker_factory = [this](const std::string &name) -> safety_worker_pointer
            {
                return structure_w::make_worker_adaptive(name, _unit_rank);
            };
        }
        virtual ~scheduler_ordinary()
        {
            stop();
        }
        scheduler_ordinary(const scheduler_ordinary&) = delete;
        scheduler_ordinary& operator=(const scheduler_ordinary&) = delete;
        scheduler_ordinary(scheduler_ordinary&&) = delete;
        scheduler_ordinary& operator=(scheduler_ordinary&&) = delete;
        /**
     * @brief 启动调度器
     * @param initial_threads 初始线程数
     * @return true 启动成功，false 启动失败
     */
        virtual bool start(std::size_t initial_workers = 0)
        {
            if(_running.load(std::memory_order_acquire))
                return false;
            try
            {
                // 确定初始线程数
                if (initial_workers == 0)
                    initial_workers = _scaling_config.core_threads;
                initial_workers = std::clamp(initial_workers, _scaling_config.min_threads, _scaling_config.max_threads);

                // 创建初始工作线程
                if (!create_workers(initial_workers))
                    return false;
                _should_stop.store(false, std::memory_order_release);
                _running.store(true, std::memory_order_release);

                _monitor_thread = std::make_unique<std::jthread>(&scheduler_ordinary::monitor_loop, this);
                _scaling_thread = std::make_unique<std::jthread>(&scheduler_ordinary::scaling_loop, this);

                if (_event_callback)
                    _event_callback("Scheduler started with " + std::to_string(initial_workers) + " threads");

                return true;
            }
            catch(const std::exception& e)
            {
                if (_event_callback)
                    _event_callback("Failed to start scheduler: " + std::string(e.what()));
                else
                    std::cerr << "Failed to start scheduler: " << e.what() << std::endl;
                return false;
            }
        }
        /**
     * @brief 停止调度器
     * @param wait_for_completion 是否等待任务完成
     */
        virtual void stop(bool wait_for_completion = true)
        {
            if(!_running.load(std::memory_order_acquire))
                return;
            _should_stop.store(true, std::memory_order_release);
            _scaling_cv.notify_all();

            if(_monitor_thread && _monitor_thread->joinable())
                _monitor_thread->join();
            if(_scaling_thread && _scaling_thread->joinable())
                _scaling_thread->join();

            stop_all_workers(wait_for_completion);

            _running.store(false, std::memory_order_release);

            if (_event_callback)
                _event_callback("Scheduler stopped");
        }
        /**
     * @brief 提交任务
     */
        virtual bool submit_uint(safety_unit_pointer task)
        {
            if (!task || !_running.load(std::memory_order_acquire))
                return false;

            // 执行调度策略
            bool result = schedule_task(task);

            if (result)
            {
                _total_tasks_scheduled.fetch_add(1, std::memory_order_relaxed);
                update_metrics_on_task_submit();
            }
            return result;
        }
        std::size_t get_thread_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_workers_mutex);
            return _workers.size();
        }
        std::size_t get_active_thread_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_workers_mutex);
            auto active_count = [this](const safety_worker_pointer &worker)
            {
                return worker->is_running(); 
            };
            return std::count_if(_workers.begin(), _workers.end(), active_count);
        }
        const load_metrics &get_metrics() const
        {
            return _metrics;
        }
        void set_scaling_config(const scaling_config &config)
        {
            std::lock_guard<std::mutex> lock(_scaling_mutex);
            _scaling_config = config;
            _scaling_cv.notify_one();
        }
        const scaling_config & get_scaling_config() const
        {
            return _scaling_config;
        }
        void set_scheduling_policy(scheduling_tactics policy)
        {
            _policy = policy;
        }
        scheduling_tactics get_scheduling_policy() const
        {
            return _policy;
        }
        void set_scaling_policy(expansion_strategy policy)
        {
            _scaling_policy = policy;
        }
        expansion_strategy get_scaling_policy() const
        {
            return _scaling_policy;
        }
        void set_event_callback(std::function<void(const std::string &)> callback)
        {
            _event_callback = std::move(callback);
        }
        void set_worker_factory(std::function<safety_worker_pointer(const std::string &)> factory)
        {
            _worker_factory = std::move(factory);
        }
        std::uint64_t get_total_tasks_scheduled() const
        {
            return _total_tasks_scheduled.load(std::memory_order_relaxed);
        }
        std::uint64_t get_total_scaling_operations() const
        {
            return _total_scaling_operations.load(std::memory_order_relaxed);
        }
        double get_uptime() const
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _start_time);
            return duration.count();
        }
        void manual_scale_up()
        {
            scale_up();
        }
        void mutual_scale_ups(std::size_t count)
        {
            auto scale_threads = _scaling_config.max_threads - get_thread_count();
            auto scale_count = std::min(count, scale_threads);

            if (scale_count > 0 && create_workers(scale_count))
            {
                _total_scaling_operations.fetch_add(1, std::memory_order_relaxed);

                if (_event_callback)
                {
                    _event_callback("Scaled up by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        void manual_scale_down()
        {
            scale_down();
        }
        /**
     * @brief 手动缩放线程池，减少工作线程数量
     * @param count 要减少的线程数量
     */
        void manual_scale_downs(std::size_t count)
        {
            auto scale_threads = get_thread_count() - _scaling_config.min_threads;
            auto scale_count = std::min(count, scale_threads);

            if (scale_count > 0)
            {
                std::unique_lock<std::shared_mutex> lock(_workers_mutex);

                // 停止最后几个工作线程
                for (std::size_t i = 0; i < scale_count && !_workers.empty(); ++i)
                {
                    auto &worker = _workers.back();
                    if (worker)
                    {
                        worker->stop(true);
                    }
                    _workers.pop_back();
                }

                _total_scaling_operations.fetch_add(1, std::memory_order_relaxed);

                if (_event_callback)
                {
                    _event_callback("Scaled down by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        bool is_running() const
        {
            return _running.load(std::memory_order_acquire);
        }
    protected:
        virtual bool schedule_task(safety_unit_pointer task)
        {
            if(!_running.load(std::memory_order_acquire))
                return false;
            return _unit_rank->push(task);
        }
        virtual bool create_workers(std::size_t count)
        {
            std::unique_lock<std::shared_mutex> lock(_workers_mutex);
            for (std::size_t i = 0; i < count; ++i)
            {
                auto worker_id = "worker_" + std::to_string(_workers.size());
                auto worker = _worker_factory(worker_id);

                if (!worker || !worker->start())
                    return false;

                _workers.push_back(std::move(worker));
            }
            return true;
        }
        /**
     * @brief 停止所有工作线程
     * @param wait_for_completion 是否等待任务完成
     */
        virtual void stop_all_workers(bool wait_for_completion)
        {
            std::unique_lock<std::shared_mutex> lock(_workers_mutex);

            for (auto &worker : _workers)
            {
                if (worker)
                {
                    worker->stop(wait_for_completion);
                }
            }
            _workers.clear();
        }
        virtual void monitor_loop()
        {
            try
            {
                while (!_should_stop.load(std::memory_order_acquire))
                {
                    update_metrics();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                throw std::runtime_error("Monitor loop failed");
            }
            
        }
        virtual void scaling_loop()
        {
            try
            {
                while (!_should_stop.load(std::memory_order_acquire))
                {
                    std::unique_lock<std::mutex> lock(_scaling_mutex);
                    auto time_out_func = [this] 
                    { 
                        return _should_stop.load(std::memory_order_acquire); 
                    };
                    _scaling_cv.wait_for(lock, std::chrono::seconds(1), time_out_func);
                    if (!_should_stop.load())
                    {
                        evaluate_scaling();
                    }
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                throw std::runtime_error("Scaling loop failed");
            }
            
        }
        virtual void update_metrics()
        {
            auto current_len = _unit_rank->size();
            _metrics.queue_length.store(current_len, std::memory_order_relaxed);

            _metrics.active_threads.store(get_active_thread_count(), std::memory_order_relaxed);
            _metrics.total_threads.store(get_thread_count(), std::memory_order_relaxed);

            // 采集队列容量（支持动态队列）
            std::size_t capacity = 0;
            if (_unit_rank)
            {
                auto transition_state = std::dynamic_pointer_cast<internals::structure_r::rank_ordinary>(_unit_rank);
                if (transition_state.get() != nullptr)
                    capacity = transition_state->get_max_size();
            }
            if (capacity == 0)
            {
                // 回退：若无法获取容量，使用当前线程数近似倍数，避免除数为0
                capacity = std::max<std::size_t>(current_len, 1);
            }
            _metrics.queue_capacity.store(capacity, std::memory_order_relaxed);

            _metrics.last_update = std::chrono::steady_clock::now();
            // 子类可以重写此方法添加更多指标
        }
        virtual void update_metrics_on_task_submit()
        {
            // 子类可以重写此方法
        }
        /**
     * @brief 评估缩放策略
     */
        virtual void evaluate_scaling()
        {
            // 基础负载分数（线程繁忙度 + 队列占用度）
            const double base_score = _metrics.calculate_load_score();

            // 队列增长压力项（仅正增长计入），归一化到容量
            auto current_len   = _metrics.queue_length.load(std::memory_order_relaxed);
            auto capacity      = std::max<std::size_t>(_metrics.queue_capacity.load(std::memory_order_relaxed), 1);
            auto total_threads = std::max<std::size_t>(_metrics.total_threads.load(std::memory_order_relaxed), 1);
            auto active_threads= _metrics.active_threads.load(std::memory_order_relaxed);

            double growth_norm = 0.0;
            if (current_len > _last_queue_length)
            {
                growth_norm = std::min(static_cast<double>(current_len - _last_queue_length) / static_cast<double>(capacity), 1.0);
            }

            // 即时负载分数（加入增长压力项）
            double instant_load = std::clamp(base_score + 0.2 * growth_norm, 0.0, 1.0);

            // 指数滑动平均，增强抗抖动
            constexpr double alpha = 0.3;
            _ema_load = alpha * instant_load + (1.0 - alpha) * _ema_load;
            _last_queue_length = current_len;

            const auto now = std::chrono::steady_clock::now();
            const auto since_last_scale = (_last_scale_time.time_since_epoch().count() == 0)
                ? std::chrono::milliseconds::max()
                : std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_scale_time);

            // 滞后窗口与冷却时间，避免频繁扩缩容
            const std::size_t up_required_windows = 2;   // 连续窗口数触发扩容
            const std::size_t down_required_windows = 3; // 连续窗口数触发缩容

            if (_ema_load > _scaling_config.scale_up_threshold)
                ++_up_window_count; else _up_window_count = 0;

            if (_ema_load < _scaling_config.scale_down_threshold)
                ++_down_window_count; else _down_window_count = 0;

            auto current_threads = total_threads;

            // 扩容条件：负载高、队列高占用或增长、达到滞后窗口、冷却期结束
            bool can_scale_up = (since_last_scale >= _scaling_config.scale_up_delay) &&
                                                    (_up_window_count >= up_required_windows) &&
                                                    (current_threads < _scaling_config.max_threads);

            // 缩容条件：负载低、队列低占用、线程空闲、达到滞后窗口、冷却期结束
            double queue_util = std::min(static_cast<double>(current_len) / static_cast<double>(capacity), 1.0);
            double thread_util = std::min(static_cast<double>(active_threads) / static_cast<double>(total_threads), 1.0);
            bool can_scale_down = (since_last_scale >= _scaling_config.scale_down_delay) &&
                                                        (_down_window_count >= down_required_windows) &&
                                                        (current_threads > _scaling_config.min_threads) &&
                                                        (queue_util < 0.15) && (thread_util < 0.30) && (growth_norm <= 0.0);

            if (can_scale_up)
            {
                scale_up();
                _last_scale_time = now;
                _up_window_count = 0;
            }
            else if (can_scale_down)
            {
                scale_down();
                _last_scale_time = now;
                _down_window_count = 0;
            }
        }
        virtual void scale_up()
        {
            auto scale_threads = _scaling_config.max_threads - get_thread_count();
            auto scale_count = std::min(_scaling_config.scale_up_step,scale_threads);

            if (scale_count > 0 && create_workers(scale_count))
            {
                _total_scaling_operations.fetch_add(1, std::memory_order_relaxed);

                if (_event_callback)
                {
                    _event_callback("Scaled up by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        virtual void scale_down()
        {
            auto scale_threads = get_thread_count() - _scaling_config.min_threads;
            auto scale_count = std::min(_scaling_config.scale_down_step, scale_threads);

            if (scale_count > 0)
            {
                std::unique_lock<std::shared_mutex> lock(_workers_mutex);

                // 停止最后几个工作线程
                for (std::size_t i = 0; i < scale_count && !_workers.empty(); ++i)
                {
                    auto &worker = _workers.back();
                    if (worker)
                    {
                        worker->stop(true);
                    }
                    _workers.pop_back();
                }

                _total_scaling_operations.fetch_add(1, std::memory_order_relaxed);

                if (_event_callback)
                {
                    _event_callback("Scaled down by " + std::to_string(scale_count) + " threads");
                }
            }
        }
    };
    inline std::unique_ptr<scheduler_ordinary> make_scheduler_ordinary(safety_rank_pointer rank,
    scheduling_tactics policy = scheduling_tactics::round_robin,
    expansion_strategy expansion_strategy = expansion_strategy::reactive)
    {
        return std::make_unique<scheduler_ordinary>(std::move(rank),policy,expansion_strategy);
    }
}