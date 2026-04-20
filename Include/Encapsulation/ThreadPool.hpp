#include <thread>
#include <iostream>
#include <functional>  //函数包装器
#include <vector>
#include <string>
#include <shared_mutex>
#include <atomic>      //原子操作
#include <queue>
#include <future>      //异步函数容器
#include <stdexcept>   //异常
#include <concepts>    //概念/ 约束特定的函数传参类型
#include <memory>      //智能指针
#include <type_traits> //类型萃取
#include <chrono>      //时间
#include "Syncs.hpp"   //MPMC队列

using task_function = std::function<void()>;
namespace con
{
    /**
   * @brief 线程池
   * @note - 线程池的线程数是根据任务的数量自动动态调整的
   */
    class thread_pool
    {
        struct thread_status
        {
            std::jthread thread;
            std::stop_source stopSrc; //令牌
            std::chrono::steady_clock::time_point lastActive; //上次活动时间
        };

        std::string name; //线程池标识

        static constexpr uint64_t defaultThreads = 5;
        static constexpr uint64_t defaultMinThreads = 1;
        static constexpr uint64_t defaultMaxThreads = 32;

        static constexpr std::chrono::milliseconds dormancy = std::chrono::milliseconds(5);   //线程休眠时间
        static constexpr std::chrono::milliseconds frequency = std::chrono::milliseconds(1000);//调整频率
        static constexpr std::chrono::milliseconds timeout   = std::chrono::milliseconds(20);  //线程超时时间

        mutable std::mutex mutex; //线程池锁
        mutable std::mutex nameMutex; //线程池标识称锁
        mutable std::shared_mutex exclusiveMutex;  //回调函数锁

        uint64_t maxThreads;
        uint64_t minThreads;

        std::condition_variable condition;
        std::atomic<bool> shutdown{false}; //关闭标识

        std::atomic<uint64_t> executeThreads;   //运行线程数
        std::atomic<uint64_t> closureThreads;   //空闲线程数

        std::jthread monitoringThread; //后台监控线程
        pros_cons_queue<task_function> tasks; //任务队列
        alignas(CACHE_ALIGNMENT) std::vector<thread_status> workersThread; //线程池

        std::function<void(const std::exception &)> exceptionCallback; //异常回调函数

