#pragma once

#include <string>
#include <shared_mutex>
#include <algorithm>

namespace multi_concurrent
{
    /**
   * @file concurrent_string.hpp
   * @brief 线程安全的字符串容器
   * @details 对 std::string 进行线程安全封装，支持多生产者多消费者并发访问。
   *          内部通过 std::shared_mutex 实现同步：读操作（如访问字符、获取子串）加共享锁，
   *          写操作（如修改字符、拼接字符串）加独占锁，保证字符串操作的线程安全。
   */
    class concurrent_string
    {
    private:
        mutable std::shared_mutex _access_mutex;
        std::string _string;

    public:
        /** @brief 默认构造空字符串 */
        concurrent_string() = default;

        /** @brief 用 C 风格字符串构造 */
        concurrent_string(const char *s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = s;
        }

        /** @brief 用 std::string 构造 */
        concurrent_string(const std::string &s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = s;
        }

        /** @brief 用 std::string 移动构造 */
        concurrent_string(std::string &&s) noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = std::move(s);
        }

        /** @brief 拷贝构造（线程安全） */
        concurrent_string(const concurrent_string &other)
        {
            std::shared_lock<std::shared_mutex> lock(other._access_mutex);
            _string = other._string;
        }

        /** @brief 拷贝赋值（线程安全） */
        concurrent_string &operator=(const concurrent_string &other)
        {
            if (this != &other)
            {
                std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
                std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
                _string = other._string;
            }
            return *this;
        }

        /** @brief 移动赋值（禁用，避免线程安全隐患） */
        concurrent_string &operator=(concurrent_string &&) = delete;

        /** @brief 赋值 C 风格字符串（写操作） */
        concurrent_string &operator=(const char *s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = s;
            return *this;
        }

        /** @brief 赋值 std::string（写操作） */
        concurrent_string &operator=(const std::string &s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = s;
            return *this;
        }

        /** @brief 赋值 std::string（移动，写操作） */
        concurrent_string &operator=(std::string &&s) noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string = std::move(s);
            return *this;
        }

        /** @brief #### 获取字符串长度（读操作） */
        size_t length() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.length();
        }

        /** @brief #### 获取字符串大小（同 length，读操作） */
        size_t size() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.size();
        }

        /** @brief #### 判断字符串是否为空（读操作） */
        bool empty() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.empty();
        }

        /** @brief #### 清空字符串（写操作） */
        void clear()
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string.clear();
        }

        /** @brief #### 调整字符串容量（写操作） */
        void reserve(size_t n)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string.reserve(n);
        }

        /** @brief #### 获取字符（只读，读操作） */
        char at(size_t pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.at(pos);
        }

        /** @brief #### 获取字符（只读，下标操作，读操作） */
        char operator[](size_t pos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string[pos];
        }

        /** @brief #### 获取 C 风格字符串（只读，读操作） */
        const char *c_str() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.c_str();
        }

        /** @brief 获取 #### std::string 副本（读操作） */
        std::string str() const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string;
        }

        /** @brief 拼接字符（写操作） */
        concurrent_string &operator+=(char c)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string += c;
            return *this;
        }

        /** @brief 拼接 C 风格字符串（写操作） */
        concurrent_string &operator+=(const char *s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string += s;
            return *this;
        }

        /** @brief 拼接 std::string（写操作） */
        concurrent_string &operator+=(const std::string &s)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string += s;
            return *this;
        }

        /** @brief 拼接 concurrent_string（线程安全，写操作） */
        concurrent_string &operator+=(const concurrent_string &other)
        {
            std::unique_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
            _string += other._string;
            return *this;
        }

        /** @brief 字符串拼接（返回新字符串，读操作） */
        // concurrent_string operator+(concurrent_string lhs, const concurrent_string &rhs);

        /** @brief 比较字符串（读操作） */
        bool operator==(const concurrent_string &other) const
        {
            std::shared_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
            return _string == other._string;
        }

        /** @brief 比较字符串（读操作） */
        bool operator!=(const concurrent_string &other) const
        {
            return !(*this == other);
        }

        /** @brief 比较字符串（读操作） */
        bool operator<(const concurrent_string &other) const
        {
            std::shared_lock<std::shared_mutex> self_lock(_access_mutex);
            std::shared_lock<std::shared_mutex> other_lock(other._access_mutex);
            return _string < other._string;
        }

        /** @brief 比较字符串（读操作） */
        bool operator>(const concurrent_string &other) const
        {
            return other < *this;
        }

        /** @brief 比较字符串（读操作） */
        bool operator<=(const concurrent_string &other) const
        {
            return !(*this > other);
        }

        /** @brief 比较字符串（读操作） */
        bool operator>=(const concurrent_string &other) const
        {
            return !(*this < other);
        }

        /** @brief #### 查找子串（读操作） */
        size_t find(const std::string &str, size_t pos = 0) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.find(str, pos);
        }

        /** @brief #### 替换子串（写操作） */
        concurrent_string &replace(size_t pos, size_t len, const std::string &str)
        {
            std::unique_lock<std::shared_mutex> lock(_access_mutex);
            _string.replace(pos, len, str);
            return *this;
        }

        /** @brief #### 截取子串（读操作） */
        std::string substr(size_t pos = 0, size_t len = std::string::npos) const
        {
            std::shared_lock<std::shared_mutex> lock(_access_mutex);
            return _string.substr(pos, len);
        }
    };

}
// 在 concurrent_string.hpp 末尾（或对应的 .cpp 文件中）
multi_concurrent::concurrent_string operator+(const multi_concurrent::concurrent_string& lhs, const multi_concurrent::concurrent_string& rhs)
{
    multi_concurrent::concurrent_string result(lhs.str());   
    result += rhs;                         
    return result;
}