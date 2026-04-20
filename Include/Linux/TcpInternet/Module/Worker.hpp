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
        std::unique_ptr<std::jthread> _workerThread; // 线程对象

        std::atomic<bool> _stop{false}; // 停止标志
        std::atomic<bool> _detached{false}; // 分离标志
        std::atomic<worker_state> _state{worker_state::idle}; // 状态标志

        std::string _workerName; // 线程名称
        worker_statistics _statistics; // 统计信息

        std::shared_mutex _stateMutex; // 状态互斥锁
        std::condition_variable _condition; // 条件变量

        safety_rank_pointer _unitRank; // 任务队列

        std::function<void(const std::string&, safety_unit_pointer)> _unitStartsCallback; // 任务开始回调
        std::function<void(const std::string&, safety_unit_pointer)> _unitFinishCallback; // 任务完成回调

        std::function<void()> _workerStartsCallback; // 线程开始回调
        std::function<void()> _workerFinishCallback; // 线程完成回调

        std::function<void(const std::string&, const std::exception&)> _abnormalCallback; // 任务异常回调 
    public:
        worker_ordinary(const std::string& name, safety_rank_pointer rank) 
        : _workerName(name), _unitRank(std::move(rank)) {}
        virtual ~worker_ordinary()
        {
            if(!_detached.load(std::memory_order_acquire))
            {
                stop();
                if(_workerThread && _workerThread->joinable())
                    _workerThread->join();
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
            std::unique_lock<std::shared_mutex> lock(_stateMutex);
            if (_state.load(std::memory_order_acquire) != worker_state::idle)
            {
                return false;
            }
            try
            {
                _stop.store(false, std::memory_order_release);

                _workerThread = std::make_unique<std::jthread>(&worker_ordinary::interiorRun, this);

                _state.store(worker_state::running, std::memory_order_release);
                _statistics.start_time = std::chrono::steady_clock::now();
                lock.unlock();
                _condition.notify_all();
                return true;
            }
            catch (const std::exception &e)
            {
                _state.store(worker_state::error, std::memory_order_release);
                if (_abnormalCallback)
                {
                    _abnormalCallback(_workerName, e);
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
                    std::unique_lock<std::shared_mutex> lock(_stateMutex);
                    _state.store(worker_state::stopping, std::memory_order_release);
                }
                _condition.notify_all();

                if (_workerThread && _workerThread->joinable() && wait_for_completion)
                {
                    _workerThread->join();
                }
            }
        }
        // 分离工作线程
        virtual void detach()
        {
            if (_workerThread && _workerThread->joinable())
            {
                _workerThread->detach();
                _detached.store(true, std::memory_order_release);
            }
        }
        /**
     * @brief 等待线程结束
     * @param timeout 超时时间
     * @return `true` 线程已结束，`false` 超时
     */
        template <typename rep, typename period>
        bool waitForStop(const std::chrono::duration<rep, period> &timeout)
        {
            std::unique_lock<std::shared_mutex> lock(_stateMutex);
            auto state_function = [this]()
            {
                auto state = _state.load(std::memory_order_acquire);
                return state == worker_state::stopped || state == worker_state::error;
            };
            return _condition.wait_for(lock, timeout, state_function);
        }

        const std::string& getWorkerName() const {return _workerName;}
        worker_state get_state() const {return _state.load(std::memory_order_acquire); }
        const worker_statistics &get_statistics() const {return _statistics;}
        void resetStatistics() {_statistics.reset();}
        /**
     * @brief 检查线程是否正在运行
     * @return `true` 正在运行，`false` 未运行
     */
        bool isRunning() const
        {
            auto state = _state.load(std::memory_order_acquire);
            return state == worker_state::running;
        }
        /**
     * @brief 检查线程是否已停止
     * @return `true` 已停止，`false` 未停止
     */
        bool isStopped() const
        {
            auto state = _state.load(std::memory_order_acquire);
            return state == worker_state::stopped;
        }
        void setAbnormalCallback(std::function<void(const std::string&, const std::exception& )> handler)
        {
            _abnormalCallback = std::move(handler);
        }
        void setStartCallback(std::function<void(const std::string &,safety_unit_pointer)> callback)
        {
            _unitStartsCallback = std::move(callback);
        }
        void setFinishCallback(std::function<void(const std::string &,safety_unit_pointer)> callback)
        {
            _unitFinishCallback = std::move(callback);
        }
        std::thread::id get_thread_id() const
        {
            if (_workerThread)
            {
                return _workerThread->get_id();
            }
            return std::thread::id{};
        }
        void setThreadStart(std::function<void()> callback)
        {
            _workerStartsCallback = std::move(callback);
        }
        void setThreadStop(std::function<void()> callback)
        {
            _workerFinishCallback = std::move(callback);
        }
    protected:
        // 线程内部运行函数
        virtual void interiorRun()
        {
            try
            {
                callThreadStart();
                while (!_stop.load(std::memory_order_acquire))
                {
                    auto task = getNextTask();
                    if (task)
                        executeTask(task);
                    else
                        handleNoTask();
                }
                callThreadStop();
            }
            catch(const std::exception& e)
            {
                _state.store(worker_state::error, std::memory_order_release);
                if (_abnormalCallback) _abnormalCallback(_workerName, e);
                else std::cerr << "Worker " << _workerName << " encountered exception: " << e.what() << std::endl;
            }
            {
                std::unique_lock<std::shared_mutex> lock(_stateMutex);
                _state.store(worker_state::stopped, std::memory_order_release);
            }
            _condition.notify_all();
        }
        virtual safety_unit_pointer getNextTask()
        {
            if (_unitRank)
                return _unitRank->pop();
            return nullptr;
        }
        virtual void executeTask(safety_unit_pointer task)
        {
            if (!task) return;
            auto start_time = std::chrono::steady_clock::now();

            try
            {
                if (_unitStartsCallback)
                    _unitStartsCallback(_workerName, task);

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

                if (_unitFinishCallback)
                    _unitFinishCallback(_workerName, task);
            }
            catch (const std::exception &e)
            {
                _statistics.tasks_failed.fetch_add(1, std::memory_order_relaxed);

                if (_abnormalCallback)
                    _abnormalCallback(_workerName, e);
                else
                    throw;
            }
        }
        // 处理无任务情况
        virtual void handleNoTask()
        {
            auto idle_start = std::chrono::steady_clock::now();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            auto idle_end = std::chrono::steady_clock::now();
            auto idle_time = std::chrono::duration_cast<std::chrono::microseconds>(idle_end - idle_start).count();
            _statistics.total_idle_time.fetch_add(idle_time, std::memory_order_relaxed);
        }
        // 线程启动时调用
        virtual void callThreadStart()
        {
            if(_workerStartsCallback)
                _workerStartsCallback();
        }
        // 线程停止时调用
        virtual void callThreadStop()
        {
            if(_workerFinishCallback)
                _workerFinishCallback();
        }
    };
    class worker_adaptive : public worker_ordinary
    {
    private:
        static constexpr std::size_t MAX_SLEEP_TIME_MS = 100; ///< 最大休眠时间(毫秒)
        std::atomic<double> _loadFactor{0.0}; // 负载因子
        std::atomic<std::size_t> _consecutiveEmptyPolls{0};  // 连续空轮询次数
        std::atomic<std::chrono::milliseconds> _adaptiveSleepTime{std::chrono::milliseconds(1)}; // 自适应休眠时间 
    public:
        worker_adaptive(const std::string& name, safety_rank_pointer rank) : worker_ordinary(name,std::move(rank)){}
        double getLoadFactor() const
        {
            return _loadFactor.load(std::memory_order_acquire);
        }
        void setLoadFactor(double load_factor)
        {
            _loadFactor.store(load_factor, std::memory_order_release);
        }
        std::chrono::milliseconds getAdaptiveSleepTime() const
        {
            return _adaptiveSleepTime.load(std::memory_order_acquire);
        }
        void setAdaptiveSleepTime(std::chrono::milliseconds sleep_time)
        {
            _adaptiveSleepTime.store(sleep_time, std::memory_order_release);
        }
    protected:
        safety_unit_pointer getNextTask() override
        {
            if (!_unitRank)
                return nullptr;
            auto load = _loadFactor.load(std::memory_order_acquire);
            auto timeout = std::chrono::milliseconds(static_cast<long>(50 + load * 50));

            auto task = _unitRank->try_pop_for(timeout);
            if (task)
            {
                // 获取到任务，重置空轮询计数
                _consecutiveEmptyPolls.store(0, std::memory_order_relaxed);
                updateLoadFactor(true);
            }
            else
            {
                // 未获取到任务，增加空轮询计数
                auto empty_polls = _consecutiveEmptyPolls.fetch_add(1, std::memory_order_relaxed);
                updateLoadFactor(false);
                adjustSleepTime(empty_polls + 1);
            }
            return task;
        }
        void handleNoTask() override
        {
            auto sleep_time = _adaptiveSleepTime.load(std::memory_order_acquire);

            auto idle_start = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(sleep_time);
            auto idle_end = std::chrono::steady_clock::now();

            auto idle_time = std::chrono::duration_cast<std::chrono::microseconds>(idle_end - idle_start).count();
            _statistics.total_idle_time.fetch_add(idle_time, std::memory_order_relaxed);
        }
    private:
        void adjustSleepTime(std::size_t empty_polls)
        {
            std::size_t sleep_ms = std::min(empty_polls / 10, MAX_SLEEP_TIME_MS);
            _adaptiveSleepTime.store(std::chrono::milliseconds(sleep_ms), std::memory_order_release);
        }
        void updateLoadFactor(bool got_task)
        {
            // 使用指数移动平均更新负载因子
            constexpr double alpha = 0.1; // 平滑因子
            auto current_load = _loadFactor.load(std::memory_order_acquire);
            auto new_sample = got_task ? 1.0 : 0.0;
            auto new_load = alpha * new_sample + (1.0 - alpha) * current_load;
            _loadFactor.store(new_load, std::memory_order_release);
        }
    };
    std::unique_ptr<worker_adaptive> makeWorkerAdaptive(const std::string& worker_name, safety_rank_pointer worker_rank)
    {
        return std::make_unique<worker_adaptive>(worker_name, std::move(worker_rank));
    }
    std::unique_ptr<worker_ordinary> makeWorkerOrdinary(const std::string& worker_name, safety_rank_pointer worker_rank)
    {
        return std::make_unique<worker_ordinary>(worker_name, std::move(worker_rank));
    }
}

namespace pool 
{
    using internals::structure_w::worker_adaptive;
    using internals::structure_w::worker_ordinary;

    using internals::structure_w::makeWorkerAdaptive;
    using internals::structure_w::makeWorkerOrdinary;

}