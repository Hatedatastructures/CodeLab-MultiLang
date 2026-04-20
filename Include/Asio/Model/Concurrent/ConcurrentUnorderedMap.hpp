/**
 * @file Concurrent_unordered_map.hpp
 * @brief 线程安全的 std::unordered_map 完整封装（仅中文注释）
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 本文件提供多线程可直接使用的哈希表容器，内部采用 std::shared_mutex 读写分离：
 *   - 读：共享锁，多线程并发读取；
 *   - 写：独占锁，保证数据一致性。
 * 所有接口与  std::unordered_map 保持一致，对外不提供可变迭代器，遍历请用 snapshot()。
 */

#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <utility>
#include <initializer_list>
namespace multi_concurrent
{
    /**
   * @class concurrent_unordered_map
   * @brief 线程安全的无序哈希映射（键值对）
   *
   * @tparam key                    键类型，必须支持哈希与相等比较
   * @tparam value                  值类型
   * @tparam hash_function_object   哈希函数对象，默认 `std::hash<key>`
   * @tparam judgment_tool          键相等判断，默认 `std::equal_to<key>`
   * @tparam custom_allocator       分配器，默认 `std::allocator<std::pair<const key, value>>`
   * @note  1. 内部实际容器为 `std::unordered_map`；
   * @note  2. 所有修改操作加独占锁；
   * @note  3. 所有只读操作加共享锁；
   * @note  4. 外部遍历请使用 `snapshot()` 获取快照。
   */
    template <typename key,typename value,typename hash_function_object = std::hash<key>,typename judgment_tool = std::equal_to<key>,
                        typename custom_allocator = std::allocator<std::pair<const key, value>>>
    class concurrent_unordered_map
    {
        using standard_library_map = std::unordered_map<key, value, hash_function_object, judgment_tool, custom_allocator>;

    public:
        using key_type       = key;
        using mapped_type    = value;
        using value_type     = typename standard_library_map::value_type;
        using size_type      = typename standard_library_map::size_type;
        using iterator       = typename standard_library_map::const_iterator;
        using const_iterator = iterator;
        using hasher         = hash_function_object;
        using key_equal      = judgment_tool;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_map _hash_map;      

    public:
        /** @brief 默认构造空哈希表 */
        concurrent_unordered_map() = default;

        /**
     * @brief 指定桶数、哈希函数、键相等函数、分配器构造
     * @param bucket_count 初始桶数
     * @param hash         哈希函数对象
     * @param pred         键相等函数对象
     * @param alloc        分配器对象
     */
        explicit concurrent_unordered_map(size_type bucket_count,const hash_function_object &hash = hash_function_object(),
            const judgment_tool &pred = judgment_tool(),const custom_allocator &alloc = custom_allocator())
            : _hash_map(bucket_count, hash, pred, alloc) {}

        /**
     * @brief 范围构造
     * @tparam input_it 输入迭代器
     * @param first 起始
     * @param last  终止（不含）
     * @param bucket_count 初始桶数
     * @param hash  哈希函数
     * @param pred  键相等函数
     * @param alloc 分配器
     */
        template <typename input_it>
        concurrent_unordered_map(input_it first, input_it last,size_type bucket_count = 0,
            const hash_function_object &hash = hash_function_object(),const judgment_tool &pred = judgment_tool(),
            const custom_allocator &alloc = custom_allocator())
            : _hash_map(first, last, bucket_count, hash, pred, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 { {k1,v1}, {k2,v2} } 的列表
     * @param bucket_count 初始桶数
     * @param hash  哈希函数
     * @param pred  键相等函数
     * @param alloc 分配器
     */
        concurrent_unordered_map(std::initializer_list<value_type> init,size_type bucket_count = 0,
            const hash_function_object &hash = hash_function_object(),const judgment_tool &pred = judgment_tool(),
            const custom_allocator &alloc = custom_allocator())
            : _hash_map(init, bucket_count, hash, pred, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_unordered_map(const concurrent_unordered_map &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _hash_map = rhs._hash_map;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_unordered_map &operator=(const concurrent_unordered_map &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _hash_map = rhs._hash_map;
            }
            return *this;
        }

        /** @brief #### 是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.empty();
        }

        /** @brief #### 元素个数 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.size();
        }

        /** @brief #### 最大元素数（理论值） */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.max_size();
        }

        /** @brief #### 当前桶数 */
        size_type bucket_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.bucket_count();
        }

