#pragma once
#include <mutex>
#include <bitset>
#include <shared_mutex>
#include <string>

namespace multi_concurrent
{
    /**
   * @file concurrent_bitset.hpp
   * @brief 线程安全的位集合容器
   * @details 对 std::bitset 进行线程安全封装，支持多生产者多消费者并发访问。
   *          内部通过 std::shared_mutex 实现同步：读操作（如位查询、转换字符串）加共享锁，
   *          写操作（如置位、复位、翻转）加独占锁，保证位操作的线程安全。
   * @tparam cap_size 位集合的大小
   */
    template <std::size_t cap_size>
    class concurrent_bitset
    {
    public:
        using size_type = std::size_t;

    private:
        mutable std::shared_mutex _access_mutex;
        std::bitset<cap_size> _bitset;

    public:
        /** @brief 默认构造，所有位初始化为 0 */
        concurrent_bitset() = default;

        /** @brief 用无符号长整型初始化（低 cap_size 位） */
        explicit concurrent_bitset(unsigned long long val) : _bitset(val) {}

        /** @brief 拷贝构造（线程安全） */
        concurrent_bitset(const concurrent_bitset &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._access_mutex);
            _bitset = other._bitset;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_bitset &operator=(const concurrent_bitset &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
                _bitset = other._bitset;
            }
            return *this;
        }

        /** @brief 移动构造（禁用，避免线程安全隐患） */
        concurrent_bitset(concurrent_bitset &&) = delete;

        /** @brief 移动赋值（禁用，避免线程安全隐患） */
        concurrent_bitset &operator=(concurrent_bitset &&) = delete;

        /** @brief #### 翻转所有位（写操作） */
        concurrent_bitset &flip()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.flip();
            return *this;
        }

        /** @brief #### 翻转指定位置的位（写操作） */
        concurrent_bitset &flip(size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.flip(pos);
            return *this;
        }

        /** @brief #### 将所有位置为 1（写操作） */
        concurrent_bitset &set()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.set();
            return *this;
        }

        /** @brief #### 将指定位置为 1（写操作） */
        concurrent_bitset &set(size_type pos, bool val = true)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.set(pos, val);
            return *this;
        }

        /** @brief #### 将所有位置为 0（写操作） */
        concurrent_bitset &reset()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.reset();
            return *this;
        }

        /** @brief #### 将指定位置为 0（写操作） */
        concurrent_bitset &reset(size_type pos)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _bitset.reset(pos);
            return *this;
        }

        /** @brief #### 测试指定位是否为 1（读操作） */
        bool test(size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.test(pos);
        }

        /** @brief 转换为布尔值（所有位全 1 则为 true，否则 false）（读操作） */
        explicit operator bool() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return static_cast<bool>(_bitset);
        }

        /** @brief 获取指定位的值（读操作） */
        bool operator[](size_type pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset[pos];
        }

        /** @brief 位与操作（返回新的 bitset，不修改自身）（读操作） */
        concurrent_bitset operator&(const concurrent_bitset &rhs) const
        {
            std::shared_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            concurrent_bitset result;
            result._bitset = _bitset & rhs._bitset;
            return result;
        }

        /** @brief 位或操作（返回新的 bitset，不修改自身）（读操作） */
        concurrent_bitset operator|(const concurrent_bitset &rhs) const
        {
            std::shared_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            concurrent_bitset result;
            result._bitset = _bitset | rhs._bitset;
            return result;
        }

        /** @brief 位异或操作（返回新的 bitset，不修改自身）（读操作） */
        concurrent_bitset operator^(const concurrent_bitset &rhs) const
        {
            std::shared_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            concurrent_bitset result;
            result._bitset = _bitset ^ rhs._bitset;
            return result;
        }

        /** @brief 位取反操作（返回新的 bitset，不修改自身）（读操作） */
        concurrent_bitset operator~() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            concurrent_bitset result;
            result._bitset = ~_bitset;
            return result;
        }

        /** @brief 位与赋值（写操作） */
        concurrent_bitset &operator&=(const concurrent_bitset &rhs)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            _bitset &= rhs._bitset;
            return *this;
        }

        /** @brief 位或赋值（写操作） */
        concurrent_bitset &operator|=(const concurrent_bitset &rhs)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            _bitset |= rhs._bitset;
            return *this;
        }

        /** @brief 位异或赋值（写操作） */
        concurrent_bitset &operator^=(const concurrent_bitset &rhs)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> rhs_lock(rhs._access_mutex);
            _bitset ^= rhs._bitset;
            return *this;
        }

        /** @brief #### 转换为 unsigned long 整数（读操作） */
        unsigned long to_ulong() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.to_ulong();
        }

        /** @brief 转换为 #### unsigned long long 整数（读操作） */
        unsigned long long to_ullong() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.to_ullong();
        }

        /** @brief #### 转换为字符串（读操作，'0' 和 '1' 组成） */
        std::string to_string(char zero = '0', char one = '1') const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.to_string(zero, one);
        }

        /** @brief #### 获取位集合大小 */
        size_type size() const noexcept
        {
            return _bitset.size();
        }

        /** @brief #### 统计置位（值为 1）的位数（读操作） */
        size_type count() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.count();
        }

        /** @brief #### 判断是否所有位都为 1（读操作） */
        bool all() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.all();
        }

        /** @brief #### 判断是否任意位为 1（读操作） */
        bool any() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.any();
        }

        /** @brief #### 判断是否所有位都为 0（读操作） */
        bool none() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _bitset.none();
        }
    };
}