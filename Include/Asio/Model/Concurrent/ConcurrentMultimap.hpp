/**
 * @file Concurrent_multimap.hpp
 * @brief 线程安全的多重映射容器
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 内部使用 std::multimap 作为底层容器，
 * 通过 std::shared_mutex 实现线程安全（读共享、写独占）：
 *   - 支持键值对的插入、删除、查找等操作，允许键重复；
 *   - 读操作（如查找、计数数量）加共享锁，写操作（如插入、删除）加独占锁；
 *   - 支持任意数量的生产者和消费者并发访问。
 */
#pragma once
#include <shared_mutex>
#include <map>
#include <mutex>
#include <iterator>
namespace multi_concurrent
{
    /**
   * @brief 线程安全的 有序多重集合
   * @tparam key 键类型
   * @tparam value 映射值类型
   * @tparam comparator 键比较器类型，默认 `std::less<key>`
   * @tparam custom_allocator 分配器类型，默认 `std::allocator<std::pair<const key, value>>`
   */
    template <typename key, typename value,typename comparator = std::less<key>,
                        typename custom_allocator = std::allocator<std::pair<const key, value>>>
    class concurrent_multimap
    {
        using standard_library_multimap = std::multimap<key, value, comparator, custom_allocator>;

    public:
        using key_type               = key;
        using mapped_type            = value;
        using value_type             = typename standard_library_multimap::value_type;
        using size_type              = typename standard_library_multimap::size_type;
        using difference_type        = typename standard_library_multimap::difference_type;
        using key_compare            = comparator;
        using allocator_type         = custom_allocator;
        using reference              = typename standard_library_multimap::reference;
        using const_reference        = typename standard_library_multimap::const_reference;
        using pointer                = typename standard_library_multimap::pointer;
        using const_pointer          = typename standard_library_multimap::const_pointer;
        // 并发容器仅提供 const 迭代器（避免外部修改内部状态）
        using iterator               = typename standard_library_multimap::const_iterator;
        using const_iterator         = iterator;
        using reverse_iterator       = typename standard_library_multimap::const_reverse_iterator;
        using const_reverse_iterator = reverse_iterator;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_multimap _multimap;              

    public:
        /** @brief 默认构造空multimap */
        concurrent_multimap() = default;

        /**
     * @brief 指定比较器和分配器构造
     * @param comp 比较器对象
     * @param alloc 分配器对象
     */
        explicit concurrent_multimap(const comparator &comp,const custom_allocator &alloc = custom_allocator())
            : _multimap(comp, alloc) {}

        /**
     * @brief 范围构造
     * @tparam input_it 输入迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器（不含）
     * @param comp 比较器对象
     * @param alloc 分配器对象
     */
        template <typename input_it>
        concurrent_multimap(input_it first, input_it last,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _multimap(first, last, comp, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 {{k1,v1}, {k2,v2}, ...} 的初始化列表
     * @param comp 比较器对象
     * @param alloc 分配器对象
     */
        concurrent_multimap(std::initializer_list<value_type> init,const comparator &comp = comparator(),
            const custom_allocator &alloc = custom_allocator())
            : _multimap(init, comp, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_multimap(const concurrent_multimap &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._access_mutex);
            _multimap = other._multimap;
        }

        /** @brief 移动构造（线程安全） */
        concurrent_multimap(concurrent_multimap &&other) noexcept
        {
            std::unique_lock<std::shared_mutex> lock(other._access_mutex);
            _multimap = std::move(other._multimap);
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_multimap &operator=(const concurrent_multimap &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
                _multimap = other._multimap;
            }
            return *this;
        }

        /** @brief 移动赋值（线程安全） */
        concurrent_multimap &operator=(concurrent_multimap &&other) noexcept
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::unique_lock<std::shared_mutex> other_lock(other._access_mutex);
                _multimap = std::move(other._multimap);
            }
            return *this;
        }

        /** @brief 初始化列表赋值 */
        concurrent_multimap &operator=(std::initializer_list<value_type> init)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multimap = init;
            return *this;
        }