        /**
     * @brief - 扩容到指定的线程数
     * @brief - 只增加超过最大线程数的空闲线程
     */
        void expandCapacity(uint64_t counter)
        {
            if (counter == 0)
                return;
            std::lock_guard<std::mutex> lock(mutex);
            for (uint64_t i = 0; i < counter; ++i)
            {
                thread_status initial;
                initial.lastActive = std::chrono::steady_clock::now();
                workersThread.emplace_back(std::move(initial));
                auto &worker = workersThread.back();
                auto task = [this, &worker]()
                {
                    this->threadTask(worker.stopSrc.get_token(), std::ref(worker));
                };
                worker.thread = std::jthread(task);
                ++closureThreads;
            }
        }
        /**
     * @brief - 缩容指定的线程数
     * @brief - 只停止超过最小线程数的空闲线程
     */
        void shrinkCapacity(uint64_t counter)
        {
            if (counter == 0 || shutdown)
                return;
            std::lock_guard<std::mutex> lock(mutex);
            uint64_t removed = 0;
            auto now = std::chrono::steady_clock::now();
            auto it = workersThread.begin();
            uint64_t currentThreads = executeThreads + closureThreads;
            while (removed < counter && it != workersThread.end())
            {
                if (currentThreads > minThreads && (now - it->lastActive) >= timeout)
                {
                    it->stopSrc.request_stop();
                    if (it->thread.joinable())
                    {
                        it->thread.join();
                    }
                    it = workersThread.erase(it);
                    ++removed;
                    --closureThreads;
                    --currentThreads;
                }
                else
                    ++it;
            }
        }
        /**
     * @brief 工作线程任务逻辑
     * @brief - 循环获取任务并执行
     * @brief - 处理线程状态转换（空闲/工作）
     * @brief - 响应停止请求
     */
        void threadTask(std::stop_token stopTok, thread_status &status)
        {
            bool idle = true;
            while (!stopTok.stop_requested() && !shutdown)
            {
                task_function task;
                if (tasks.pop(task))
                {
                    idle ? (closureThreads -= 1, executeThreads += 1, idle = false) : false;
                    try
                    {
                        task();
                    }
                    catch (const std::exception &mistake)
                    {
                        std::unique_lock<std::shared_mutex> lock(exclusiveMutex);
                        if (exceptionCallback)
                            exceptionCallback(mistake);
                        else
                            std::cerr << mistake.what() << '\n';
                    }
                    status.lastActive = std::chrono::steady_clock::now();
                }
                else
                {
                    if (!idle)
                    {
                        executeThreads -= 1;
                        closureThreads += 1;
                        idle = true;
                        status.lastActive = std::chrono::steady_clock::now();
                    }
                    std::unique_lock<std::mutex> lock(mutex);
                    auto waitFunction = [this, stopTok]()
                    {
                        return stopTok.stop_requested() || !tasks.empty() || shutdown;
                    };
                    condition.wait_for(lock, dormancy, waitFunction);
                }
            }
            idle ? --closureThreads : --executeThreads;
        }
        /**
     * @brief 监控线程池任务
     * @brief - 根据任务数量动态调整线程池大小
     * @brief - 调整频率为500ms调整一次
     * @note - 扩容逻辑：任务数超过当前线程数的两倍时，增加线程数
     * @note - 缩容逻辑：任务数少于当前线程数时，减少线程数
     */
        void monitoringThreadFunc(std::stop_token stopTok)
        {
            while (!stopTok.stop_requested() && !shutdown)
            {
                std::this_thread::sleep_for(frequency);
                uint64_t currentThreads = executeThreads + closureThreads;
                uint64_t tasksCount = tasks.size();
                if (tasksCount > executeThreads * 2 && currentThreads < maxThreads)
                {
                    const uint64_t standardNumber = (tasksCount + 1) / 2;
                    const uint64_t increaseThreads = std::min(standardNumber, maxThreads - currentThreads);
                    expandCapacity(increaseThreads);
                }
                else if (tasksCount < executeThreads && currentThreads > minThreads)
                {
                    const uint64_t standardNumber = currentThreads - minThreads;
                    const uint64_t decreaseThreads = std::min(standardNumber, closureThreads.load());
                    shrinkCapacity(decreaseThreads);
                }
            }
        }
    public:
        /**
     * @brief 构造函数：支持自定义线程池参数
     * @param initThreads 初始线程数（默认5）
     * @param minthreads 最小线程数（默认1）
     * @param maxthreads 最大线程数（默认32）
     */
        thread_pool(uint64_t initThreads = defaultThreads, uint64_t min = defaultMinThreads,
                uint64_t max = defaultMaxThreads) : maxThreads(max), minThreads(min), executeThreads(0),
                                        closureThreads(0), tasks(max * 2), workersThread(max)
        {
            if (initThreads > max || initThreads < min || min > max)
            {
                throw std::invalid_argument("非法参数");
            }
            expandCapacity(initThreads);
            auto monitoring = [this](std::stop_token stopTok)
            {
                this->monitoringThreadFunc(stopTok);
            };
            monitoringThread = std::jthread(monitoring);
        }
        thread_pool(const thread_pool &) = delete;
        thread_pool(thread_pool &&) = delete;
        thread_pool &operator=(const thread_pool &) = delete;
        thread_pool &operator=(thread_pool &&) = delete;
        /**
     * @brief 提交任务到线程池
     * @param task 任务函数
     * @param args 任务参数
     * @return `std::future<return_type>` 任务结果
     */
        template <class... Args, std::invocable<Args...> func>
        auto submit(func &&taskValue, Args &&...args)
        -> std::future<std::invoke_result_t<func, Args...>>
        {
            using return_type = std::invoke_result_t<func, Args...>;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<func>(taskValue), std::forward<Args>(args)...));
            std::future<return_type> result = task->get_future();
            tasks.push([task](){ (*task)(); });
            condition.notify_one();
            return result;
        }
        /**
     * @brief 获取当前正在执行任务的线程数
     */
        uint64_t activeThreads() const 
        {
            return executeThreads.load(std::memory_order_acquire);
        }

        /**
     * @brief 获取当前空闲线程数
     */
        uint64_t idleThreads() const 
        {
            return closureThreads.load(std::memory_order_acquire);
        }

        /**
     * @brief 获取当前线程池中线程数量
     */
        uint64_t totalThreads() const 
        {
            return activeThreads() + idleThreads();
        }
        /**
     * @brief 设置任务异常的回调函数
     */
        void setExceptionHandler(std::function<void(const std::exception &)> errorCallback)
        {
            std::unique_lock<std::shared_mutex> lock(exclusiveMutex);
            exceptionCallback = std::move(errorCallback);
        }
        /**
     * @brief 设置线程池标识
     */
        void setThreadName(const std::string &name)
        {
            std::lock_guard<std::mutex> lock(nameMutex);
            name = std::move(name);
        }
        /**
     * @brief  获取线程池标识
     */
        std::string getThreadName() const
        {
            std::lock_guard<std::mutex> lock(nameMutex);
            return name;
        }
        /**
     * @brief 关闭任务队列
     */
        void closeTaskQueue()
        {
            tasks.close();
        }
        /**
     * @brief 启用任务队列
     */
        void enableTaskQueue()
        {
            tasks.enable();
        }
        /**
     * @brief 清空任务队列
     */
        void clearTaskQueue()
        {

            tasks.clear();
        }
        /**
     * @brief 获取当前线程池中的任务数
     */
        uint64_t getTaskSize() const
        {
            return tasks.size();
        }
        /**
     * @brief 设置线程池最小线程数
     */
        bool setMinThreads(uint64_t min)
        {
            if(min == 0 || min > maxThreads || shutdown)
            {
                return false;
            }
            std::lock_guard<std::mutex> lock(mutex);
            minThreads = min;
            uint64_t currentThreads = totalThreads();
            if (currentThreads < min)
            {
                expandCapacity(min - currentThreads);
            }
            return true;
        }
        /**
     * @brief 设置线程池最大线程数
     */
        bool setMaxThreads(uint64_t max)
        {
            if(max == 0 || max < minThreads || shutdown)
            {
                return false;
            }
            std::lock_guard<std::mutex> lock(mutex);
            maxThreads = max;
            uint64_t currentThreads = totalThreads();
            if (currentThreads > max)
            {
                shrinkCapacity(currentThreads - max);
            }
            return true;
        }
        /**
     * @brief  增加线程数
     * @param increaseThreads 
     */
        void increaseThreads(uint64_t increaseThreads)
        {
            if(increaseThreads == 0 || shutdown) return;
            expandCapacity(increaseThreads);
        }
        /**
     * @brief 减少线程数
     * @param decreaseThreads 
     */
        void decreaseThreads(uint64_t decreaseThreads)
        {
            if(decreaseThreads == 0 || shutdown) return;
            shrinkCapacity(decreaseThreads);
        }
        /**
     * @brief 析构函数：销毁线程池
     */
        ~thread_pool()
        {
            shutdown.store(true, std::memory_order_release);
            if (monitoringThread.joinable())
            {
                monitoringThread.request_stop();  
                monitoringThread.join();       
            }
            tasks.close();
            {
                std::lock_guard<std::mutex> lock(mutex);
                for (auto& worker : workersThread)
                {
                        worker.stopSrc.request_stop(); 
                }
                condition.notify_all();
            }
            std::vector<std::jthread> threadsToJoin;
            {
                std::lock_guard<std::mutex> lock(mutex);
                for (auto& worker : workersThread)
                {
                    if (worker.thread.joinable())
                    {
                        threadsToJoin.emplace_back(std::move(worker.thread));  
                    }
                }
                workersThread.clear(); 
            }
            for (auto& thread : threadsToJoin)
            {
                thread.join();
            }
        }
    };
}
