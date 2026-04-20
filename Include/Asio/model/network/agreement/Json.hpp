/**
 * @file Json.hpp
 * @brief JSON协议定义
 * @details 提供`JSON`协议对象的定义与操作
 */
#pragma once
#include <boost/Json.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <string_view>

namespace protocol
{
    class json
    {
    private:
        boost::json::value _value;                 // JSON值
        mutable std::string _cached_string;        // 缓存的JSON字符串
        mutable bool _string_cache_valid = false;  // 字符串缓存是否有效

    private:
        /**
     * @brief 无效化字符串缓存
     */
        void _invalidate_cache() const
        {
            _string_cache_valid = false;
        }
    public:
        json() : _value(boost::json::object{}) {}

        explicit json(const boost::json::value &value) : _value(value) {}

        explicit json(boost::json::value &&value) noexcept : _value(std::move(value)) {}

        explicit json(const std::string &json_str)
        {
            from_string(json_str);
        }

        explicit json(std::string_view json_str)
        {
            from_string(json_str);
        }

        // 拷贝和移动语义 - 优化缓存处理
        json(const json &other) 
        : _value(other._value), _cached_string(other._cached_string), _string_cache_valid(other._string_cache_valid) 
        {
            // 拷贝时保留缓存状态，避免重复序列化
        }

        json(json &&other) noexcept
        : _value(std::move(other._value)), _cached_string(std::move(other._cached_string)), _string_cache_valid(other._string_cache_valid)
        {
            other._string_cache_valid = false;
        }

        json &operator=(const json &other)
        {
            if (this != &other)
            {
                _value = other._value;
                _cached_string = other._cached_string;
                _string_cache_valid = other._string_cache_valid;
                // 拷贝赋值时保留缓存状态，避免重复序列化
            }
            return *this;
        }

        json &operator=(json &&other) noexcept
        {
            if (this != &other)
            {
                _value = std::move(other._value);
                _cached_string = std::move(other._cached_string);
                _string_cache_valid = other._string_cache_valid;
                other._string_cache_valid = false;
            }
            return *this;
        }
        /**
     * @brief 从字符串解析`JSON`
     * @param json_str `JSON`字符串
     * @return 解析是否成功
     */
        bool from_string(std::string_view json_str) noexcept
        {
            try
            {
                _value = boost::json::parse(json_str);
                _invalidate_cache();
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        /**
     * @brief 转换为字符串
     * @return JSON字符串表示
     * @details 提供异常安全保证，序列化失败时返回空字符串
     */
        const std::string &to_string() const noexcept
        {
            if (!_string_cache_valid)
            {
                try
                {
                    _cached_string = boost::json::serialize(_value);
                    _string_cache_valid = true;
                }
                catch (...)
                {
                    // 序列化失败时返回空字符串，保持缓存无效状态
                    _cached_string = "";
                    // 不设置_string_cache_valid = true，下次调用时会重试
                }
            }
            return _cached_string;
        }
        /**
     * @brief 获取原始boost::json::value引用
     * @return boost::json::value引用
     */
        const boost::json::value &value() const noexcept { return _value; }

        boost::json::value &value() noexcept
        {
            _invalidate_cache();
            return _value;
        }

        /**
     * @brief 简化的类型安全值获取（支持任意类型）
     * @tparam T 目标类型（支持所有`boost::json::value_to`支持的类型）
     * @param key 键名
     * @param default_value 转换失败时的默认值
     * @return 从JSON中获取的值或默认值
     */
        template <typename T>
        T get(const std::string &key, const T &default_value = T{}) const noexcept
        {
            try
            {
                // 确保JSON是对象类型且包含指定键
                if (!_value.is_object() || !_value.as_object().contains(key))
                    return default_value;
                // 利用boost::json::value_to转换为目标类型（支持任意类型）
                return boost::json::value_to<T>(_value.as_object().at(key));
            }
            catch (...)
            {
                return default_value;
            }
        }

        /**
     * @brief 简化的类型安全值设置（支持任意类型）
     * @tparam T 值类型（支持所有boost::json::value_from支持的类型）
     * @param key 键名
     * @param value 要设置的值（任意类型）
     */
        template <typename T>
        void set(const std::string &key, const T &value)
        {
            try
            {
                // 确保JSON是对象类型
                if (!_value.is_object())
                    _value = boost::json::object{};

                // 利用boost::json::value_from将任意类型转换为boost::json::value
                _value.as_object()[key] = boost::json::value_from(value);
                _invalidate_cache();
            }
            catch (...)
            {
                // 转换失败时可根据需求处理（此处忽略，保持原逻辑）
            }
        }

        /**
     * @brief 检查是否包含指定键
     * @param key 键名
     * @return 是否包含该键
     */
        bool contains(const std::string &key) const noexcept
        {
            if (!_value.is_object())
                return false;

            const auto &obj = _value.as_object();
            return obj.find(key) != obj.end();
        }

        /**
     * @brief 移除指定键
     * @param key 键名
     * @return 是否成功移除
     */
        bool remove(const std::string &key) noexcept
        {
            if (!_value.is_object())
                return false;

            auto &obj = _value.as_object();
            auto it = obj.find(key);
            if (it != obj.end())
            {
                obj.erase(it);
                _invalidate_cache();
                return true;
            }
            return false;
        }

        /**
     * @brief 清空所有内容
     */
        void clear() noexcept
        {
            _value = boost::json::object{};
            _invalidate_cache();
        }

        /**
     * @brief 获取对象大小（键值对数量）
     * @return 大小
     */
        std::size_t size() const noexcept
        {
            if (_value.is_object())
                return _value.as_object().size();
            if (_value.is_array())
                return _value.as_array().size();
            return 0;
        }

        /**
     * @brief 检查是否为空
     * @return 是否为空
     */
        bool empty() const noexcept
        {
            return size() == 0;
        }

        bool operator==(const json &other) const noexcept
        {
            return _value == other._value;
        }

        bool operator!=(const json &other) const noexcept
        {
            return !(*this == other);
        }
    };
} // end namespace protocol