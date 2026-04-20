
#pragma once
#include <unordered_set>
#include <shared_mutex>
#include <vector>
#include <initializer_list>
#include <utility>
#include <iterator>

namespace multi_concurrent
{
    /**
   * @class concurrent_unordered_multiset
   * @brief 线程安全的无序哈希集合
   *
   * @tparam value                 元素类型，必须支持哈希与相等比较
   * @tparam hash_function_object       哈希函数对象，默认` std::hash<value>`
   * @tparam judgment_tool              相等判断函数对象，默认 `std::equal_to<value>`
   * @tparam custom_allocator           分配器，默认 `std::allocator<value>`
   *
   * @note 1. 内部真正容器是 `std::unordered_multiset`；
   * @note 2. 所有可能修改内部哈希表的操作都加独占锁；
   * @note 3. 所有只读操作加共享锁；
   * @note 4. 外部遍历请使用 `snapshot()` 获取只读快照。
   */
    template <typename value,typename hash_function_object = std::hash<value>,typename judgment_tool = std::equal_to<value>,
                        typename custom_allocator = std::allocator<value>>
    class concurrent_unordered_multiset
    {
        using standard_library_set = std::unordered_multiset<value, hash_function_object, judgment_tool, custom_allocator>;

    public:
        using value_type     = value;
        using size_type      = typename standard_library_set::size_type;
        using iterator       = typename standard_library_set::const_iterator;
        using const_iterator = iterator;
        using hasher         = hash_function_object;
        using key_equal      = judgment_tool;
        using allocator_type = custom_allocator;

    private:
        mutable std::shared_mutex _access_mutex;
        standard_library_set _hash_set;

    public:
        /** @brief 默认构造空集合 */
        concurrent_unordered_multiset() = default;

        /**
     * @brief 指定桶数、哈希函数、相等函数、分配器构造
     * @param bucket_count 初始桶数
     * @param hash         哈希函数对象
     * @param pred         相等函数对象
     * @param alloc        分配器对象
     */
        explicit concurrent_unordered_multiset(size_type bucket_count,const hash_function_object &hash = hash_function_object(),
            const judgment_tool &pred = judgment_tool(),const custom_allocator &alloc = custom_allocator())
                : _hash_set(bucket_count, hash, pred, alloc) {}

        /**
     * @brief #### 范围构造
     * @tparam input_it 任意输入迭代器
     * @param first 起始迭代器
     * @param last  终止迭代器（不含）
     * @param bucket_count 初始桶数
     * @param hash  哈希函数
     * @param pred  相等函数
     * @param alloc 分配器
     */
        template <typename input_it>
        concurrent_unordered_multiset(input_it first, input_it last,size_type bucket_count = 0,
            const hash_function_object &hash = hash_function_object(),const judgment_tool &pred = judgment_tool(),
            const custom_allocator &alloc = custom_allocator())
            : _hash_set(first, last, bucket_count, hash, pred, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 {1,2,3,2} 的初始化列表
     * @param bucket_count 初始桶数
     * @param hash  哈希函数
     * @param pred  相等函数
     * @param alloc 分配器
     */
        concurrent_unordered_multiset(std::initializer_list<value_type> init,size_type bucket_count = 0,
            const hash_function_object &hash = hash_function_object(),const judgment_tool &pred = judgment_tool(),
            const custom_allocator &alloc = custom_allocator())
            : _hash_set(init, bucket_count, hash, pred, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_unordered_multiset(const concurrent_unordered_multiset &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _hash_set = rhs._hash_set;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_unordered_multiset &operator=(const concurrent_unordered_multiset &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _hash_set = rhs._hash_set;
            }
            return *this;
        }

        /** @brief 移动构造 */
        concurrent_unordered_multiset(concurrent_unordered_multiset &&rhs) noexcept
                : _hash_set(std::move(rhs._hash_set)) {}

        /** @brief 移动赋值 */
        concurrent_unordered_multiset &operator=(concurrent_unordered_multiset &&rhs) noexcept
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lock(_access_mutex);
                _hash_set = std::move(rhs._hash_set);
            }
            return *this;
        }

        /** @brief #### 获取分配器 */
        allocator_type get_allocator() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.get_allocator();
        }

