/**
 * @file    Concurrent_map.hpp
 * @brief   线程安全的 std::map 完整封装
 * @author  wang
 * @version 1.0
 * @date    2025-08-15
 *
 * 本文件提供了一个多线程环境下可直接使用的线程安全 map。
 * 内部采用 std::shared_mutex 实现读写分离：
 *   - 读操作（查找、遍历）允许多线程并发；
 *   - 写操作（插入、删除、修改）独占锁，保证数据一致性。
 * 所有接口与 C++17 标准 std::map 保持一致，并补充了线程安全快照遍历。
 */

#pragma once
#include <map>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <utility>
namespace multi_concurrent
{
    /**
   * @class concurrent_map
   * @brief  线程安全的有序映射容器（键值对）
   *
   * @tparam key               键类型，必须支持比较操作
   * @tparam value             值类型，可以是任意可复制或可移动类型
   * @tparam comparator        键比较器，默认为` std::less<key>`
   * @tparam custom_allocator  分配器，默认为 `std::allocator<std::pair<const key, value>>`
   * @note  1. 内部真正存储数据的是 `std::map`；
   * @note  2. 所有可能修改内部树结构的操作都使用`unique_lock`（写锁）；
   * @note  3. 只读操作使用 `shared_lock`（读锁），允许多线程并发读取；
   * @note  4. 外部无法获得可变迭代器，遍历请使用 `snapshot()`。
   */
    template <typename key,typename value,typename comparator = std::less<key>,
                        typename custom_allocator = std::allocator<std::pair<const key, value>>>
    class concurrent_map
    {
        using standard_library_map = std::map<key, value, comparator, custom_allocator>;
    public:
        using key_type       = key;                             
        using mapped_type    = value;                             
        using value_type     = typename standard_library_map::value_type; 
        using size_type      = typename standard_library_map::size_type;     
        using iterator       = typename standard_library_map::const_iterator; 
        using const_iterator = iterator;                    
        using key_compare    = comparator;                     

    private:
        mutable std::shared_mutex _access_mutex;  
        standard_library_map _map;                          

    public:
        concurrent_map() = default;

        /**
     * @brief 指定比较器和分配器的构造函数
     * @param comp  键比较器实例
     * @param alloc 分配器实例
     */
        explicit concurrent_map(const comparator &comp,const custom_allocator &alloc = custom_allocator())
                : _map(comp, alloc) {}

        /**
     * @brief 范围构造：用 `[first, last)` 区间内的键值对初始化` map`
     * @tparam `input_it` 任意可解引用为 value_type 的输入迭代器
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     * @param comp  比较器实例
     * @param alloc 分配器实例
     */
        template <typename input_it>
        concurrent_map(input_it first, input_it last,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _map(first, last, comp, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 `{ {key1, val1}, {key2, val2} }` 的初始化列表
     * @param comp 比较器实例
     * @param alloc 分配器实例
     */
        concurrent_map(std::initializer_list<value_type> init,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _map(init, comp, alloc) {}

        /**
     * @brief 拷贝构造函数
     * @param rhs 待拷贝的线程安全` map`
     * @note  拷贝时加读锁，线程安全
     */
        concurrent_map(const concurrent_map &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _map = rhs._map;
        }

        /**
     * @brief 拷贝赋值运算符
     * @param rhs 待拷贝的线程安全` map`
     * @return 本对象引用
     * @note  采用双锁避免自赋值死锁
     */
        concurrent_map &operator=(const concurrent_map &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _map = rhs._map;
            }
            return *this;
        }

        /**
     * @brief #### 判断容器是否为空
     * @return `true ` 容器内无元素
     * @return `false `容器内至少有一个元素
     */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.empty();
        }

        /**
     * @brief #### 获取当前元素数量
     * @return 容器中键值对的个数
     */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.size();
        }

        /**
     * @brief #### 获取容器理论上能容纳的最大元素数
     * @return 最大元素数（由 `allocator` 和系统资源决定）
     */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.max_size();
        }

        /*-------------- 元素访问 --------------*/
        /**
     * @brief #### 带边界检查的元素访问（可变）
     * @param key 待查询的键
     * @return 对应值的引用
     * @throw `std::out_of_range` 若键不存在则抛出异常
     */
        mapped_type &at(const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _map.at(key_data);
        }

        /**
     * @brief #### 带边界检查的元素访问（只读）
     * @param key 待查询的键
     * @return 对应值的 `const` 引用
     * @throw `std::out_of_range` 若键不存在则抛出异常
     */
        const mapped_type &at(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.at(key_data);
        }

        /**
     * @brief 下标访问运算符（可变）
     * @param key 键
     * @return 对应值的引用；若键不存在则先插入默认值再返回引用
     * @note  由于可能修改容器，内部加写锁
     */
        mapped_type &operator[](const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _map[key_data];
        }

        /**
     * @brief 下标访问运算符（右值键）
     * @param key 键（右值）
     * @return 对应值的引用；若键不存在则先插入默认值再返回引用
     */
        mapped_type &operator[](key_type &&key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _map[std::move(key_data)];
        }

        /**
     * @brief #### 插入键值对（拷贝）
     * @param value 键值对实例
     * @return `first` 迭代器指向已存在或新插入的元素
     * @return `second true` 表示成功插入；`false` 表示键已存在
     */
        std::pair<iterator, bool> insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.insert(value_data);
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 插入键值对（移动）
     * @param value 键值对实例（右值）
     * @return 同上
     */
        std::pair<iterator, bool> insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.insert(std::move(value_data));
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 范围插入
     * @tparam `input_it` 输入迭代器
     * @param first 起始迭代器
     * @param last  终止迭代器
     */
        template <typename input_it>
        void insert(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _map.insert(first, last);
        }