        /** @brief #### 获取分配器 */
        allocator_type get_allocator() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.get_allocator();
        }

        // 迭代器相关接口

        /** @brief #### 返回结束迭代器`（const）`*/
        iterator end() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.end();
        }


        /** @brief #### 判断容器是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.empty();
        }

        /** @brief #### 返回当前元素数量 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.size();
        }

        /** @brief #### 返回最大可容纳元素数量 */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.max_size();
        }

        /** @brief #### 清空所有元素 */
        void clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multimap.clear();
        }

        /**
     * @brief #### 插入键值对（拷贝）
     * @param value 待插入的键值对
     * @return 指向插入元素的迭代器
     */
        iterator insert(const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.insert(value_data);
        }

        /**
     * @brief #### 插入键值对（移动）
     * @param value 待插入的键值对（右值）
     * @return 指向插入元素的迭代器
     */
        iterator insert(value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.insert(std::move(value_data));
        }

        /**
     * @brief #### 在指定位置插入元素（提示位置）
     * @param pos 提示插入位置的迭代器
     * @param value 待插入的键值对
     * @return 指向插入元素的迭代器
     */
        iterator insert(const_iterator pos, const value_type &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.insert(pos, value_data);
        }

        /**
     * @brief #### 在指定位置插入右值元素（提示位置）
     * @param pos 提示插入位置的迭代器
     * @param value 待插入的键值对（右值）
     * @return 指向插入元素的迭代器
     */
        iterator insert(const_iterator pos, value_type &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.insert(pos, std::move(value_data));
        }

        /**
     * @brief #### 范围插入
     * @tparam input_it 输入迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器（不含）
     */
        template <typename input_it>
        void insert(input_it first, input_it last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multimap.insert(first, last);
        }

        /**
     * @brief #### 初始化列表插入
     * @param init 初始化列表
     */
        void insert(std::initializer_list<value_type> init)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _multimap.insert(init);
        }

        /**
     * @brief #### 就地构造元素
     * @tparam Args 构造参数类型
     * @param args 构造元素的参数
     * @return 指向新构造元素的迭代器
     */
        template <typename... Args>
        iterator emplace(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.emplace(std::forward<Args>(args)...);
        }

        /**
     * @brief #### 在指定位置就地构造元素
     * @tparam Args 构造参数类型
     * @param pos 提示插入位置的迭代器
     * @param args 构造元素的参数
     * @return 指向新构造元素的迭代器
     */
        template <typename... Args>
        iterator emplace_hint(const_iterator pos, Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.emplace_hint(pos, std::forward<Args>(args)...);
        }

        /**
     * @brief #### 删除指定位置的元素
     * @param pos 待删除元素的迭代器
     * @return 指向下一个元素的迭代器
     */
        iterator erase(const_iterator pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.erase(pos);
        }

        /**
     * @brief #### 删除指定范围的元素
     * @param first 起始迭代器
     * @param last 终止迭代器（不含）
     * @return 指向last的迭代器
     */
        iterator erase(const_iterator first, const_iterator last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.erase(first, last);
        }

        /**
     * @brief #### 删除指定键的所有元素
     * @param key_data 待删除的键
     * @return 被删除的元素数量
     */
        size_type erase(const key_type &key_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.erase(key_data);
        }

        /**
     * @brief #### 与另一个`concurrent_multimap`交换内容
     * @param other 另一个`concurrent_multimap`实例
     * @note 采用双锁避免死锁（按地址顺序加锁）
     */
        void swap(concurrent_multimap &other)
        {
            if (this == &other)
                return;
            // 按地址顺序加锁，避免死锁
            if (std::less<concurrent_multimap *>()(this, &other))
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::unique_lock<std::shared_mutex> other_lock(other._access_mutex);
                _multimap.swap(other._multimap);
            }
            else
            {
                std::unique_lock<std::shared_mutex> other_lock(other._access_mutex);
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                _multimap.swap(other._multimap);
            }
        }

        /**
     * @brief #### 查找指定键的第一个元素
     * @param key_data 目标键
     * @return 指向找到元素的迭代器；若不存在则返回`end()`
     */
        iterator find(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.find(key_data);
        }

        /**
     * @brief #### 统计指定键的元素数量
     * @param key_data 目标键
     * @return 键对应的元素数量
     */
        size_type count(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.count(key_data);
        }

        /**
     * @brief #### 判断指定键是否存在
     * @param key_data 目标键
     * @return 存在则返回`true`，否则返回`false`
     */
        bool contains(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.contains(key_data);
        }

        /**
     * @brief #### 获取指定键的元素范围 
     * @param key_data 目标键
     * @return 包含范围的迭代器对 `[first, last)`
     */
        std::pair<iterator, iterator> equal_range(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.equal_range(key_data);
        }

        /**
     * @brief #### 获取首个不小于指定键的元素
     * @param key_data 目标键
     * @return 指向找到元素的迭代器
     */
        iterator lower_bound(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.lower_bound(key_data);
        }

        /**
     * @brief #### 获取首个大于指定键的元素
     * @param key_data 目标键
     * @return 指向找到元素的迭代器
     */
        iterator upper_bound(const key_type &key_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.upper_bound(key_data);
        }
        /** @brief #### 返回键比较器 */
        key_compare key_comp() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.key_comp();
        }

        /** @brief #### 返回值比较器（比较键值对的键） */
        typename standard_library_multimap::value_compare value_comp() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _multimap.value_comp();
        }

        /**
     * @brief #### 合并另一个multimap（左值）
     * @tparam C2 其他`multimap`的比较器类型
     * @tparam A2 其他`multima`p的分配器类型
     * @param source 源`multimap`（元素会被转移）
     */
        template <typename C2, typename A2>
        void merge(concurrent_multimap<key, value, C2, A2> &source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _multimap.merge(source._multimap);
        }

        /**
     * @brief #### 合并另一个`multimap`（右值）
     * @tparam C2 其他`multimap`的比较器类型
     * @tparam A2 其他`multimap`的分配器类型
     * @param source 源`multimap`（元素会被转移）
     */
        template <typename C2, typename A2>
        void merge(concurrent_multimap<key, value, C2, A2> &&source)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> src_lock(source._access_mutex);
            _multimap.merge(std::move(source._multimap));
        }

        /**
     * @brief #### 生成当前`multimap`的只读快照
     * @return 包含所有元素的有序vector副本
     * @note 内部加读锁，拷贝完成后释放，外部可安全遍历
     */
        std::vector<value_type> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return std::vector<value_type>(_multimap.begin(), _multimap.end());
        }
    };
}
