/**
 * @file Concurrent_priority_queue.hpp
 * @brief 线程安全的优先级队列（多生产者多消费者）
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 内部使用 std::priority_queue 作为底层容器，
 * 通过 std::mutex + std::condition_variable 实现线程安全：
 *   - push：若队列已满则阻塞等待；
 *   - pop：若队列为空则阻塞等待；
 *   - 支持任意数量的生产者和消费者并发访问。
 */

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <utility>

namespace multi_concurrent
{
    /**
   * @class concurrent_priority_queue
   * @brief 线程安全的优先级队列
   * @tparam value         元素类型
   * @tparam comparator   元素比较器，默认` std::less<value>`（大顶堆）
   * @tparam container    底层容器，默认 `std::vector<value>`
   */
    template <typename value, typename comparator = std::less<value>, typename container = std::vector<value>>
    class concurrent_priority_queue
    {
        using standard_library_priority_queue = std::priority_queue<value, container, comparator>;

    private:
        standard_library_priority_queue _priority_queue;
        mutable std::mutex _access_mutex;
        std::condition_variable _cv_not_full;
        std::condition_variable _cv_not_empty;
        std::size_t _max_cap;

    public:
        /**
     * @brief 构造可指定最大容量的优先级队列
     * @param max_capacity 最大元素个数，0 表示无界
     */
        explicit concurrent_priority_queue(std::size_t max_capacity = 0)
                : _max_cap(max_capacity) {}
        concurrent_priority_queue(const concurrent_priority_queue &) = delete;
        concurrent_priority_queue &operator=(const concurrent_priority_queue &) = delete;
        concurrent_priority_queue(concurrent_priority_queue &&) = default;
        concurrent_priority_queue &operator=(concurrent_priority_queue &&) = default;
        /**
     * @brief #### 获取当前队列元素个数
     * @return 元素数量
     */
        std::size_t size() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            return _priority_queue.size();
        }

        /** @brief #### 判断队列是否为空 */
        bool empty() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            return _priority_queue.empty();
        }

        /**
     * @brief #### 获取最大容量
     * @return 最大元素个数；0 表示无界
     */
        std::size_t max_capacity() const
        {
            return _max_cap;
        }

        /**
     * @brief #### 判断队列是否已满
     * @return `true` 已满；`false` 未满或无界
     */
        bool full() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            return _max_cap != 0 && _priority_queue.size() >= _max_cap;
        }

        /**
     * @brief #### 入队（拷贝）
     * @param value 待入队元素
     * @note 若队列已满将阻塞等待；入队后唤醒一个等待 `pop` 的线程
     */
        void push(const value &value_data)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            if (_max_cap != 0)
                _cv_not_full.wait(lock, [this]{ return _priority_queue.size() < _max_cap; });
            _priority_queue.push(value_data);
            _cv_not_empty.notify_one();
        }

        /** @brief #### 入队（移动） */
        void push(value &&value_data)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            if (_max_cap != 0)
                _cv_not_full.wait(lock, [this]{ return _priority_queue.size() < _max_cap; });
            _priority_queue.push(std::move(value_data));
            _cv_not_empty.notify_one();
        }

        /**
     * @brief #### 就地构造入队
     * @param args 构造元素所需参数
     */
        template <typename... Args>
        void emplace(Args &&...args)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            if (_max_cap != 0)
                _cv_not_full.wait(lock, [this]{ return _priority_queue.size() < _max_cap; });
            _priority_queue.emplace(std::forward<Args>(args)...);
            _cv_not_empty.notify_one();
        }

        /**
     * @brief #### 出队（阻塞等待） 
     * @param out 接收最高优先级元素的引用
     * @note 若队列为空将阻塞等待；出队后唤醒一个等待 push 的线程
     */
        void pop(value &out)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            _cv_not_empty.wait(lock, [this]{ return !_priority_queue.empty(); });
            out = std::move(_priority_queue.top());
            _priority_queue.pop();
            if (_max_cap != 0)
                _cv_not_full.notify_one(); // 队列空出一位，可唤醒 push
        }

        /**
     * @brief #### 尝试出队（非阻塞）
     * @param out 接收最高优先级元素的引用
     * @return true 成功出队；false 队列为空
     */
        bool try_pop(value &out)
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            if (_priority_queue.empty())
                return false;
            out = std::move(_priority_queue.top());
            _priority_queue.pop();
            if (_max_cap != 0)
                _cv_not_full.notify_one();
            return true;
        }

        /**
     * @brief #### 清空队列
     * @note  会唤醒所有等待 `pop` 的线程，但此时队列已空，它们将继续等待
     */
        void clear()
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            while (!_priority_queue.empty())
                _priority_queue.pop();
            _cv_not_full.notify_all();
        }

        /**
     * @brief #### 与另一线程安全优先级队列交换内容
     * @param other 另一个实例
     * @note  采用双锁避免死锁
     */
        void swap(concurrent_priority_queue &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::mutex> rhs_lock(other._access_mutex);
            _priority_queue.swap(other._priority_queue);
            std::swap(_max_cap, other._max_cap);
            lhs_lock.unlock();
            rhs_lock.unlock();
            _cv_not_empty.notify_all();
            _cv_not_full.notify_all();
            other._cv_not_empty.notify_all();
            other._cv_not_full.notify_all();
        }
        /**
     * @brief #### 获取当前队列快照（按优先级排序）
     * @return `std::vector<value>` 元素副本，顺序与优先级一致
     * @note  内部加锁，拷贝后立即释放，外部可安全遍历
     */
        std::vector<value> snapshot() const
        {
            std::unique_lock<std::mutex> lock(_access_mutex);
            std::vector<value> vec;
            vec.reserve(_priority_queue.size());
            standard_library_priority_queue tmp = _priority_queue;
            while (!tmp.empty())
            {
                vec.emplace_back(std::move(tmp.top()));
                tmp.pop();
            }
            return vec;
        }
    };
}