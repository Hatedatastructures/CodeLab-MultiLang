/**
 * @file Worker.hpp
 * @brief 工作线程定义
 * @details 提供工作线程的定义与操作，包括线程的创建、销毁、调度等功能
 */
#pragma once
#include "Unit.hpp"
#include "Rank.hpp"
#include "Integration.hpp"
#include <thread>
#include <string>
#include <atomic>
#include <memory>
#include <iostream>
#include <condition_variable>

namespace internals
{
    namespace structure_w{}
}
namespace internals::structure_w
{
    using safety_unit_pointer = internals::structure_r::safety_unit_pointer;
    using safety_rank_pointer = std::shared_ptr<internals::structure_r::rank_ordinary>;
    /**
   * @class worker_ordinary
   * @brief 工作线程基类
   * 
   * 定义工作线程的基本接口和行为，所有具体的工作线程类型都继承自此类
   * 
   * 设计模式： 模板方法模式：定义线程执行流程，策略模式：支持不同的任务获取策略
   * 
   * 调用关系：被`thread_pool`管理和调用， 从`rank_ordinary`获取任务， 执行`unit_ordinary`及其派生类
   */
class worker_ordinary
    {
    protected:
        /// @brief 
        std::unique_ptr<std::jthread> _worker_thread; // 线程对象

        std::atomic<bool> _stop{false}; // 停止标志
        std::atomic<bool> _detached{false}; // 分离标志
        std::atomic<worker_state> _state{worker_state::idle}; // 状态标志

        std::string _worker_name; // 线程名称
        worker_statistics _statistics; // 统计信息

        std::shared_mutex _state_mutex; // 状态互斥锁
        std::condition_variable _condition; // 条件变量

        safety_rank_pointer _unit_rank; // 任务队列

        std::function<void(const std::string&, safety_unit_pointer)> _unit_starts_callback; // 任务开始回调
        std::function<void(const std::string&, safety_unit_pointer)> _unit_finish_callback; // 任务完成回调

        std::function<void()> _worker_starts_callback; // 线程开始回调
        std::function<void()> _worker_finish_callback; // 线程完成回调

        std::function<void(const std::string&, const std::exception&)> _abnormal_callback; // 任务异常回调 
    public:
        worker_ordinary(const std::string& name, safety_rank_pointer rank) 
        : _worker_name(name), _unit_rank(std::move(rank)) {}
        virtual ~worker_ordinary()
        {
            if(!_detached.load(std::memory_order_acquire))
            {
                stop();
                if(_worker_thread && _worker_thread->joinable())
                    _worker_thread->join();
            }
        }
        worker_ordinary(const worker_ordinary &) = delete;
        worker_ordinary &operator=(const worker_ordinary &) = delete;
        worker_ordinary(worker_ordinary &&) = delete;
        worker_ordinary &operator=(worker_ordinary &&) = delete;
        /**
     * @brief 启动工作线程
     * @return `true` 启动成功，`false` 启动失败
     */
        virtual bool start() 
        {
            std::unique_lock<std::shared_mutex> lock(_state_mutex);
            if (_state.load(std::memory_order_acquire) != worker_state::idle)
            {
                return false;
            }
            try
            {
                _stop.store(false, std::memory_order_release);

                _worker_thread = std::make_unique<std::jthread>(&worker_ordinary::interior_run, this);

                _state.store(worker_state::running, std::memory_order_release);
                _statistics.start_time = std::chrono::steady_clock::now();
                lock.unlock();
                _condition.notify_all();
                return true;
            }
            catch (const std::exception &e)
            {
                _state.store(worker_state::error, std::memory_order_release);
                if (_abnormal_callback)
                {
                    _abnormal_callback(_worker_name, e);
                }
                else
                {
                    std::cerr << e.what() << '\n';
                }
                return false;
            }
        }
        /**
     * @brief 停止工作线程
     * @param wait_for_completion 是否等待当前任务完成
     */
        virtual void stop(bool wait_for_completion = true)
        {
            if(!_detached.load(std::memory_order_acquire))
            {
                _stop.store(true, std::memory_order_release);

                {
                    std::unique_lock<std::shared_mutex> lock(_state_mutex);
                    _state.store(worker_state::stopping, std::memory_order_release);
                }
                _condition.notify_all();

                if (_worker_thread && _worker_thread->joinable() && wait_for_completion)
                {
                    _worker_thread->join();
                }
            }
        }
        // 分离工作线程
        virtual void detach()
        {
            if (_worker_thread && _worker_thread->joinable())
            {
                _worker_thread->detach();
                _detached.store(true, std::memory_order_release);
            }
        }
        /**
     * @brief 等待线程结束
     * @param timeout 超时时间
     * @return `true` 线程已结束，`false` 超时
     */
        template <typename rep, typename period>
        bool wait_for_stop(const std::chrono::duration<rep, period> &timeout)
        {
            std::unique_lock<std::shared_mutex> lock(_state_mutex);
            auto state_function = [this]()
            {
                auto state = _state.load(std::memory_order_acquire);
                return state == worker_state::stopped || state == worker_state::error;
            };
            return _condition.wait_for(lock, timeout, state_function);
        }