        /**
     * @brief #### 初始化列表插入
     * @param ilist 形如 `{ {k1, v1}, {k2, v2} }` 的列表
     */
        void insert(std::initializer_list<value_type> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _map.insert(ilist);
        }

        /**
     * @brief #### 就地构造键值对
     * @param args 构造参数（需能组成 `std::pair`）
     * @return 同 `insert`
     */
        template <typename... Args>
        std::pair<iterator, bool> emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.emplace(std::forward<Args>(args)...);
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 若键不存在则就地构造值
     * @param k    键（左值）
     * @param args 值构造实参
     * @return 同 `insert`
     */
        template <typename... Args>
        std::pair<iterator, bool> try_emplace(const key_type &k, Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.try_emplace(k, std::forward<Args>(args)...);
            return {iterator(ret.first), ret.second};
        }

        /** @overload try_emplace(key_type&&, Args&&...) */
        template <typename... Args>
        std::pair<iterator, bool> try_emplace(key_type &&k, Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.try_emplace(std::move(k), std::forward<Args>(args)...);
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 键存在则赋值，不存在则插入
     * @param k   键（左值）
     * @param obj 值
     * @return 同 `insert`
     */
        template <typename M>
        std::pair<iterator, bool> insert_or_assign(const key_type &k, M &&obj)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.insert_or_assign(k, std::forward<M>(obj));
            return {iterator(ret.first), ret.second};
        }

        /** @overload insert_or_assign(key_type&&, M&&) */
        template <typename M>
        std::pair<iterator, bool> insert_or_assign(key_type &&k, M &&obj)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.insert_or_assign(std::move(k), std::forward<M>(obj));
            return {iterator(ret.first), ret.second};
        }

        /**
     * @brief #### 删除指定键
     * @param key 待删除的键
     * @return 删除的元素个数`（0 或 1）`
     */
        size_type erase(const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _map.erase(key_data);
        }

        /**
     * @brief #### 按迭代器删除单个元素
     * @param pos 待删除的位置
     * @return 指向被删除元素下一位置的迭代器
     */
        iterator erase(const_iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _map.erase(pos.base());
            return iterator(it);
        }

        /**
     * @brief #### 范围删除
     * @param first 起始位置
     * @param last  终止位置（不含）
     * @return 指向被删除段后一位置的迭代器
     */
        iterator erase(const_iterator first, const_iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _map.erase(first.base(), last.base());
            return iterator(it);
        }

        /**
     * @brief 清空所有元素
     */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _map.clear();
        }

        /**
     * @brief #### 与另一线程安全 `map` 交换内容
     * @param other 另一个 `concurrent_map` 实例
     * @note  采用双锁避免死锁
     */
        void swap(concurrent_map &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _map.swap(other._map);
        }

        /**
     * @brief #### 根据键查找元素
     * @param key 要查找的键
     * @return 指向元素的只读迭代器；若不存在则等于` end()`
     */
        iterator find(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_map.find(key_data));
        }
        /**
     * @brief #### 返回最后一个元素的迭代器
     */
        iterator end()const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_map.end());
        }
        /**
     * @brief #### 统计键出现次数
     * @param key 待统计的键
     * @return 出现次数`（0 或 1）`
     */
        size_type count(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.count(key_data);
        }

        /**
     * @brief #### 判断键是否存在
     * @param key 待判断的键
     * @return true 存在；`false` 不存在
     */
        bool contains(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.contains(key_data);
        }

        /**
     * @brief 返回首个不小于 `key` 的元素的迭代器
     * @param key 键
     * @return 指向元素的只读迭代器
     */
        iterator lower_bound(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_map.lower_bound(key_data));
        }

        /**
     * @brief 返回首个大于 `key` 的元素的迭代器
     * @param key 键
     * @return 指向元素的只读迭代器
     */
        iterator upper_bound(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_map.upper_bound(key_data));
        }

        /**
     * @brief 获取键的区间` [lower, upper)`
     * @param key 键
     * @return `pair.first`  起始迭代器
     * @return `pair.second` 结束迭代器（不含）
     */
        std::pair<iterator, iterator> equal_range(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _map.equal_range(key_data);
            return {iterator(ret.first), iterator(ret.second)};
        }

        /**
     * @brief 获取键比较器副本
     * @return 比较器实例
     */
        key_compare key_comp() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _map.key_comp();
        }

        /**
     * @brief 零拷贝合并另一 `map`（左值）
     * @tparam C2  另一个比较器类型
     * @tparam A2  另一个分配器类型
     * @param source 源 `map`（合并后被清空）
     */
        template <typename C2, typename A2>
        void merge(concurrent_map<key_type, mapped_type, C2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _map.merge(source._map);
        }

        /**
     * @brief 零拷贝合并另一 `map`（右值）
     * @tparam C2  另一个比较器类型
     * @tparam A2  另一个分配器类型
     * @param source 源 `map`（合并后被清空）
     */
        template <typename C2, typename A2>
        void merge(concurrent_map<key_type, mapped_type, C2, A2> &&source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _map.merge(std::move(source._map));
        }

        /**
     * @brief 生成容器当前状态的只读快照
     * @return `std::vector<value_type>` 按 `key` 升序排列的键值对副本
     * @note  内部加读锁，拷贝完成后立即释放，外部可长时间安全遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return {_map.begin(), _map.end()};
        }
    };
}