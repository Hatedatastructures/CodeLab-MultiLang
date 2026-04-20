#pragma once
#include "Unit.hpp"
#include "Rank.hpp"
#include "Worker.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

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

        std::mutex _scalingMutex; // 扩缩容互斥锁
        mutable std::shared_mutex _workersMutex; // 工作线程读写锁

        std::condition_variable _scalingCv; // 扩缩容条件变量

        std::unique_ptr<std::jthread> _monitorThread; // 监控线程
        std::unique_ptr<std::jthread> _scalingThread; // 扩缩容线程

        load_metrics _metrics; // 负载指标
        scheduling_tactics _policy; // 调度策略
        scaling_config _scalingConfig; // 扩缩容配置
        expansion_strategy _scalingPolicy; // 扩缩容策略

        std::function<void(const std::string &)> _eventCallback; // 事件回调
        std::function<safety_worker_pointer(const std::string &)> _workerFactory; // 工作线程工厂

        std::chrono::steady_clock::time_point _startTime;       // 启动时间
        std::atomic<std::uint64_t> _totalTasksScheduled{0};    // 总调度任务数
        std::atomic<std::uint64_t> _totalScalingOperations{0}; // 总扩缩容操作数
    public:
        scheduler_ordinary(safety_rank_pointer rank, scheduling_tactics policy = scheduling_tactics::adaptive,
            expansion_strategy scaling_policy = expansion_strategy::reactive)
        :_unit_rank(std::move(rank)),_policy(policy),_scalingPolicy(scaling_policy)
        {
            _startTime = std::chrono::steady_clock::now();
            _workerFactory = [this](const std::string &name) -> safety_worker_pointer
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
                    initial_workers = _scalingConfig.core_threads;
                initial_workers = std::clamp(initial_workers, _scalingConfig.min_threads, _scalingConfig.max_threads);

                // 创建初始工作线程
                if (!createWorkers(initial_workers))
                    return false;
                _should_stop.store(false, std::memory_order_release);
                _running.store(true, std::memory_order_release);

                _monitorThread = std::make_unique<std::jthread>(&scheduler_ordinary::monitorLoop, this);
                _scalingThread = std::make_unique<std::jthread>(&scheduler_ordinary::scalingLoop, this);

                if (_eventCallback)
                    _eventCallback("Scheduler started with " + std::to_string(initial_workers) + " threads");

                return true;
            }
            catch(const std::exception& e)
            {
                if (_eventCallback)
                    _eventCallback("Failed to start scheduler: " + std::string(e.what()));
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
            _scalingCv.notify_all();

            if(_monitorThread && _monitorThread->joinable())
                _monitorThread->join();
            if(_scalingThread && _scalingThread->joinable())
                _scalingThread->join();

            stopAllWorkers(wait_for_completion);

            _running.store(false, std::memory_order_release);

            if (_eventCallback)
                _eventCallback("Scheduler stopped");
        }
        /**
     * @brief 提交任务
     */
        virtual bool submitUint(safety_unit_pointer task)
        {
            if (!task || !_running.load(std::memory_order_acquire))
                return false;

            // 执行调度策略
            bool result = scheduleTask(task);

            if (result)
            {
                _totalTasksScheduled.fetch_add(1, std::memory_order_relaxed);
                updateMetrics_on_task_submit();
            }
            return result;
        }
        std::size_t getThreadCount() const
        {
            std::shared_lock<std::shared_mutex> lock(_workersMutex);
            return _workers.size();
        }
        std::size_t getActiveThreadCount() const
        {
            std::shared_lock<std::shared_mutex> lock(_workersMutex);
            auto active_count = [this](const safety_worker_pointer &worker)
            {
                return worker->isRunning(); 
            };
            return std::count_if(_workers.begin(), _workers.end(), active_count);
        }
        const load_metrics &get_metrics() const
        {
            return _metrics;
        }
        void setScalingConfig(const scaling_config &config)
        {
            std::lock_guard<std::mutex> lock(_scalingMutex);
            _scalingConfig = config;
            _scalingCv.notify_one();
        }
        const scaling_config & getScalingConfig() const
        {
            return _scalingConfig;
        }
        void setSchedulingPolicy(scheduling_tactics policy)
        {
            _policy = policy;
        }
        scheduling_tactics getSchedulingPolicy() const
        {
            return _policy;
        }
        void setScalingPolicy(expansion_strategy policy)
        {
            _scalingPolicy = policy;
        }
        expansion_strategy getScalingPolicy() const
        {
            return _scalingPolicy;
        }
        void setEventCallback(std::function<void(const std::string &)> callback)
        {
            _eventCallback = std::move(callback);
        }
        void setWorkerFactory(std::function<safety_worker_pointer(const std::string &)> factory)
        {
            _workerFactory = std::move(factory);
        }
        std::uint64_t getTotalTasksScheduled() const
        {
            return _totalTasksScheduled.load(std::memory_order_relaxed);
        }
        std::uint64_t getTotalScalingOperations() const
        {
            return _totalScalingOperations.load(std::memory_order_relaxed);
        }
        double getUptime() const
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _startTime);
            return duration.count();
        }
        void manualScaleUp()
        {
            scale_up();
        }
        void mutualScaleUps(std::size_t count)
        {
            auto scale_threads = _scalingConfig.max_threads - getThreadCount();
            auto scale_count = std::min(count, scale_threads);

            if (scale_count > 0 && createWorkers(scale_count))
            {
                _totalScalingOperations.fetch_add(1, std::memory_order_relaxed);

                if (_eventCallback)
                {
                    _eventCallback("Scaled up by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        void manualScaleDown()
        {
            scale_down();
        }
        void manualScaleDowns(std::size_t count)
        {
            auto scale_threads = getThreadCount() - _scalingConfig.min_threads;
            auto scale_count = std::min(count, scale_threads);

            if (scale_count > 0)
            {
                std::unique_lock<std::shared_mutex> lock(_workersMutex);

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

                _totalScalingOperations.fetch_add(1, std::memory_order_relaxed);

                if (_eventCallback)
                {
                    _eventCallback("Scaled down by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        bool isRunning() const
        {
            return _running.load(std::memory_order_acquire);
        }
    protected:
        virtual bool scheduleTask(safety_unit_pointer task)
        {
            if(!_running.load(std::memory_order_acquire))
                return false;
            return _unit_rank->push(task);
        }
        virtual bool createWorkers(std::size_t count)
        {
            std::unique_lock<std::shared_mutex> lock(_workersMutex);
            for (std::size_t i = 0; i < count; ++i)
            {
                auto worker_id = "worker_" + std::to_string(_workers.size());
                auto worker = _workerFactory(worker_id);

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
        virtual void stopAllWorkers(bool wait_for_completion)
        {
            std::unique_lock<std::shared_mutex> lock(_workersMutex);

            for (auto &worker : _workers)
            {
                if (worker)
                {
                    worker->stop(wait_for_completion);
                }
            }
            _workers.clear();
        }
        virtual void monitorLoop()
        {
            try
            {
                while (!_should_stop.load(std::memory_order_acquire))
                {
                    updateMetrics();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                throw std::runtime_error("Monitor loop failed");
            }
            
        }
        virtual void scalingLoop()
        {
            try
            {
                while (!_should_stop.load(std::memory_order_acquire))
                {
                    std::unique_lock<std::mutex> lock(_scalingMutex);
                    auto time_out_func = [this] 
                    { 
                        return _should_stop.load(std::memory_order_acquire); 
                    };
                    _scalingCv.wait_for(lock, std::chrono::seconds(1), time_out_func);
                    if (!_should_stop.load())
                    {
                        evaluateScaling();
                    }
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                throw std::runtime_error("Scaling loop failed");
            }
            
        }
        virtual void updateMetrics()
        {
            _metrics.queue_length.store(_unit_rank->size(), std::memory_order_relaxed);
            _metrics.active_threads.store(getActiveThreadCount(), std::memory_order_relaxed);
            _metrics.last_update = std::chrono::steady_clock::now();
            // 子类可以重写此方法添加更多指标
        }
        virtual void updateMetrics_on_task_submit()
        {
            // 子类可以重写此方法
        }
        virtual void evaluateScaling()
        {
            auto load_score = _metrics.calculateLoadScore();
            auto current_threads = getThreadCount();

            if (load_score > _scalingConfig.scale_up_threshold && current_threads < _scalingConfig.max_threads)
            {
                scale_up();
            }
            else if (load_score < _scalingConfig.scale_down_threshold && current_threads > _scalingConfig.min_threads)
            {
                scale_down();
            }
        }
        virtual void scale_up()
        {
            auto scale_threads = _scalingConfig.max_threads - getThreadCount();
            auto scale_count = std::min(_scalingConfig.scale_up_step,scale_threads);

            if (scale_count > 0 && createWorkers(scale_count))
            {
                _totalScalingOperations.fetch_add(1, std::memory_order_relaxed);

                if (_eventCallback)
                {
                    _eventCallback("Scaled up by " + std::to_string(scale_count) + " threads");
                }
            }
        }
        virtual void scale_down()
        {
            auto scale_threads = getThreadCount() - _scalingConfig.min_threads;
            auto scale_count = std::min(_scalingConfig.scale_down_step, scale_threads);

            if (scale_count > 0)
            {
                std::unique_lock<std::shared_mutex> lock(_workersMutex);

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

                _totalScalingOperations.fetch_add(1, std::memory_order_relaxed);

                if (_eventCallback)
                {
                    _eventCallback("Scaled down by " + std::to_string(scale_count) + " threads");
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
namespace pool 
{
    using internals::structure_s::scheduler_ordinary;
    using internals::structure_s::make_scheduler_ordinary;
}