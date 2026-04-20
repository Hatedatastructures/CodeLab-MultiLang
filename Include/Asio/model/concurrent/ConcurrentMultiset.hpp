/**
 * @file Concurrent_multiset.hpp
 * @brief 线程安全的多重集合容器
 * @author wang
 * @version 1.0
 * @date 2025-08-17
 *
 * 内部使用 std::multiset 作为底层容器，
 * 通过 std::shared_mutex 实现线程安全（读共享、写独占）：
 *   - 支持元素的插入、删除、查找等操作，允许元素重复；
 *   - 读操作（如查找、计数数量）加共享锁，写操作（如插入、删除）加独占锁；
 *   - 支持任意数量的生产者和消费者并发访问。
 */
#pragma once
#include <mutex>
#include <shared_mutex>
#include <initializer_list>
#include <vector>
#include <algorithm>

namespace multi_concurrent
{
   /**
   * @brief 线程安全的 有序多重集合
   * @tparam value_data 值类型
   * @tparam comparator 键比较器类型，默认 `std::less<value_data>`
   * @tparam custom_allocator 分配器类型，默认 `std::allocator<value_data>`
   */
    template <typename value,typename comparator = std::less<value>,typename custom_allocator = std::allocator<value>>
    class concurrent_multiset
    {
        using standard_library_multiset = std::multiset<value, comparator, custom_allocator>;

    public:
        using value_type      = value;
        using size_type       = typename standard_library_multiset::size_type;
        using iterator        = typename standard_library_multiset::const_iterator; // 只读迭代器
        using const_iterator  = iterator;
        using key_compare     = comparator;
        using allocator_type  = custom_allocator;
        using reference       = const value &; // 禁止非const引用修改
        using const_reference = const value &;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_multiset _multiset;

    public:
        /** @brief 默认构造一个空多重集合 */
        concurrent_multiset() = default;

        /** @brief 指定比较器和分配器构造 */
        explicit concurrent_multiset(const key_compare &comp,const allocator_type &alloc = allocator_type())
            : _multiset(comp, alloc) {}

        /**
     * @brief 范围构造
     * @tparam InputIt 输入迭代器类型
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     * @param comp  比较器
     * @param alloc 分配器
     */
        template <typename InputIt>
        concurrent_multiset(InputIt first, InputIt last,const key_compare &comp = key_compare(),
            const allocator_type &alloc = allocator_type())
            : _multiset(first, last, comp, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 `{1,2,2,3}` 的初始化列表
     * @param comp 比较器
     * @param alloc 分配器
     */
        concurrent_multiset(std::initializer_list<value_type> init,const key_compare &comp = key_compare(),
            const allocator_type &alloc = allocator_type())
            : _multiset(init, comp, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_multiset(const concurrent_multiset &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _multiset = rhs._multiset;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_multiset &operator=(const concurrent_multiset &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _multiset = rhs._multiset;
            }
            return *this;
        }

        /** @brief 移动构造（禁用，避免线程安全问题） */
        concurrent_multiset(concurrent_multiset &&) = delete;

        /** @brief 移动赋值（禁用，避免线程安全问题） */
        concurrent_multiset &operator=(concurrent_multiset &&) = delete;

        /** @brief #### 获取分配器 */
        allocator_type get_allocator() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.get_allocator();
        }

        // 迭代器相关
        /** @brief #### 返回起始迭代器（只读） */
        iterator begin() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.begin();
        }

        /** @brief #### 返回结束迭代器（只读） */
        iterator end() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.end();
        }

        /** @brief #### 返回起始迭代器（只读） */
        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        /** @brief #### 返回结束迭代器（只读） */
        const_iterator cend() const noexcept
        {
            return end();
        }

