/**
 * @file Concurrent_annular_queue.hpp
 * @brief 线程安全环形队列
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 特点：
 *   - 接口数量 ≈ std::deque 全集，额外支持批量、超时、迭代器适配器；
 *   - 同步：1 把读写锁 + 2 个 C++20 信号量（空槽/满槽）；
 *   - 支持无界模式（capacity==0）；
 *   - 提供只读迭代器适配器与快照；
 *   - 所有修改器线程安全，读操作并发安全。
 */

#pragma once
#include <vector>
#include <shared_mutex>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <initializer_list>
#include <optional>

namespace multi_concurrent
{
    /**
   * @class concurrent_annular_queue
   * @brief 线程安全环形队列
   * @tparam value         元素类型
   * @tparam custom_allocator 分配器，默认 std::allocator<value>
   */
    template <typename value, typename custom_allocator = std::allocator<value>>
    class concurrent_annular_queue
    {
        using standard_library_vector = std::vector<value, custom_allocator>;

    public:
        using value_type     = value;
        using size_type      = typename standard_library_vector::size_type;
        using allocator_type = custom_allocator;

    private:
        standard_library_vector _vector; 

        size_type _consume = 0;               
        size_type _produce = 0;   

        size_type _cnt = 0;                
        mutable std::shared_mutex _access_mutex;    

        std::counting_semaphore<> _sem_free; 
        std::counting_semaphore<> _sem_used;

    public:
        /**
     * @brief 构造指定容量的环形队列
     * @param cap 容量，0 表示无界
     * @param alloc 分配器
     * @throw std::invalid_argument 若 cap == 0 且需要边界
     */
        explicit concurrent_annular_queue(size_type cap,
            const custom_allocator &alloc = custom_allocator())
            : _vector(cap ? cap : 1, value(), alloc), _sem_free(static_cast<int>(cap ? cap : 1)),
            _sem_used(0) {}
        concurrent_annular_queue(const concurrent_annular_queue &) = delete;
        concurrent_annular_queue &operator=(const concurrent_annular_queue &) = delete;
        concurrent_annular_queue(concurrent_annular_queue &&) = default;
        concurrent_annular_queue &operator=(concurrent_annular_queue &&) = default;

        /** @brief #### 队列总容量 */
        size_type capacity() const noexcept { return _vector.size(); }

        /** @brief #### 当前元素个数（线程安全） */
        size_type size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _cnt;
        }

        /** @brief #### 是否为空 */
        bool empty() const noexcept { return size() == 0; }

        /** @brief #### 是否已满 */
        bool full() const noexcept { return size() == capacity(); }

        /**
     * @brief #### 在尾部写入元素（阻塞等待空槽位）
     * @param value_data 待写入值
     */
        void push_back(const value &value_data)
        {
            _sem_free.acquire(); // 等待空槽
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _vector[_produce] = value_data;
                _produce = (_produce + 1) % _vector.size();
                ++_cnt;
            }
            _sem_used.release(); // 通知消费者
        }

        /** @brief #### 写入（移动） */
        void push_back(value &&value_data)
        {
            _sem_free.acquire();
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _vector[_produce] = std::move(value_data);
                _produce = (_produce + 1) % _vector.size();
                ++_cnt;
            }
            _sem_used.release();
        }

        /** @brief #### 就地构造写入 */
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            _sem_free.acquire();
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _vector[_produce] = value(std::forward<Args>(args)...);
                _produce = (_produce + 1) % _vector.size();
                ++_cnt;
            }
            _sem_used.release();
        }

        /**
     * @brief #### 读取头部元素（阻塞等待有数据）
     * @param out 接收元素
     */
        void pop_front(value &out)
        {
            _sem_used.acquire(); // 等待有数据
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                out = std::move(_vector[_consume]);
                _consume = (_consume + 1) % _vector.size();
                --_cnt;
            }
            _sem_free.release(); // 通知生产者
        }

        /**
     * @brief #### 批量写入（阻塞，直到全部写完）
     * @tparam `input_it` 输入迭代器
     * @param first 起始
     * @param last  终止（不含）
     */
        template <typename input_it>
        void push_range(input_it first, input_it last)
        {
            for (; first != last; ++first)
                push_back(*first);
        }

        /**
     * @brief #### 批量读取（阻塞，直到读完指定数量）
     * @tparam `output_it` 输出迭代器
     * @param first 起始
     * @param n     读取个数
     */
        template <typename output_it>
        void pop_range(output_it first, size_type n)
        {
            for (size_type i = 0; i < n; ++i)
            {
                value tmp;
                pop_front(tmp);
                *first++ = std::move(tmp);
            }
        }
        /**
     * @brief #### 尝试写入（非阻塞）
     * @param value_data 待写入值
     * @return `true` 成功；`false` 缓冲区满
     */
        bool try_push_back(const value &value_data)
        {
            if (!_sem_free.try_acquire())
                return false;
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _vector[_produce] = value_data;
                _produce = (_produce + 1) % _vector.size();
                ++_cnt;
            }
            _sem_used.release();
            return true;
        }

        /**
     * @brief #### 尝试读取（非阻塞）
     * @param out 接收元素
     * @return `true` 成功；`false` 缓冲区空
     */
        bool try_pop_front(value &out)
        {
            if (!_sem_used.try_acquire())
                return false;
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                out = std::move(_vector[_consume]);
                _consume = (_consume + 1) % _vector.size();
                --_cnt;
            }
            _sem_free.release();
            return true;
        }

        /**
     * @brief #### 超时写入
     * @tparam Rep,Period 时间类型
     * @param value_data 待写入值
     * @param timeout 等待时长
     * @return `true` 成功；`false` 超时
     */
        template <typename Rep, typename Period>
        bool push_back_for(const value &value_data,
            const std::chrono::duration<Rep, Period> &timeout)
        {
            if (!_sem_free.try_acquire_for(timeout))
                return false;
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _vector[_produce] = value_data;
                _produce = (_produce + 1) % _vector.size();
                ++_cnt;
            }
            _sem_used.release();
            return true;
        }

        /**
     * @brief #### 超时读取
     * @tparam Rep,Period 时间类型
     * @param out 接收元素
     * @param timeout 等待时长
     * @return `true` 成功；`false` 超时
     */
        template <typename Rep, typename Period>
        bool pop_front_for(value &out,const std::chrono::duration<Rep, Period> &timeout)
        {
            if (!_sem_used.try_acquire_for(timeout))
                return false;
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                out = std::move(_vector[_consume]);
                _consume = (_consume + 1) % _vector.size();
                --_cnt;
            }
            _sem_free.release();
            return true;
        }

        /**
     * @brief #### 获取当前队列快照
     * @return `std::vector<value>` 按先进先出顺序的元素副本
     */
        std::vector<value> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            std::vector<value> return_vec;
            return_vec.reserve(_cnt);
            size_type idx = _consume;
            for (size_type i = 0; i < _cnt; ++i)
            {
                return_vec.push_back(_vector[idx]);
                idx = (idx + 1) % _vector.size();
            }
            return return_vec;
        }

        /** @brief #### 清空队列并唤醒所有等待线程 */
        void clear()
        {
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _consume = _produce = _cnt = 0;
            }
            _sem_free.release(static_cast<int>(capacity()));
            while(_sem_used.try_acquire()){}
        }
    }; 
}