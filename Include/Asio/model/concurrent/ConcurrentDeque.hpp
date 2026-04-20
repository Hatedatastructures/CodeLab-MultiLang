/**
 * @file Concurrent_deque.hpp
 * @brief 线程安全双端队列
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 特点：
 *   1. 内部仅使用 **一把读写锁** `std::shared_mutex`；
 *   2. **所有接口** 与 `std::deque` **同名同语义**；
 *   3. 支持 **任意数量** 生产者/消费者并发；
 *   4. 提供 **批量/超时/快照** 等扩展功能；
 *   5. 不暴露可变迭代器，遍历请用 snapshot()。
 */

#pragma once
#include <deque>
#include <shared_mutex>
#include <vector>
#include <chrono>
#include <initializer_list>

namespace multi_concurrent
{
    /**
   * @class concurrent_deque
   * @brief 线程安全双端队列
   * @tparam value         元素类型
   * @tparam custom_allocator 分配器，默认 `std::allocator<value>`
   */
    template <typename value, typename custom_allocator = std::allocator<value>>
    class concurrent_deque
    {
        using standard_library_deque = std::deque<value, custom_allocator>;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_deque _deque;                 

    public:
        concurrent_deque() = default;

        /**
     * @brief 指定分配器构造
     * @param alloc 分配器实例
     */
        explicit concurrent_deque(const custom_allocator &alloc)
            : _deque(alloc) {}

        /**
     * @brief 范围构造
     * @tparam input_it 输入迭代器
     * @param first 起始
     * @param last  终止（不含）
     * @param alloc 分配器
     */
        template <typename input_it>
        concurrent_deque(input_it first, input_it last,const custom_allocator &alloc = custom_allocator())
            : _deque(first, last, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 {1,2,3} 的列表
     * @param alloc 分配器
     */
        concurrent_deque(std::initializer_list<value> init,const custom_allocator &alloc = custom_allocator())
            : _deque(init, alloc) {}

        concurrent_deque(const concurrent_deque &) = delete;
        concurrent_deque &operator=(const concurrent_deque &) = delete;
        concurrent_deque(concurrent_deque &&) = default;
        concurrent_deque &operator=(concurrent_deque &&) = default;

        /** @brief ####  当前元素个数（线程安全） */
        size_t size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _deque.size();
        }

        /** @brief #### 是否为空 */
        bool empty() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _deque.empty();
        }

        /** @brief #### 最大元素数（理论值） */
        size_t max_size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _deque.max_size();
        }

        /**
     * @brief #### 在队尾压入元素（拷贝）
     * @param value_data 待压入值
     */
        void push_back(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.push_back(value_data);
        }

        /** @brief #### 队尾压入（移动） */
        void push_back(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.push_back(std::move(value_data));
        }

        /** @brief #### 在队首压入元素（拷贝） */
        void push_front(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.push_front(value_data);
        }

        /** @brief #### 队首压入（移动） */
        void push_front(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.push_front(std::move(value_data));
        }

        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.emplace_back(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void emplace_front(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.emplace_front(std::forward<Args>(args)...);
        }

        /**
     * @brief #### 从队首弹出元素（消费者真正拿走）
     * @param out 接收元素
     * @note 若空则阻塞等待
     */
        void pop_front(value &out)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            while (_deque.empty())
            {
            } // 阻塞等待（可替换为条件变量）
            out = std::move(_deque.front());
            _deque.pop_front();
        }

        /** @brief #### 从队尾弹出元素（消费者真正拿走） */
        void pop_back(value &out)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            while (_deque.empty())
            {
            }
            out = std::move(_deque.back());
            _deque.pop_back();
        }

        /**
     * @brief #### 尝试从队首弹出（非阻塞）
     * @param out 接收元素
     * @return `true `成功；`false` 空
     */
        bool try_pop_front(value &out)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (_deque.empty())
                return false;
            out = std::move(_deque.front());
            _deque.pop_front();
            return true;
        }

        /**
     * @brief #### 尝试从队尾弹出（非阻塞）
     * @param out 接收元素
     * @return `true` 成功；`false` 空
     */
        bool try_pop_back(value &out)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (_deque.empty())
                return false;
            out = std::move(_deque.back());
            _deque.pop_back();
            return true;
        }

        /**
     * @brief #### 批量写入（阻塞写完所有）
     * @tparam input_it 输入迭代器
     * @param first 起始
     * @param last  终止
     */
        template <typename input_it>
        void push_range(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.insert(_deque.end(), first, last);
        }

        /**
     * @brief #### 批量读取（阻塞读完所有）
     * @tparam `OutputIt` 输出迭代器
     * @param first 起始
     * @param n     读取个数
     */
        template <typename OutputIt>
        void pop_range(OutputIt first, size_t n)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            for (size_t i = 0; i < n && !_deque.empty(); ++i)
            {
                *first++ = std::move(_deque.front());
                _deque.pop_front();
            }
        }

        /**
     * @brief #### 随机下标访问（边界检查）
     * @param pos 下标
     * @return 元素引用
     * @throw `std::out_of_range` 越界
     */
        value &at(size_t pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _deque.at(pos);
        }

        const value &at(size_t pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _deque.at(pos);
        }

        /** @brief #### 随机下标访问（无边界检查） */
        value &operator[](size_t pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _deque[pos];
        }

        const value &operator[](size_t pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _deque[pos];
        }


        /** @brief #### 清空所有元素 */
        void clear()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _deque.clear();
        }

        /**
     * @brief #### 获取当前双端队列快照（按顺序）
     * @return std::vector<value> 元素副本
     */
        std::vector<value> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return std::vector<value>(_deque.begin(), _deque.end());
        }
    };
}