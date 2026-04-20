/**
 * @file Concurrent_queue.hpp
 * @brief 线程安全的无界 FIFO 队列（多生产者多消费者）
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 内部基于 std::queue + std::mutex + std::condition_variable
 * 支持任意数量的生产者线程与消费者线程并发访问。
 * 当队列为空时，pop() 会阻塞等待；当队列有元素时，pop() 立即返回。
 */

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>

namespace multi_concurrent
{
    /**
   * @class concurrent_queue
   * @brief 线程安全的通用先进先出队列
   * @tparam value 元素类型
   */
    template <typename value>
    class concurrent_queue
    {
    private:
        std::queue<value> _queue;
        mutable std::mutex _access_mutex;
        std::condition_variable _cv_empty;

    public:
        concurrent_queue() = default;
        ~concurrent_queue() 
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _cv_empty.notify_all();
        }
        concurrent_queue(const concurrent_queue &) = delete;
        concurrent_queue &operator=(const concurrent_queue &) = delete;
        concurrent_queue(concurrent_queue &&) = default;
        concurrent_queue &operator=(concurrent_queue &&) = default;

        /**
     * @brief #### 获取队列当前元素个数
     * @return 元素数量
     */
        size_t size() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            return _queue.size();
        }

        /** @brief #### 判断队列是否为空 */
        bool empty() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            return _queue.empty();
        }

        /**
     * @brief #### 入队（拷贝）
     * @param item 待入队元素
     */
        void push(const value &item)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _queue.push(item);
            _cv_empty.notify_one(); // 唤醒一个等待的消费者
        }

        /** @brief #### 入队（移动） */
        void push(value &&item)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _queue.push(std::move(item));
            _cv_empty.notify_one();
        }

        /**
     * @brief #### 就地构造入队
     * @param args 构造元素所需参数
     */
        template <typename... Args>
        void emplace(Args &&...args)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _queue.emplace(std::forward<Args>(args)...);
            _cv_empty.notify_one();
        }

        /**
     * @brief #### 出队（阻塞等待）
     * @param out 接收出队元素的引用
     * @return `true` 成功出队；`false` 仅在队列被销毁时返回（本实现永远不返回 `false`）
     */
        bool pop(value &out)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _cv_empty.wait(lock, [this]{ return !_queue.empty(); });
            out = std::move(_queue.front());
            _queue.pop();
            return true;
        }

        /**
     * @brief #### 尝试出队（非阻塞）
     * @param out 接收出队元素的引用
     * @return `true` 成功出队；`false` 队列空
     */
        bool try_pop(value &out)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            if (_queue.empty())
                return false;
            out = std::move(_queue.front());
            _queue.pop();
            return true;
        }

        /**
     * @brief #### 清空队列
     * @note  会唤醒所有等待 `pop` 的线程，但此时队列为空，它们会继续等待
     */
        void clear()
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            while (!_queue.empty())
                _queue.pop();
            _cv_empty.notify_all();
        }
        /**
     * @brief #### 获取当前队列快照
     * @return `std::vector<value>` 按` FIFO `顺序的副本
     */
        std::vector<value> snapshot() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            std::vector<value> snapshot;
            std::queue<value> temp_queue = _queue;
            while (!temp_queue.empty())
            {
                snapshot.push_back(temp_queue.front());
                temp_queue.pop();
            }
            return snapshot;
        }
    };
}