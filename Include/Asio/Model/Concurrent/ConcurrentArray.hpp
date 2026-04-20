
#pragma once
#include <array>
#include <shared_mutex>
#include <iterator>
#include <algorithm>

namespace multi_concurrent
{
    /**
   * @file concurrent_array.hpp
   * @brief 线程安全的固定大小数组容器
   * @details 对 std::array 进行线程安全封装，支持多生产者多消费者并发访问。
   *          内部通过 std::shared_mutex 实现同步：读操作（如访问元素、遍历）加共享锁，
   *          写操作（如修改元素）加独占锁，平衡并发性能与线程安全。
   * @tparam value 元素类型
   * @tparam cap_size 数组固定大小
   */
    template <typename value, std::size_t cap_size>
    class concurrent_array
    {
    public:
        using value_type      = value;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference       = value &;
        using const_reference = const value &;
        using pointer         = value *;
        using const_pointer   = const value *;
        using iterator        = typename std::array<value, cap_size>::const_iterator;
        using const_iterator  = iterator;

    private:
        mutable std::shared_mutex _access_mutex;
        std::array<value, cap_size> _array;

    public:
        /** @brief 默认构造，值初始化每个元素 */
        concurrent_array() = default;

        /** @brief 用初始化列表构造 */
        concurrent_array(std::initializer_list<value> init)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            std::copy(init.begin(), init.end(), _array.begin());
        }

        /** @brief 拷贝构造（线程安全） */
        concurrent_array(const concurrent_array &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._access_mutex);
            _array = other._array;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_array &operator=(const concurrent_array &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
                _array = other._array;
            }
            return *this;
        }

        /** @brief 移动构造（禁用，避免线程安全隐患） */
        concurrent_array(concurrent_array &&) = delete;

        /** @brief 移动赋值（禁用，避免线程安全隐患） */
        concurrent_array &operator=(concurrent_array &&) = delete;
        /** @brief #### 获取元素（只读） */
        const_reference at(size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.at(pos);
        }
        reference at(size_type pos)
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.at(pos);
        }

        /** @brief 获取元素（只读，下标操作） */
        const_reference operator[](size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array[pos];
        }
        reference operator[](size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            return _array[pos];
        }


        /** @brief #### 获取第一个元素（只读） */
        const_reference front() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.front();
        }

        /** @brief #### 获取最后一个元素（只读） */
        const_reference back() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.back();
        }

        /** @brief #### 获取原始指针（只读） */
        const_pointer data() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.data();
        }

        /** @brief #### 判断数组是否为空（固定大小，恒为 false，除非 cap_size=0） */
        bool empty() const noexcept
        {
            return cap_size == 0;
        }

        /** @brief #### 获取数组大小 */
        size_type size() const noexcept
        {
            return cap_size;
        }

        /** @brief #### 获取数组最大大小（同 size） */
        size_type max_size() const noexcept
        {
            return cap_size;
        }

        /** @brief #### 填充数组元素） */
        void fill(const value &value_data)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _array.fill(value_data);
        }

        /** @brief #### 交换两个数组 */
        void swap(concurrent_array &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::unique_lock<std::shared_mutex> other_lock(other._access_mutex);
                _array.swap(other._array);
            }
        }

        /** @brief #### 起始迭代器（只读） */
        iterator begin() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.begin();
        }

        /** @brief #### 结束迭代器（只读） */
        iterator end() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _array.end();
        }

        /** @brief #### 起始迭代器） */
        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        /** @brief #### 结束迭代器 */
        const_iterator cend() const noexcept
        {
            return end();
        }
    };
}