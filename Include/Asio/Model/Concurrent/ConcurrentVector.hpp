/**
 * @file Concurrent_vector.hpp
 * @brief 线程安全的动态数组（vector）封装
 * @author wang
 * @version 1.0
 * @date 2025-08-15
 *
 * 本文件提供可在多线程环境直接使用的动态数组容器：
 *   1. 内部真正容器是 std::vector；
 *   2. 所有可能修改内部数据的操作都加独占锁；
 *   3. 所有只读操作加共享锁；
 *   4. 对外不暴露可变迭代器，遍历请使用 snapshot()；
 *   5. 支持 push_back、pop_back、随机读写、区间插入、快照等功能。
 */

#pragma once
#include <vector>
#include <shared_mutex>
#include <initializer_list>

namespace multi_concurrent 
{
    /**
   * @class concurrent_vector
   * @brief 线程安全的动态数组
   * @tparam value         元素类型
   * @tparam custom_allocator 分配器，默认 `std::allocator<value>`
   */
    template <typename value, typename custom_allocator = std::allocator<value>>
    class concurrent_vector
    {
        using standard_library_vector = std::vector<value, custom_allocator>;

    public:
        using value_type = value;
        using size_type = typename standard_library_vector::size_type;
        using iterator = typename standard_library_vector::const_iterator;
        using const_iterator = iterator;

    private:
        mutable std::shared_mutex _access_mutex; 
        standard_library_vector _vector;                  

    public:
        /** @brief 默认构造空 vector */
        concurrent_vector() = default;

        /**
     * @brief 指定初始大小与值构造
     * @param count 初始元素个数
     * @param value_data 初始值（默认构造）
     * @param alloc 分配器
     */
        explicit concurrent_vector(size_type count,const value &value_data = value(),
            const custom_allocator &alloc = custom_allocator())
            : _vector(count, value_data, alloc) {}

        /**
     * @brief 范围构造
     * @tparam input_it 输入迭代器
     * @param first 起始
     * @param last  终止（不含）
     * @param alloc 分配器
     */
        template <typename input_it>
        concurrent_vector(input_it first, input_it last,
            const custom_allocator &alloc = custom_allocator())
            : _vector(first, last, alloc) {}

        /**
     * @brief 初始化列表构造
     * @param init 形如 {1, 2, 3} 的列表
     * @param alloc 分配器
     */
        concurrent_vector(std::initializer_list<value_type> init,
            const custom_allocator &alloc = custom_allocator())
            : _vector(init, alloc) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_vector(const concurrent_vector &rhs)
        {
            std::shared_lock<std::shared_mutex> lock(rhs._access_mutex);
            _vector = rhs._vector;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_vector &operator=(const concurrent_vector &rhs)
        {
            if (this != &rhs)
            {
                std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
                _vector = rhs._vector;
            }
            return *this;
        }
        /** @brief #### 当前元素数量 */
        size_type size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.size();
        }

        /** @brief #### 是否为空 */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.empty();
        }

        /** @brief #### 最大元素数（理论值） */
        size_type max_size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.max_size();
        }

        /** @brief #### 当前容量（已分配空间大小） */
        size_type capacity() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.capacity();
        }

        /**
     * @brief #### 随机下标访问（带边界检查）
     * @param pos 下标位置
     * @return 元素的引用
     * @throw std::out_of_range 越界则抛出异常
     */
        value_type &at(size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.at(pos);
        }

        /** @brief #### 随机下标访问（只读） */
        const value_type &at(size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector.at(pos);
        }

        /**
     * @brief #### 随机下标访问（越界行为未定义）
     * @param pos 下标位置
     * @return 元素的引用
     */
        value_type &operator[](size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _vector[pos];
        }

        /** @brief #### 随机下标访问（只读） */
        const value_type &operator[](size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector[pos];
        }

        /**
     * @brief #### 在末尾追加元素（拷贝）
     * @param value_data 待追加元素
     */
        void push_back(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.push_back(value_data);
        }

        /** @brief #### 在末尾追加元素（移动） */
        void push_back(value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.push_back(std::move(value_data));
        }

        /**
     * @brief #### 就地构造追加元素
     * @param args 构造参数
     */
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.emplace_back(std::forward<Args>(args)...);
        }

        /**
     * @brief #### 删除末尾元素
     * @note 若 `vector` 为空，行为未定义（内部不做检查）
     */
        void pop_back()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (!_vector.empty())
                _vector.pop_back();
        }

        /**
     * @brief #### 删除末尾元素并返回其值
     * @param out 接收出栈元素的引用
     * @return `true` 成功；`false` 为空
     */
        bool pop_back(value &out)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (_vector.empty())
                return false;
            out = std::move(_vector.back());
            _vector.pop_back();
            return true;
        }

        /**
     * @brief #### 清空所有元素
     */
        void clear()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.clear();
        }

        /**
     * @brief #### 调整容器大小
     * @param count 新大小
     * @param value_data 新增元素的默认值
     */
        void resize(size_type count, const value &value_data = value())
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.resize(count, value_data);
        }

        /**
     * @brief #### 预留容量，避免多次重新分配
     * @param new_cap 期望容量
     */
        void reserve(size_type new_cap)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _vector.reserve(new_cap);
        }

        /**
     * @brief #### 与另一线程安全 `vector` 交换内容
     * @param other 另一个实例
     */
        void swap(concurrent_vector &other)
        {
            if (this == &other)
                return;
            std::unique_lock<std::shared_mutex> lhs_lock(_access_mutex);
            std::unique_lock<std::shared_mutex> rhs_lock(other._access_mutex);
            _vector.swap(other._vector);
        }

        /**
     * @brief #### 在指定位置插入元素（拷贝）
     * @param pos 插入位置下标
     * @param value_data 待插入元素
     */
        void insert(size_type pos, const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (pos > _vector.size())
                return;
            _vector.insert(_vector.begin() + pos, value_data);
        }

        /** @brief #### 在指定位置插入元素（移动） */
        void insert(size_type pos, value &&value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (pos > _vector.size())
                return;
            _vector.insert(_vector.begin() + pos, std::move(value_data));
        }

        /**
     * @brief #### 删除指定位置的元素
     * @param pos 待删除位置下标
     */
        void erase(size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (pos >= _vector.size())
                return;
            _vector.erase(_vector.begin() + pos);
        }

        /**
     * @brief #### 删除区间元素
     * @param first 起始下标
     * @param last  终止下标（不含）
     */
        void erase(size_type first, size_type last)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            if (first >= _vector.size() || first >= last)
                return;
            if (last > _vector.size())
                last = _vector.size();
            _vector.erase(_vector.begin() + first, _vector.begin() + last);
        }

        /**
     * @brief #### 判断元素是否存在
     * @param value_data 待查找值
     * @return `true` 存在；`false` 不存在
     */
        bool contains(const value &value_data) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            for (const auto &v : _vector)
                if (v == value_data)
                    return true;
            return false;
        }

        /**
     * @brief #### 获取当前 `vector` 的只读快照
     * @return `std::vector<value>` 元素副本，顺序与内部一致
     * @note  内部加读锁，拷贝后立即释放，外部可安全遍历
     */
        std::vector<value> snapshot() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _vector;
        }
    };  
}