        /** @brief #### 最大允许桶数 */
        size_type max_bucket_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.max_bucket_count();
        }

        /** @brief #### 指定桶的元素数量 */
        size_type bucket_size(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.bucket_size(n);
        }

        /** @brief #### 指定桶的起始迭代器 */
        iterator begin(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.begin(n));
        }
        /**
     * @brief #### 返回第一个桶的起始迭代器
     */
        iterator begin() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.begin());
        }

        /** @brief ####  指定桶的终止迭代器 */
        iterator end(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.end(n));
        }
        /**
     * @brief #### 返回最后一个桶的终止迭代器
     */
        iterator end() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.end());
        }
        /**
     * @brief #### 带边界检查的访问（可变）
     * @param key 键
     * @return 值的引用
     * @throw `std::out_of_range` 键不存在则抛异常
     */
        mapped_type &at(const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.at(key_data);
        }

        /**
     * @brief #### 带边界检查的访问（只读）
     * @param key 键
     * @return 值的 `const` 引用
     * @throw `std::out_of_range` 键不存在则抛异常
     */
        const mapped_type &at(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.at(key_data);
        }

        /**
     * @brief #### 下标访问（不存在则插入默认构造值）
     * @param key 键
     * @return 值的引用
     */
        mapped_type &operator[](const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map[key_data];
        }

        /** @overload #### 右值键下标访问 */
        mapped_type &operator[](key_type &&key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map[std::move(key_data)];
        }

        /**
     * @brief #### 插入键值对（拷贝）
     * @param value 键值对
     * @return `first`  迭代器
     * @return `second true` = 插入成功，`false =已存在
     */
        std::pair<iterator, bool> insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _hash_map.insert(value_data);
            return {iterator(ret.first), ret.second};
        }

        /** @brief #### 插入键值对（移动） */
        std::pair<iterator, bool> insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _hash_map.insert(std::move(value_data));
            return {iterator(ret.first), ret.second};
        }

        /** @brief #### 范围插入 */
        template <typename input_it>
        void insert(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_map.insert(first, last);
        }

        /** @brief #### 初始化列表插入 */
        void insert(std::initializer_list<value_type> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_map.insert(ilist);
        }

        /** @brief ####  就地构造键值对 */
        template <typename... Args>
        std::pair<iterator, bool> emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _hash_map.emplace(std::forward<Args>(args)...);
            return {iterator(ret.first), ret.second};
        }

        /** @brief ####  删除指定键 */
        size_type erase(const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.erase(key_data);
        }

        /** @brief #### 按迭代器删除单个元素 */
        iterator erase(const_iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _hash_map.erase(pos.base());
            return iterator(it);
        }

        /** @brief ####  范围删除 */
        iterator erase(const_iterator first, const_iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _hash_map.erase(first.base(), last.base());
            return iterator(it);
        }

        /** @brief #### 清空所有元素 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_map.clear();
        }

        /** @brief ####  与另一线程安全 `unordered_map` 交换内容 */
        void swap(concurrent_unordered_map &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _hash_map.swap(other._hash_map);
        }

        /**
     * @brief ####  查找键
     * @param key 待查找键
     * @return 指向元素的迭代器；不存在返回 `end()`
     */
        iterator find(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.find(key_data));
        }

        /** @brief #### 统计键出现次数（0 或 1） */
        size_type count(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.count(key_data);
        }

        /** @brief ####  判断键是否存在 */
        bool contains(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.contains(key_data);
        }

        /** @brief ####  返回哈希函数对象 */
        hasher hash_function() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.hash_function();
        }

        /** @brief ####  返回键相等函数对象 */
        key_equal key_eq() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_map.key_eq();
        } 

        /**
     * @brief ####  零拷贝合并另一无序 `map`（左值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源 `map`（合并后清空）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_map<key_type, mapped_type, H2, P2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _hash_map.merge(source._hash_map);
        }

        /**
     * @brief ####  零拷贝合并另一无序 `map`（右值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源 `map`（合并后清空）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_map<key_type, mapped_type, H2, P2, A2> &&source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _hash_map.merge(std::move(source._hash_map));
        }

        /**
     * @brief #### 生成当前哈希表的只读快照
     * @return `std::vector<value_type>` 无序键值对副本
     * @note  内部加读锁，拷贝后立即释放，外部可安全遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return {_hash_map.begin(), _hash_map.end()};
        }
    };
}
