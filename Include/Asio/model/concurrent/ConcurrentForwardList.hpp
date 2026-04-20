
#pragma once
#include <mutex>
#include <forward_list>
#include <shared_mutex>
#include <iterator>
#include <algorithm>

namespace multi_concurrent
{
    /**
   * @file Concurrent_forward_list.hpp
   * @brief 线程安全的单向链表容器
   * @details 对 std::forward_list 进行线程安全封装，支持多生产者多消费者并发访问。
   *          内部通过 std::shared_mutex 实现同步：读操作（如查找、遍历）加共享锁，
   *          写操作（如插入、删除）加独占锁，保证单向链表操作的线程安全。
   * @tparam value 元素类型
   * @tparam custom_allocator 分配器类型，默认 `std::allocator<value>`
   */
    template <typename value, typename custom_allocator = std::allocator<value>>
    class concurrent_forward_list
    {
    public:
        using value_type      = value;
        using size_type       = typename std::forward_list<value, custom_allocator>::size_type;
        using difference_type = typename std::forward_list<value, custom_allocator>::difference_type;
        using reference       = const value &; // 只读引用，避免外部修改破坏一致性
        using const_reference = const value &;
        using iterator        = typename std::forward_list<value, custom_allocator>::const_iterator;
        using const_iterator  = iterator;
        using allocator_type  = custom_allocator;

    private:
        mutable std::shared_mutex _mutex;
        std::forward_list<value, custom_allocator> _forward_list;

    public:
        /** @brief 默认构造空单向链表 */
        concurrent_forward_list() = default;

        /** @brief 指定分配器构造 */
        explicit concurrent_forward_list(const custom_allocator &alloc) : _forward_list(alloc) {}

        /** @brief 范围构造 */
        template <typename InputIt>
        concurrent_forward_list(InputIt first, InputIt last, const custom_allocator &alloc = custom_allocator())
            : _forward_list(first, last, alloc) {}

        /** @brief 初始化列表构造 */
        concurrent_forward_list(std::initializer_list<value> init, const custom_allocator &alloc = custom_allocator())
            : _forward_list(init, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_forward_list(const concurrent_forward_list &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._mutex);
            _forward_list = other._forward_list;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_forward_list &operator=(const concurrent_forward_list &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._mutex);
                _forward_list = other._forward_list;
            }
            return *this;
        }

        /** @brief 移动构造（禁用，避免线程安全隐患） */
        concurrent_forward_list(concurrent_forward_list &&) = delete;

        /** @brief 移动赋值（禁用，避免线程安全隐患） */
        concurrent_forward_list &operator=(concurrent_forward_list &&) = delete;

        /** @brief #### 获取分配器 */
        allocator_type get_allocator() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return _forward_list.get_allocator();
        }

        /** @brief #### 判断单向链表是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return _forward_list.empty();
        }

        /** @brief #### 获取第一个元素（只读） */
        const_reference front() const
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return _forward_list.front();
        }

        /** @brief ####  在头部插入元素（拷贝） */
        void push_front(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.push_front(value_data);
        }

        /** @brief #### 在头部插入元素（移动） */
        void push_front(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.push_front(std::move(value_data));
        }

        /** @brief #### 移除头部元素 */
        void pop_front()
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (!_forward_list.empty())
            {
                _forward_list.pop_front();
            }
        }

        /** @brief #### 插入元素（拷贝，在迭代器前） */
        iterator insert_after(iterator pos, const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            auto it = _forward_list.insert_after(pos.base(), value_data);
            return iterator(it);
        }

        /** @brief ####  插入元素（移动，在迭代器前） */
        iterator insert_after(iterator pos, value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            auto it = _forward_list.insert_after(pos.base(), std::move(value_data));
            return iterator(it);
        }

        /** @brief #### 范围插入（在迭代器后） */
        template <typename InputIt>
        void insert_after(iterator pos, InputIt first, InputIt last)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.insert_after(pos.base(), first, last);
        }

        /** @brief #### 初始化列表插入（在迭代器后） */
        void insert_after(iterator pos, std::initializer_list<value> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.insert_after(pos.base(), ilist);
        }

        /** @brief #### 就地构造元素并插入（在迭代器后） */
        template <typename... Args>
        iterator emplace_after(iterator pos, Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            auto it = _forward_list.emplace_after(pos.base(), std::forward<Args>(args)...);
            return iterator(it);
        }

        /** @brief #### 在头部就地构造元素 */
        template <typename... Args>
        void emplace_front(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.emplace_front(std::forward<Args>(args)...);
        }

        /** @brief ####  移除迭代器后一个元素 */
        iterator erase_after(iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            auto it = _forward_list.erase_after(pos.base());
            return iterator(it);
        }

        /** @brief #### 移除迭代器后一个范围的元素 */
        iterator erase_after(iterator first, iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            auto it = _forward_list.erase_after(first.base(), last.base());
            return iterator(it);
        }

        /** @brief #### 清空单向链表 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            _forward_list.clear();
        }

        /** @brief #### 交换两个单向链表（线程安全） */
        void swap(concurrent_forward_list &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_mutex);
                std::unique_lock<std::shared_mutex> other_lock(other._mutex);
                _forward_list.swap(other._forward_list);
            }
        }

        /** @brief #### 起始迭代器（只读） */
        iterator begin() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return _forward_list.begin();
        }

        /** @brief #### 结束迭代器（只读） */
        iterator end() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return _forward_list.end();
        }

        /** @brief #### 起始迭代器（只读，C++11 兼容） */
        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        /** @brief #### 结束迭代器（只读，C++11 兼容） */
        const_iterator cend() const noexcept
        {
            return end();
        }
    };
}