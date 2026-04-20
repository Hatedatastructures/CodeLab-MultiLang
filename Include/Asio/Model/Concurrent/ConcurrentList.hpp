#pragma once

#include <list>
#include <mutex>
#include <shared_mutex>
#include <iterator>
#include <algorithm>

namespace multi_concurrent
{
    /**
   * @file Concurrent_list.hpp
   * @brief 线程安全的双向链表容器
   * @details 对 std::list 进行线程安全封装，支持多生产者多消费者并发访问。
   *          内部通过 std::shared_mutex 实现同步：读操作（如查找、遍历）加共享锁，
   *          写操作（如插入、删除）加独占锁，保证链表操作的线程安全。
   * @tparam value 元素类型
   * @tparam custom_allocator 分配器类型，默认 `std::allocator<value>`
   */
    template <typename value, typename custom_allocator = std::allocator<value>>
    class concurrent_list
    {
    public:
        using value_type      = value;
        using size_type       = typename std::list<value, custom_allocator>::size_type;
        using difference_type = typename std::list<value, custom_allocator>::difference_type;
        using reference       = const value &; // 只读引用，避免外部修改破坏一致性
        using const_reference = const value &;
        using iterator        = typename std::list<value, custom_allocator>::const_iterator;
        using const_iterator  = iterator;
        using allocator_type  = custom_allocator;

    private:
        mutable std::shared_mutex _access_mutex;
        std::list<value, custom_allocator> _list;

    public:
        /** @brief 默认构造空链表 */
        concurrent_list() = default;

        /** @brief 指定分配器构造 */
        explicit concurrent_list(const custom_allocator &alloc) : _list(alloc) {}

        /** @brief 范围构造 */
        template <typename InputIt>
        concurrent_list(InputIt first, InputIt last, const custom_allocator &alloc = custom_allocator())
            : _list(first, last, alloc) {}

        /** @brief 初始化列表构造 */
        concurrent_list(std::initializer_list<value> init, const custom_allocator &alloc = custom_allocator())
            : _list(init, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_list(const concurrent_list &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._access_mutex);
            _list = other._list;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_list &operator=(const concurrent_list &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
                _list = other._list;
            }
            return *this;
        }

        /** @brief 移动构造（禁用，避免线程安全隐患） */
        concurrent_list(concurrent_list &&) = delete;

        /** @brief 移动赋值（禁用，避免线程安全隐患） */
        concurrent_list &operator=(concurrent_list &&) = delete;

        /** @brief #### 获取分配器 */
        allocator_type get_allocator() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.get_allocator();
        }

        /** @brief #### 判断链表是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.empty();
        }

        /** @brief #### 获取元素个数 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.size();
        }

        /** @brief #### 获取最大元素个数（理论值） */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.max_size();
        }

        /** @brief #### 获取第一个元素（只读） */
        const_reference front() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.front();
        }

        /** @brief #### 获取最后一个元素（只读） */
        const_reference back() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.back();
        }

        /** @brief #### 在头部插入元素（拷贝） */
        void push_front(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.push_front(value_data);
        }

        /** @brief #### 在头部插入元素（移动） */
        void push_front(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.push_front(std::move(value_data));
        }

        /** @brief #### 在尾部插入元素（拷贝） */
        void push_back(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.push_back(value_data);
        }

        /** @brief #### 在尾部插入元素（移动） */
        void push_back(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.push_back(std::move(value_data));
        }

        /** @brief #### 移除头部元素 */
        void pop_front()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (!_list.empty())
            {
                _list.pop_front();
            }
        }

        /** @brief #### 移除尾部元素 */
        void pop_back()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (!_list.empty())
            {
                _list.pop_back();
            }
        }

        /** @brief #### 插入元素（拷贝，在迭代器前） */
        iterator insert(iterator pos, const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _list.insert(pos.base(), value_data);
            return iterator(it);
        }

        /** @brief #### 插入元素（移动，在迭代器前） */
        iterator insert(iterator pos, value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _list.insert(pos.base(), std::move(value_data));
            return iterator(it);
        }

        /** @brief #### 范围插入 */
        template <typename InputIt>
        void insert(iterator pos, InputIt first, InputIt last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.insert(pos.base(), first, last);
        }

        /** @brief #### 初始化列表插入 */
        void insert(iterator pos, std::initializer_list<value> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.insert(pos.base(), ilist);
        }

        /** @brief #### 就地构造元素并插入（在迭代器前） */
        template <typename... Args>
        iterator emplace(iterator pos, Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _list.emplace(pos.base(), std::forward<Args>(args)...);
            return iterator(it);
        }

        /** @brief #### 在头部就地构造元素 */
        template <typename... Args>
        void emplace_front(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.emplace_front(std::forward<Args>(args)...);
        }

        /** @brief #### 在尾部就地构造元素 */
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.emplace_back(std::forward<Args>(args)...);
        }

        /** @brief #### 移除指定迭代器位置的元素 */
        iterator erase(iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _list.erase(pos.base());
            return iterator(it);
        }

        /** @brief #### 移除迭代器范围的元素 */
        iterator erase(iterator first, iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _list.erase(first.base(), last.base());
            return iterator(it);
        }

        /** @brief #### 移除所有等于指定值的元素 */
        size_type erase(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _list.erase(value_data);
        }

        /** @brief #### 清空链表 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _list.clear();
        }

        /** @brief #### 交换两个链表（线程安全） */
        void swap(concurrent_list &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::unique_lock<std::shared_mutex> other_lock(other._access_mutex);
                _list.swap(other._list);
            }
        }

        /** @brief #### 起始迭代器（只读） */
        iterator begin() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.begin();
        }

        /** @brief #### 结束迭代器（只读） */
        iterator end() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _list.end();
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