        /** @brief #### 判断容器是否为空 */
        bool empty() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.empty();
        }

        /** @brief #### 返回元素个数 */
        size_type size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.size();
        }

        /** @brief #### 返回最大可容纳元素数（理论值） */
        size_type max_size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.max_size();
        }

        // 修改操作
        /**
     * @brief #### 插入元素（拷贝）
     * @param value_data 待插入值
     * @return 指向插入元素的迭代器
     */
        iterator insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.insert(value_data);
        }

        /**
     * @brief #### 插入元素（移动）
     * @param value_data 待插入值（右值）
     * @return 指向插入元素的迭代器
     */
        iterator insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.insert(std::move(value_data));
        }

        /**
     * @brief #### 范围插入
     * @tparam InputIt 输入迭代器
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     */
        template <typename InputIt>
        void insert(InputIt first, InputIt last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multiset.insert(first, last);
        }

        /**
     * @brief #### 初始化列表插入
     * @param ilist 初始化列表
     */
        void insert(std::initializer_list<value_type> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multiset.insert(ilist);
        }

        /**
     * @brief #### 就地构造元素并插入
     * @param args 构造元素的参数
     * @return 指向新元素的迭代器
     */
        template <typename... Args>
        iterator emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.emplace(std::forward<Args>(args)...);
        }

        /**
     * @brief #### 按迭代器删除单个元素
     * @param pos 指向待删除元素的迭代器
     * @return 下一个元素的迭代器
     */
        iterator erase(iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            // 注意：需要将const_iterator转换为非const迭代器（内部操作安全）
            auto non_const_pos = _multiset.erase(pos.base());
            return iterator(non_const_pos);
        }

        /**
     * @brief #### 删除区间元素
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     * @return 指向last的迭代器
     */
        iterator erase(iterator first, iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto non_const_last = _multiset.erase(first.base(), last.base());
            return iterator(non_const_last);
        }

        /**
     * @brief #### 删除所有等于指定值的元素
     * @param value_data 待删除的值
     * @return 被删除的元素个数
     */
        size_type erase(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.erase(value_data);
        }

        /** @brief #### 清空所有元素 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multiset.clear();
        }

        /**
     * @brief #### 与另一个线程安全多重集合交换内容
     * @param other 另一个concurrent_multiset实例
     * @note 采用双锁避免死锁
     */
        void swap(concurrent_multiset &other)
        {
            if (this == &other)
                return;

            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex, std::defer_lock);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex, std::defer_lock);
            std::lock(lhs_lock, rhs_lock); // 同时加锁避免死锁

            _multiset.swap(other._multiset);
        }

        /**
     * @brief #### 查找第一个等于指定值的元素
     * @param value_data 目标值
     * @return 指向找到元素的迭代器；若未找到则返回end()
     */
        iterator find(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.find(value_data);
        }

        /**
     * @brief #### 计算等于指定值的元素个数
     * @param value_data 目标值
     * @return 匹配元素的数量
     */
        size_type count(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.count(value_data);
        }

        /**
     * @brief #### 返回第一个不小于指定值的元素迭代器
     * @param value_data 目标值
     * @return 满足条件的迭代器
     */
        iterator lower_bound(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.lower_bound(value_data);
        }

        /**
     * @brief #### 返回第一个大于指定值的元素迭代器
     * @param value_data 目标值
     * @return 满足条件的迭代器
     */
        iterator upper_bound(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.upper_bound(value_data);
        }

        /**
     * @brief #### 返回等于指定值的元素范围
     * @param value_data 目标值
     * @return 包含[first, last)的迭代器对
     */
        std::pair<iterator, iterator> equal_range(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            auto range = _multiset.equal_range(value_data);
            return {iterator(range.first), iterator(range.second)};
        }

        /** @brief #### 获取比较器对象 */
        key_compare key_comp() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multiset.key_comp();
        }

        /**
     * @brief #### 获取容器的只读快照
     * @return 包含所有元素的`vector`副本（按内部排序）
     * @note 内部加读锁，拷贝后立即释放，外部可安全遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return std::vector<value_type>(_multiset.begin(), _multiset.end());
        }
    };

}
