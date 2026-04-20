/**
 * @file    Concurrent_set.hpp
 * @brief   线程安全的 std::set 完整封装
 * @author  wang
 * @version 1.0
 * @date    2025-08-15
 *
 * 本文件提供多线程可直接使用的有序集合。
 * 内部使用 std::shared_mutex 读写分离：
 *   - 读：共享锁，多线程并发安全；
 *   - 写：独占锁，保证数据一致性。
 * 对外只暴露只读快照，不提供可变迭代器。
 */

#pragma once
#include <set>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <initializer_list>
namespace multi_concurrent
{
    /**
   * @class concurrent_set
   * @brief  线程安全的有序集合
   *
   * @tparam value              元素类型
   * @tparam comparator         比较器，默认 `std::less`
   * @tparam custom_allocator   分配器，默认 `std::allocator`
   * @note  1. 内部真正容器是 `std::set`；
   * @note  2. 所有写操作加独占锁；
   * @note  3. 所有读操作加共享锁；
   * @note  4. 外部遍历请用 `snapshot()`，安全方便。
   */
    template <typename value,typename comparator = std::less<value>,
                        typename custom_allocator = std::allocator<value>>
    class concurrent_set
    {
        using standard_library_set = std::set<value, comparator, custom_allocator>;

    public:
        using value_type     = value;
        using size_type      = typename standard_library_set::size_type;
        using iterator       = typename standard_library_set::const_iterator;
        using const_iterator = iterator;
        using key_compare    = comparator;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_set _set;                 

    public:
        /** @brief 默认构造一个空集合 */
        concurrent_set() = default;

        /** @brief 指定比较器和分配器构造 */
        explicit concurrent_set(const comparator &comp,const custom_allocator &alloc = custom_allocator())
            : _set(comp, alloc) {}

        /**
     * @brief 范围构造
     * @tparam input_it 输入迭代器类型
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     * @param comp  比较器
     * @param alloc 分配器
     */
        template <typename input_it>
        concurrent_set(input_it first, input_it last,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _set(first, last, comp, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 `{1,2,3}` 的列表
     * @param comp 比较器
     * @param alloc 分配器
     */
        concurrent_set(std::initializer_list<value_type> init,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _set(init, comp, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_set(const concurrent_set &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _set = rhs._set;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_set &operator=(const concurrent_set &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _set = rhs._set;
            }
            return *this;
        }

        /** @brief #### 是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.empty();
        }

        /** @brief #### 元素个数 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.size();
        }

        /** @brief #### 最大可容纳元素数（理论值） */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.max_size();
        }

        /**
     * @brief #### 插入元素（拷贝）
     * @param value_data 待插入值
     * @return `first`  迭代器指向元素
     * @return `second true`= 成功插入，`false`= 已存在
     */
        std::pair<iterator, bool> insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _set.insert(value_data);
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 插入元素（移动）
     * @param value_data 待插入值（右值）
     * @return 同上
     */
        std::pair<iterator, bool> insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _set.insert(std::move(value_data));
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 范围插入
     * @tparam input_it 输入迭代器
     * @param first 起始
     * @param last  终止（不含）
     */
        template <typename input_it>
        void insert(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _set.insert(first, last);
        }

        /**
     * @brief #### 初始化列表插入
     * @param ilist 初始化列表
     */
        void insert(std::initializer_list<value_type> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _set.insert(ilist);
        }

        /**
     * @brief #### 就地构造元素
     * @param args 构造元素所需参数
     * @return 同上
     */
        template <typename... Args>
        std::pair<iterator, bool> emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _set.emplace(std::forward<Args>(args)...);
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 删除指定值
     * @param value_data 待删除的值
     * @return 删除的元素个数（0 或 1）
     */
        size_type erase(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _set.erase(value_data);
        }

        /**
     * @brief #### 按迭代器删除
     * @param pos 待删除位置
     * @return 指向下一个元素的迭代器
     */
        iterator erase(const_iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _set.erase(pos.base());
            return iterator(it);
        }

        /**
     * @brief #### 范围删除
     * @param first 起始
     * @param last  终止（不含）
     * @return 指向 `last` 的迭代器
     */
        iterator erase(const_iterator first, const_iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _set.erase(first.base(), last.base());
            return iterator(it);
        }

        /** @brief #### 清空所有元素 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _set.clear();
        }

        /**
     * @brief #### 与另一线程安全 `set` 交换内容
     * @param other 另一个实例
     * @note  采用双锁避免死锁
     */
        void swap(concurrent_set &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _set.swap(other._set);
        }

        /**
     * @brief #### 查找元素
     * @param value_data 待查找的值
     * @return 指向元素的迭代器；不存在时等于 end()
     */
        iterator find(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_set.find(value_data));
        }

        /** @brief #### 统计元素出现次数（0 或 1） */
        size_type count(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.count(value_data);
        }

        /** @brief #### 判断元素是否存在 */
        bool contains(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.contains(value_data);
        }

        /**
     * @brief #### 首个不小于给定值的迭代器
     * @param value_data 给定值
     */
        iterator lower_bound(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_set.lower_bound(value_data));
        }

        /**
     * @brief #### 首个大于给定值的迭代器
     * @param value_data 给定值
     */
        iterator upper_bound(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_set.upper_bound(value_data));
        }

        /**
     * @brief #### 获取等于给定值的区间 `[lower, upper)`
     * @param value_data 给定值
     * @return pair.first  起始迭代器
     * @return pair.second 结束迭代器
     */
        std::pair<iterator, iterator> equal_range(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _set.equal_range(value_data);
            return {iterator(ret.first), iterator(ret.second)};
        }

        /** @brief #### 返回键比较器副本 */
        key_compare key_comp() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _set.key_comp();
        }

        /**
     * @brief #### 零拷贝合并另一 `set`（左值）
     * @tparam C2  比较器类型
     * @tparam A2  分配器类型
     * @param source 源 `set`（合并后清空）
     */
        template <typename C2, typename A2>
        void merge(concurrent_set<value, C2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _set.merge(source._set);
        }

        /**
     * @brief #### 零拷贝合并另一 `set`（右值）
     * @tparam C2  比较器类型
     * @tparam A2  分配器类型
     * @param source 源 `set`（合并后清空）
     */
        template <typename C2, typename A2>
        void merge(concurrent_set<value, C2, A2> &&source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _set.merge(std::move(source._set));
        }

        /**
     * @brief #### 生成当前集合的只读快照
     * @return `std::vector<value_type>` 升序副本
     * @note  内部加读锁，拷贝完成后释放，外部可任意遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return {_set.begin(), _set.end()};
        }
    };
}
