#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <vector>
#include <initializer_list>
#include <utility>
#include <iterator>

namespace multi_concurrent
{
    /**
   * @class concurrent_unordered_multimap
   * @brief 线程安全的无序哈希多重映射（键值对）
   *
   * @tparam key                    键类型，必须支持哈希与相等比较
   * @tparam value                  值类型
   * @tparam hash_function_object   哈希函数对象，默认 `std::hash<key>`
   * @tparam judgment_tool          键相等判断，默认 `std::equal_to<key>`
   * @tparam custom_allocator       分配器，默认 `std::allocator<std::pair<const key, value>>`
   * @note  遍历请使用 `snapshot()` 获取快照。
   */
    template <typename key, typename value, typename hash_function_object = std::hash<key>, typename judgment_tool = std::equal_to<key>,
                        typename custom_allocator = std::allocator<std::pair<const key, value>>>
    class concurrent_unordered_multimap
    {
        using standard_library_multimap = std::unordered_multimap<key, value, hash_function_object, judgment_tool, custom_allocator>;

    public:
        using key_type       = key;
        using mapped_type    = value;
        using value_type     = typename standard_library_multimap::value_type;
        using size_type      = typename standard_library_multimap::size_type;
        using iterator       = typename standard_library_multimap::const_iterator;
        using const_iterator = iterator;
        using hasher         = hash_function_object;
        using key_equal      = judgment_tool;

    private:
        mutable std::shared_mutex _access_mutex;
        standard_library_multimap _hash_map;

    public:
        /** @brief 默认构造空哈希表 */
        concurrent_unordered_multimap() = default;

        /**
     * @brief 指定桶数、哈希函数、键相等函数、分配器构造
     * @param bucket_count 初始桶数
     * @param hash         哈希函数对象
     * @param pred         键相等函数对象
     * @param alloc        分配器对象
     */
        explicit concurrent_unordered_multimap(size_type bucket_count, const hash_function_object &hash = hash_function_object(),
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
        concurrent_unordered_multimap(input_it first, input_it last,size_type bucket_count = 0,
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
        concurrent_unordered_multimap(std::initializer_list<value_type> init,size_type bucket_count = 0,
            const hash_function_object &hash = hash_function_object(),const judgment_tool &pred = judgment_tool(),
            const custom_allocator &alloc = custom_allocator())
            : _hash_map(init, bucket_count, hash, pred, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_unordered_multimap(const concurrent_unordered_multimap &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _hash_map = rhs._hash_map;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_unordered_multimap &operator=(const concurrent_unordered_multimap &rhs)
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

        /** @brief #### 返回 返回第一个桶的起始迭代器 */
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

        /** @brief #### 返回最后一个桶的终止迭代器 */
        iterator end() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.end());
        }

        /**
     * @brief #### 插入键值对（拷贝）
     * @param value 键值对
     * @return 指向插入元素的迭代器
     */
        iterator insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.insert(value_data));
        }

        /** @brief #### 插入键值对（移动） */
        iterator insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.insert(std::move(value_data)));
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
        iterator emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.emplace(std::forward<Args>(args)...));
        }

        /** @brief ####  删除指定键的所有元素 */
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

        /** @brief ####  与另一线程安全 `unordered_multimap` 交换内容 */
        void swap(concurrent_unordered_multimap &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _hash_map.swap(other._hash_map);
        }

        /**
     * @brief ####  查找键（返回     * @param key 待查找键
     * @return 指向第一个匹配元素的迭代器；不存在返回 `end()`
     */
        iterator find(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_map.find(key_data));
        }

        /** @brief #### 统计键出现次数 */
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
     * @brief ####  零拷贝合并另一无序 `multimap`（左值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源 `multimap`（合并后保留不重复元素）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_multimap<key_type, mapped_type, H2, P2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _hash_map.merge(source._hash_map);
        }

        /**
     * @brief ####  零拷贝合并另一无序 `multimap`（右值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源 `multimap`（合并后保留不重复元素）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_multimap<key_type, mapped_type, H2, P2, A2> &&source)
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