        /** @brief #### 是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.empty();
        }

        /** @brief #### 元素个数 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.size();
        }

        /** @brief #### 最大元素数（理论值） */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.max_size();
        }

        /** @brief #### 当前桶数 */
        size_type bucket_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.bucket_count();
        }

        /** @brief #### 最大允许桶数 */
        size_type max_bucket_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.max_bucket_count();
        }

        /** @brief #### 指定桶中的元素个数 */
        size_type bucket_size(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.bucket_size(n);
        }

        /**
     * @brief #### 获取元素所在的桶索引
     * @param value_data 目标元素
     * @return 桶索引值
     */
        size_type bucket(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.bucket(value_data);
        }

        /** @brief #### 指定桶的起始迭代器 */
        iterator begin(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_set.begin(n));
        }

        /** @brief #### 指定桶的终止迭代器 */
        iterator end(size_type n) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_set.end(n));
        }

        /** @brief #### 返回第一个元素的起始迭代器 */
        iterator begin() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_set.begin());
        }

        /** @brief #### 返回最后一个元素的终止迭代器 */
        iterator end() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_set.end());
        }

        /** @brief #### 插入元素（拷贝） */
        iterator insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.insert(value_data);
        }

        /** @brief #### 插入元素（移动） */
        iterator insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.insert(std::move(value_data));
        }

        /** @brief #### 范围插入 */
        template <typename input_it>
        void insert(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.insert(first, last);
        }

        /** @brief #### 初始化列表插入 */
        void insert(std::initializer_list<value_type> ilist)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.insert(ilist);
        }

        /** @brief #### 就地构造元素 */
        template <typename... Args>
        iterator emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.emplace(std::forward<Args>(args)...);
        }

        /** @brief #### 删除指定值的所有元素 */
        size_type erase(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.erase(value_data);
        }

        /** @brief #### 按迭代器删除单个元素 */
        iterator erase(const_iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _hash_set.erase(pos.base());
            return iterator(it);
        }

        /** @brief #### 范围删除 */
        iterator erase(const_iterator first, const_iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            auto it = _hash_set.erase(first.base(), last.base());
            return iterator(it);
        }

        /** @brief #### 清空所有元素 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.clear();
        }

        /** @brief #### 与另一线程安全 `unordered_multiset` 交换内容 */
        void swap(concurrent_unordered_multiset &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _hash_set.swap(other._hash_set);
        }

        /**
     * @brief #### 查找元素
     * @param value_data 待查找值
     * @return 指向第一个匹配元素的迭代器；不存在返回 `end()`
     */
        iterator find(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return iterator(_hash_set.find(value_data));
        }

        /** @brief #### 统计元素出现次数 */
        size_type count(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.count(value_data);
        }

        /** @brief #### 判断元素是否存在 */
        bool contains(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.contains(value_data);
        }

        /**
     * @brief #### 获取等于给定值的区间 `[first, last)`
     * @param value_data 给定值
     * @return pair.first  起始迭代器
     * @return pair.second 结束迭代器
     */
        std::pair<iterator, iterator> equal_range(const value_type &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            auto ret = _hash_set.equal_range(value_data);
            return {iterator(ret.first), iterator(ret.second)};
        }

        /** @brief #### 返回哈希函数对象 */
        hasher hash_function() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.hash_function();
        }

        /** @brief #### 返回键相等函数对象 */
        key_equal key_eq() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.key_eq();
        }

        /** @brief #### 返回当前负载因子 */
        float load_factor() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.load_factor();
        }

        /** @brief #### 返回最大允许负载因子 */
        float max_load_factor() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _hash_set.max_load_factor();
        }

        /**
     * @brief #### 设置最大负载因子
     * @param ml 新的最大负载因子
     */
        void max_load_factor(float ml)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.max_load_factor(ml);
        }

        /**
     * @brief #### 重新哈希，使桶数不少于指定值
     * @param count 最小桶数
     */
        void rehash(size_type count)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.rehash(count);
        }

        /**
     * @brief #### 预留空间，避免多次重哈希
     * @param count 期望元素个数
     */
        void reserve(size_type count)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _hash_set.reserve(count);
        }

        /**
     * @brief #### 零拷贝合并另一无序多集合（左值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源集合（合并后保留重复元素）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_multiset<value, H2, P2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _hash_set.merge(source._hash_set);
        }

        /**
     * @brief #### 零拷贝合并另一无序多集合（右值）
     * @tparam H2   源哈希器类型
     * @tparam P2   源键相等器类型
     * @tparam A2   源分配器类型
     * @param source 源集合（合并后保留重复元素）
     */
        template <typename H2, typename P2, typename A2>
        void merge(concurrent_unordered_multiset<value, H2, P2, A2> &&source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _hash_set.merge(std::move(source._hash_set));
        }

        /**
     * @brief #### 生成当前集合的只读快照
     * @return `std::vector<value_type>` 无序元素副本（包含所有重复元素）
     * @note  内部加读锁，拷贝后立即释放，外部可安全遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return {_hash_set.begin(), _hash_set.end()};
        }
    };

}