        const std::string& get_worker_name() const {return _worker_name;}
        worker_state get_state() const {return _state.load(std::memory_order_acquire); }
        const worker_statistics &get_statistics() const {return _statistics;}
        void reset_statistics() {_statistics.reset();}
        /**
     * @brief 检查线程是否正在运行
     * @return `true` 正在运行，`false` 未运行
     */
        bool is_running() const
        {
            auto state = _state.load(std::memory_order_acquire);
            return state == worker_state::running;
        }
        /**
     * @brief 检查线程是否已停止
     * @return `true` 已停止，`false` 未停止
     */
        bool is_stopped() const
        {
            auto state = _state.load(std::memory_order_acquire);
            return state == worker_state::stopped;
        }
        void set_abnormal_callback(std::function<void(const std::string&, const std::exception& )> handler)
        {
            _abnormal_callback = std::move(handler);
        }
        void set_start_callback(std::function<void(const std::string &,safety_unit_pointer)> callback)
        {
            _unit_starts_callback = std::move(callback);
        }
        void set_finish_callback(std::function<void(const std::string &,safety_unit_pointer)> callback)
        {
            _unit_finish_callback = std::move(callback);
        }
        std::thread::id get_thread_id() const
        {
            if (_worker_thread)
            {
                return _worker_thread->get_id();
            }
            return std::thread::id{};
        }
        void set_thread_start(std::function<void()> callback)
        {
            _worker_starts_callback = std::move(callback);
        }
        void set_thread_stop(std::function<void()> callback)
        {
            _worker_finish_callback = std::move(callback);
        }
    protected:
        // 线程内部运行函数
        virtual void interior_run()
        {
            try
            {
                call_thread_start();
                while (!_stop.load(std::memory_order_acquire))
                {
                    auto task = get_next_task();
                    if (task)
                        execute_task(task);
                    else
                        handle_no_task();
                }
                call_thread_stop();
            }
            catch(const std::exception& e)
            {
                _state.store(worker_state::error, std::memory_order_release);
                if (_abnormal_callback) _abnormal_callback(_worker_name, e);
                else std::cerr << "Worker " << _worker_name << " encountered exception: " << e.what() << std::endl;
            }
            {
                std::unique_lock<std::shared_mutex> lock(_state_mutex);
                _state.store(worker_state::stopped, std::memory_order_release);
            }
            _condition.notify_all();
        }
        virtual safety_unit_pointer get_next_task()
        {
            if (_unit_rank)
                return _unit_rank->pop();
            return nullptr;
        }
        virtual void execute_task(safety_unit_pointer task)
        {
            if (!task) return;
            auto start_time = std::chrono::steady_clock::now();

            try
            {
                if (_unit_starts_callback)
                    _unit_starts_callback(_worker_name, task);

                if (task->is_timeout() == false && task->has_deadline())
                {
                    task->mark_timeout();
                    _statistics.tasks_failed.fetch_add(1, std::memory_order_relaxed);
                    return;
                }

                task->execute();

                auto end_time = std::chrono::steady_clock::now();
                auto execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

                _statistics.tasks_executed.fetch_add(1, std::memory_order_relaxed);
                _statistics.total_execution_time.fetch_add(execution_time, std::memory_order_relaxed);
                _statistics.last_task_time = end_time;

                if (_unit_finish_callback)
                    _unit_finish_callback(_worker_name, task);
            }
            catch (const std::exception &e)
            {
                _statistics.tasks_failed.fetch_add(1, std::memory_order_relaxed);

                if (_abnormal_callback)
                    _abnormal_callback(_worker_name, e);
                else
                    throw;
            }
        }
        // 处理无任务情况
        virtual void handle_no_task()
        {
            auto idle_start = std::chrono::steady_clock::now();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            auto idle_end = std::chrono::steady_clock::now();
            auto idle_time = std::chrono::duration_cast<std::chrono::microseconds>(idle_end - idle_start).count();
            _statistics.total_idle_time.fetch_add(idle_time, std::memory_order_relaxed);
        }
        // 线程启动时调用
        virtual void call_thread_start()
        {
            if(_worker_starts_callback)
                _worker_starts_callback();
        }
        // 线程停止时调用
        virtual void call_thread_stop()
        {
            if(_worker_finish_callback)
                _worker_finish_callback();
        }
    };
    class worker_adaptive : public worker_ordinary
    {
    private:
        static constexpr std::size_t MAX_SLEEP_TIME_MS = 100; ///< 最大休眠时间(毫秒)
        std::atomic<double> _load_factor{0.0}; // 负载因子
        std::atomic<std::size_t> _consecutive_empty_polls{0};  // 连续空轮询次数
        std::atomic<std::chrono::milliseconds> _adaptive_sleep_time{std::chrono::milliseconds(1)}; // 自适应休眠时间 
    public:
        worker_adaptive(const std::string& name, safety_rank_pointer rank) : worker_ordinary(name,std::move(rank)){}
        double get_load_factor() const
        {
            return _load_factor.load(std::memory_order_acquire);
        }
        void set_load_factor(double load_factor)
        {
            _load_factor.store(load_factor, std::memory_order_release);
        }
        std::chrono::milliseconds get_adaptive_sleep_time() const
        {
            return _adaptive_sleep_time.load(std::memory_order_acquire);
        }
        void set_adaptive_sleep_time(std::chrono::milliseconds sleep_time)
        {
            _adaptive_sleep_time.store(sleep_time, std::memory_order_release);
        }
    protected:
        safety_unit_pointer get_next_task() override
        {
            if (!_unit_rank)
                return nullptr;
            auto load = _load_factor.load(std::memory_order_acquire);
            auto timeout = std::chrono::milliseconds(static_cast<long>(50 + load * 50));

            auto task = _unit_rank->try_pop_for(timeout);
            if (task)
            {
                // 获取到任务，重置空轮询计数
                _consecutive_empty_polls.store(0, std::memory_order_relaxed);
                update_load_factor(true);
            }
            else
            {
                // 未获取到任务，增加空轮询计数
                auto empty_polls = _consecutive_empty_polls.fetch_add(1, std::memory_order_relaxed);
                update_load_factor(false);
                adjust_sleep_time(empty_polls + 1);
            }
            return task;
        }
        void handle_no_task() override
        {
            auto sleep_time = _adaptive_sleep_time.load(std::memory_order_acquire);

            auto idle_start = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(sleep_time);
            auto idle_end = std::chrono::steady_clock::now();

            auto idle_time = std::chrono::duration_cast<std::chrono::microseconds>(idle_end - idle_start).count();
            _statistics.total_idle_time.fetch_add(idle_time, std::memory_order_relaxed);
        }
    private:
        void adjust_sleep_time(std::size_t empty_polls)
        {
            std::size_t sleep_ms = std::min(empty_polls / 10, MAX_SLEEP_TIME_MS);
            _adaptive_sleep_time.store(std::chrono::milliseconds(sleep_ms), std::memory_order_release);
        }
        void update_load_factor(bool got_task)
        {
            // 使用指数移动平均更新负载因子
            constexpr double alpha = 0.1; // 平滑因子
            auto current_load = _load_factor.load(std::memory_order_acquire);
            auto new_sample = got_task ? 1.0 : 0.0;
            auto new_load = alpha * new_sample + (1.0 - alpha) * current_load;
            _load_factor.store(new_load, std::memory_order_release);
        }
    };
    std::unique_ptr<worker_adaptive> make_worker_adaptive(const std::string& worker_name, safety_rank_pointer worker_rank)
    {
        return std::make_unique<worker_adaptive>(worker_name, std::move(worker_rank));
    }
    std::unique_ptr<worker_ordinary> make_worker_ordinary(const std::string& worker_name, safety_rank_pointer worker_rank)
    {
        return std::make_unique<worker_ordinary>(worker_name, std::move(worker_rank));
    }
}