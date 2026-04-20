/**
 * @file Protocol.hpp
 * @brief 协议定义
 * @details 提供基础协议头、协议体、协议转换等基础组件
 */
#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <concepts> //模板约束
#include <algorithm>
#include <charconv>
#include <cctype>
#include <format>
#include <cstdint>
#include <utility>
#include <boost/Json.hpp>
#include "./Json.hpp"
#include "./Auxiliary.hpp"
#include "../crypt/Encryption.hpp"

namespace protocol
{
    /**
   * @brief 协议头约束
   * @details 定义了协议头的基础接口，要求实现序列化、反序列化、校验和计算等功能
   */
    template <class header_t>
    concept header_constraint = requires(header_t header,std::string_view data,const protocol::json& json_object,std::string_view content) 
    {
        { header.to_string() } -> std::same_as<std::string>;
        { header.from_string(data) } -> std::same_as<bool>;
        { header.to_json() } -> std::same_as<protocol::json>;
        { header.from_json(json_object) } -> std::same_as<bool>;
        { header.verify_integrity(content) } -> std::same_as<bool>;
        { header.calculate_and_set_checksum(content) } -> std::same_as<std::uint32_t>;
    };
    /**
   * @brief `TCP`协议请求头类
   * @details 实现`TCP`协议的请求头，支持自定义方法和高性能解析
   */
    class request_header : public auxiliary::protocol_header
    {
    private:
        std::string _method;                              // 请求方法
        std::string _target;                              // 请求目标
        std::string _user_agent;                          // 用户代理
        std::chrono::system_clock::time_point _timestamp; // 时间戳
    private:
        /**
     * @brief 序列化头部字段为字符串
     * @param out 输出字符串
     */
        void _serialize_headers_to_string(std::string &out) const
        {
            std::vector<std::string> keys;
            keys.reserve(_headers.size());
            for (const auto &[key, value] : _headers)
                keys.push_back(key);
            std::sort(keys.begin(), keys.end());
            for (const std::string &key : keys)
            {
                const std::string &value = _headers.at(key);
                out.append(key);
                out.append(": ");
                out.append(value);
                out.append("\r\n");
            }
        }
    public:
        request_header()
        {
            _timestamp = std::chrono::system_clock::now();
            _protocol_type = auxiliary::protocol_type::CUSTOM_TCP;
        }

        const std::string &get_method() const noexcept { return _method; }
        void set_method(const std::string &method) { _method = method; }

        const std::string &get_target() const noexcept { return _target; }
        void set_target(const std::string &target) { _target = target; }

        const std::string &get_user_agent() const noexcept { return _user_agent; }
        void set_user_agent(const std::string &user_agent) { _user_agent = user_agent; }

        const std::chrono::system_clock::time_point &get_timestamp() const noexcept { return _timestamp; }
        void set_timestamp(const std::chrono::system_clock::time_point &timestamp) { _timestamp = timestamp; }

        /**
     * @brief 序列化为字符串
     * @return 序列化后的字符串
     */
        std::string to_string() const override
        {
            std::size_t estimated_size = _method.size() + _target.size() + 64;
            for (const auto &[key, value] : _headers)
                estimated_size += key.size() + value.size() + 4; // ": " + "\r\n"

            std::string result;
            result.reserve(estimated_size);

            result += std::format("{} {} {} {} {} {}\r\n",_method, _target, _version,
            static_cast<std::uint8_t>(_checksum_type),_checksum_value, _content_length);

            if (!_user_agent.empty())
                result += std::format("User-Agent: {}\r\n", _user_agent);
            auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
            result += std::format("Timestamp: {}\r\n", timestamp_ms);

            _serialize_headers_to_string(result);
            result += "\r\n";
            return result;
        }
        /**
     * @brief 从字符串反序列化
     * @param data 字符串数据
     * @return 是否成功
     */
        bool from_string(std::string_view data) override;

        /**
     * @brief 转换为JSON
     * @return JSON对象
     */
        protocol::json to_json() const override
        {
            protocol::json json_object = protocol_header::to_json();
            json_object.set("method", _method);
            json_object.set("target", _target);
            json_object.set("user_agent", _user_agent);

            auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
            json_object.set("timestamp", timestamp_ms);

            return json_object;
        }
        /**
     * @brief 从`JSON`反序列化
     * @param json_object `JSON`对象
     * @return 是否成功
     */
        bool from_json(const protocol::json &json_object) override
        {
            if (!protocol_header::from_json(json_object))
                return false;

            try
            {
                _method = json_object.get<std::string>("method", "");
                _target = json_object.get<std::string>("target", "");
                _user_agent = json_object.get<std::string>("user_agent", "");
                auto timestamp_ms = json_object.get<std::int64_t>("timestamp", 0);
                _timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp_ms));
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
    }; // end class request_header

    /**
   * @brief `TCP`协议响应头类
   * @details 实现`TCP`协议的响应头
   */
    class response_header : public auxiliary::protocol_header
    {
    private:
        std::string _server;                              // 服务器信息
        std::uint16_t _status_code = 200;                 // 状态码
        std::string _status_message = "OK";               // 状态消息
        std::chrono::system_clock::time_point _timestamp; // 时间戳
    private:
        /**
     * @brief 序列化头部字段为字符串
     * @param out 输出字符串
     */
        void _serialize_headers_to_string(std::string &out) const
        {
            std::vector<std::string> keys;
            keys.reserve(_headers.size());
            for (const auto &[key, value] : _headers)
                keys.push_back(key);
            std::sort(keys.begin(), keys.end());
            for (const std::string &key : keys)
            {
                const std::string &value = _headers.at(key);
                out.append(key);
                out.append(": ");
                out.append(value);
                out.append("\r\n");
            }
        }
    public:
        response_header()
        {
            _timestamp = std::chrono::system_clock::now();
            _protocol_type = auxiliary::protocol_type::CUSTOM_TCP;
        }
        std::uint16_t get_status_code() const noexcept { return _status_code; }
        void set_status_code(std::uint16_t code) { _status_code = code; }

        const std::string &get_status_message() const noexcept { return _status_message; }
        void set_status_message(const std::string &message) { _status_message = message; }

        const std::string &get_server() const noexcept { return _server; }
        void set_server(const std::string &server) { _server = server; }

        const std::chrono::system_clock::time_point &get_timestamp() const noexcept { return _timestamp; }
        void set_timestamp(const std::chrono::system_clock::time_point &timestamp) { _timestamp = timestamp; }

        std::string to_string() const override
        {
            std::size_t estimated_size = _status_message.size() + 64;
            for (const auto &[key, value] : _headers)
                estimated_size += key.size() + value.size() + 4;
            std::string result;
            result.reserve(estimated_size);
            result += std::format("{} {} {} {} {} {}\r\n",_version, _status_code, _status_message,
            static_cast<std::uint8_t>(_checksum_type),_checksum_value, _content_length);
            if (!_server.empty())
                result += std::format("Server: {}\r\n", _server);
            auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
            result += std::format("Timestamp: {}\r\n", timestamp_ms);
            _serialize_headers_to_string(result);
            result += "\r\n";
            return result;
        }
        bool from_string(std::string_view data) override;
        /**
     * @brief 转换为JSON
     * @return JSON对象
     */
        protocol::json to_json() const override
        {
            protocol::json json_object = protocol_header::to_json();
            json_object.set("status_code", _status_code);
            json_object.set("status_message", _status_message);
            json_object.set("server", _server);
            auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timestamp.time_since_epoch()).count();
            json_object.set("timestamp", timestamp_ms);
            return json_object;
        }
        bool from_json(const protocol::json &json_object) override
        {
            if (!protocol_header::from_json(json_object))
                return false;
            try
            {
                _status_code = json_object.get<std::uint16_t>("status_code", 200);
                _status_message = json_object.get<std::string>("status_message", "OK");
                _server = json_object.get<std::string>("server", "");
                auto timestamp_ms = json_object.get<std::int64_t>("timestamp", 0);
                _timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp_ms));
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
    }; // end class response_header

    /**
   * @brief `TCP`协议请求类
   * @details 实现`TCP`协议的请求
   * @warning 使用该类需检查请求头是否符合 `header_constraint` 约束
   * @note 该类默认使用`request_header`作为请求头类型
   */
    template <header_constraint header_t = request_header>
    class request 
    {
    private:
        header_t _header;                       // 请求头
        std::string _message;                   // 请求体
        mutable std::string _cached_full;       // 缓存的完整请求字符串
        mutable bool _full_cache_valid = false; // 完整缓存是否有效
    private:
        /**
     * @brief 清除缓存
     */
        void _invalidate_cache() const noexcept
        {
            _full_cache_valid = false;
        } 
    public:
        request() = default;

        explicit request(const header_t &header) : _header(header) {}

        request(const header_t &header, const std::string &body)
        : _header(header), _message(body) {}

        request(const header_t &header, std::string &&body)
        : _header(header), _message(std::move(body)) {}

        // 拷贝和移动语义
        request(const request &other)
        : _header(other._header), _message(other._message) {}

        request(request &&other) noexcept
        : _header(std::move(other._header)), _message(std::move(other._message)), 
        _cached_full(std::move(other._cached_full)), _full_cache_valid(other._full_cache_valid)
        {
            other._full_cache_valid = false;
        }

        request &operator=(const request &other)
        {
            if (this != &other)
            {
                _header = other._header;
                _message = other._message;
                _invalidate_cache();
            }
            return *this;
        }

        request &operator=(request &&other) noexcept
        {
            if (this != &other)
            {
                _header = std::move(other._header);
                _message = std::move(other._message);
                _cached_full = std::move(other._cached_full);
                _full_cache_valid = other._full_cache_valid;
                other._full_cache_valid = false;
            }
            return *this;
        }
        const header_t &header() const noexcept { return _header; }
        header_t &header() noexcept
        {
            _invalidate_cache();
            return _header;
        }

        const std::string &body() const noexcept { return _message; }

        void set_message(const std::string &body)
        {
            _message = body;
            _invalidate_cache();
        }

        void set_message(std::string &&body)
        {
            _message = std::move(body);
            _invalidate_cache();
        }
        /**
     * @brief 序列化为完整的请求字符串
     * @return 完整的请求字符串
     */
        const std::string &to_string() const
        {
            if (!_full_cache_valid)
            { // `calculate_and_set_checksum` 作用： 计算消息体校验码并设置校验码
                const_cast<header_t &>(_header).calculate_and_set_checksum(_message);

                // 优化字符串拼接性能，预分配内存避免多次重新分配
                const auto& header_str = _header.to_string();
                _cached_full.clear();
                _cached_full.reserve(header_str.size() + _message.size());
                _cached_full.append(header_str);
                _cached_full.append(_message);
                _full_cache_valid = true;
            }
            return _cached_full;
        }
        /**
     * @brief 从字符串反序列化
     * @param data 完整的请求字符串
     * @return 是否成功
     */
        bool from_string(std::string_view data)
        {
            std::size_t header_end = data.find("\r\n\r\n");
            if (header_end == std::string_view::npos)
                return false;

            std::string_view header_data = data.substr(0, header_end + 2); // 包含最后的\r\n
            if (!_header.from_string(header_data))
                return false;
            // 提取body
            std::size_t body_start = header_end + 4; // 跳过 "\r\n\r\n"
            if (body_start < data.size())
                _message = std::string(data.substr(body_start));
            else
                _message.clear();
            _invalidate_cache();
            return _header.verify_integrity(_message);
        }
        /**
     * @brief 转换为JSON
     * @return JSON对象
     */
        protocol::json to_json() const
        {
            protocol::json json_object;
            const_cast<header_t &>(_header).calculate_and_set_checksum(_message);
            json_object.set("header", _header.to_json().value());
            json_object.set("body", _message);
            return json_object;
        }
        /**
     * @brief 从`JSON`反序列化
     * @param json_object JSON对象
     * @return 是否成功
     */
        bool from_json(const protocol::json &json_object)
        {
            try
            {
                std::string header_json_str = json_object.get<std::string>("header", "");
                if (!header_json_str.empty())
                {
                    protocol::json header_json(header_json_str);
                    if (!_header.from_json(header_json))
                        return false;
                }
                _message = json_object.get<std::string>("body", "");
                _invalidate_cache();
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        /**
     * @brief 验证请求完整性
     * @return 验证是否通过
     */
        bool verify_integrity() const
        {
            return _header.verify_integrity(_message);
        }
        /**
     * @brief 获取请求大小（字节）
     * @return 请求大小
     */
        std::size_t size() const
        {
            return to_string().size();
        }
        /**
     * @brief 检查是否为空请求
     * @return 是否为空
     */
        bool empty() const noexcept
        {
            return _message.empty();
        }

        bool operator==(const request &other) const noexcept
        {
            return _header.to_string() == other._header.to_string() && _message == other._message;
        }

        bool operator!=(const request &other) const noexcept
        {
            return !(*this == other);
        }
    }; // end class request

    /**
   * @brief `TCP`协议响应头类
   * @details 实现`TCP`协议的响应头
   * @warning 使用该类需检查响应头是否符合 `header_constraint` 约束
   * @note 该类默认使用`response_header`作为响应头类型
   */
    template<header_constraint header_t = response_header>
    class response
    {
    private:
        header_t _header;                       // 响应头
        std::string _message;                   // 响应体
        mutable std::string _cached_full;       // 缓存的完整响应字符串
        mutable bool _full_cache_valid = false; // 完整缓存是否有效

    private:
        /**
     * @brief 清除缓存
     */
        void _invalidate_cache() const noexcept
        {
            _full_cache_valid = false;
        }

    public:
        response() = default;

        explicit response(const header_t &header) : _header(header) {}

        response(const header_t &header, const std::string &body)
        : _header(header), _message(body) {}

        response(const header_t &header, std::string &&body)
        : _header(header), _message(std::move(body)) {}

        // 便利构造函数（用于快速创建响应）
        response(std::uint16_t status_code, const std::string &status_message, const std::string &body = "")
        : _message(body)
        {
            if constexpr (std::is_same_v<header_t, response_header>)
            {
                _header.set_status_code(status_code);
                _header.set_status_message(status_message);
            }
        }
        response(const response &other)
        : _header(other._header), _message(other._message) {}

        response(response &&other) noexcept
        : _header(std::move(other._header)), _message(std::move(other._message)), _cached_full(std::move(other._cached_full)), _full_cache_valid(other._full_cache_valid)
        {
            other._full_cache_valid = false;
        }

        response &operator=(const response &other)
        {
            if (this != &other)
            {
                _header = other._header;
                _message = other._message;
                _invalidate_cache();
            }
            return *this;
        }

        response &operator=(response &&other) noexcept
        {
            if (this != &other)
            {
                _header = std::move(other._header);
                _message = std::move(other._message);
                _cached_full = std::move(other._cached_full);
                _full_cache_valid = other._full_cache_valid;
                other._full_cache_valid = false;
            }
            return *this;
        }
        const header_t &header() const noexcept { return _header; }

        header_t &header() noexcept
        {
            _invalidate_cache();
            return _header;
        }

        const std::string &body() const noexcept { return _message; }

        void set_message(const std::string &body)
        {
            _message = body;
            _invalidate_cache();
        }

        void set_message(std::string &&body)
        {
            _message = std::move(body);
            _invalidate_cache();
        }

        // 便利方法（仅适用于 response_header ） 如果类型不符合 则忽略函数

        template <typename current_header_t = header_t>
        std::enable_if_t<std::is_same_v<current_header_t, response_header>, std::uint16_t>
        get_status_code() const noexcept
        {
            return _header.get_status_code();
        }

        template <typename current_header_t = header_t>
        std::enable_if_t<std::is_same_v<current_header_t, response_header>, void>
        set_status_code(std::uint16_t code)
        {
            _header.set_status_code(code);
            _invalidate_cache();
        }

        template <typename current_header_t = header_t>
        std::enable_if_t<std::is_same_v<current_header_t, response_header>, const std::string &>
        get_status_message() const noexcept
        {
            return _header.get_status_message();
        }

        template <typename current_header_t = header_t>
        std::enable_if_t<std::is_same_v<current_header_t, response_header>, void>
        set_status_message(const std::string &message)
        {
            _header.set_status_message(message);
            _invalidate_cache();
        }

        const std::string &to_string() const
        {
            if (!_full_cache_valid)
            {
                // 先计算并设置校验值
                const_cast<header_t &>(_header).calculate_and_set_checksum(_message);

                // 优化字符串拼接性能，预分配内存避免多次重新分配
                const auto& header_str = _header.to_string();
                _cached_full.clear();
                _cached_full.reserve(header_str.size() + _message.size());
                _cached_full.append(header_str);
                _cached_full.append(_message);
                _full_cache_valid = true;
            }
            return _cached_full;
        }

        /**
     * @brief 从字符串反序列化
     * @param data 完整的响应字符串
     * @return 是否成功
     */
        bool from_string(std::string_view data)
        {
            // 查找头部结束标志
            std::size_t header_end = data.find("\r\n\r\n");
            if (header_end == std::string_view::npos)
                return false;

            // 解析头部
            std::string_view header_data = data.substr(0, header_end + 2); // 包含最后的\r\n
            if (!_header.from_string(header_data))
                return false;

            // 提取body
            std::size_t body_start = header_end + 4; // 跳过 "\r\n\r\n"
            if (body_start < data.size())
                _message = std::string(data.substr(body_start));
            else
                _message.clear();
            _invalidate_cache();
            // 验证数据完整性
            return _header.verify_integrity(_message);
        }

        protocol::json to_json() const
        {
            protocol::json json_object;
            const_cast<header_t &>(_header).calculate_and_set_checksum(_message);
            json_object.set("header", _header.to_json().value());
            json_object.set("body", _message);
            return json_object;
        }
        /**
     * @brief 从JSON反序列化
     * @param json_object JSON对象
     * @return 是否成功
     */
        bool from_json(const protocol::json &json_object)
        {
            try
            {
                std::string header_json_str = json_object.get<std::string>("header", "");
                if (!header_json_str.empty())
                {
                    protocol::json header_json(header_json_str);
                    if (!_header.from_json(header_json))
                        return false;
                }
                _message = json_object.get<std::string>("body", "");
                _invalidate_cache();
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        /**
     * @brief 验证响应完整性
     * @return 验证是否通过
     */
        bool verify_integrity() const
        {
            return _header.verify_integrity(_message);
        }
        /**
     * @brief 获取响应大小（字节）
     * @return 响应大小
     */
        std::uint64_t size() const
        {
            return to_string().size();
        }
        /**
     * @brief 检查是否为空响应
     * @return 是否为空
     */
        bool empty() const noexcept
        {
            return _message.empty();
        }

        bool operator==(const response &other) const noexcept
        {
            return _header.to_string() == other._header.to_string() && _message == other._message;
        }

        bool operator!=(const response &other) const noexcept
        {
            return !(*this == other);
        }

        static response create_success(const std::string &body = "", const std::string &server = "")
        {
            response resp(200, "OK", body);
            if constexpr (std::is_same_v<header_t, response_header>)
            {
                if (!server.empty())
                    resp._header.set_server(server);
            }
            return resp;
        }
        static response create_error(std::uint16_t code, const std::string &message, const std::string &body = "")
        {
            return response(code, message, body);
        }

        static response create_not_found(const std::string &body = "not found")
        {
            return response(404, "not found", body);
        }

        static response create_internal_error(const std::string &body = "internal server error")
        {
            return response(500, "internal server error", body);
        }
    }; // end class response
    namespace conversion
    {

    } // end namespace conversion
} // end namespace protocol


bool protocol::request_header::from_string(std::string_view data)
{
    // 边界检查：确保数据不为空且有最小长度
    if (data.empty() || data.size() < 10) // 最小合理长度检查
        return false;
        
    // 异常安全：使用RAII清理，只在成功时提交更改
    std::unordered_map<std::string, std::string> temp_headers;
    std::string temp_method, temp_target, temp_user_agent;
    std::uint32_t temp_version = 0, temp_checksum_value = 0, temp_content_length = 0;
    auxiliary::checksum_type temp_checksum_type = auxiliary::checksum_type::CRC32;
    std::chrono::system_clock::time_point temp_timestamp;
    
    std::size_t pos = 0;

    // 安全的解析函数，添加边界检查
    auto safe_parse = [](std::string_view sv, auto &out) noexcept -> bool
    {
        if (sv.empty() || sv.size() > 20) // 防止过长的数字字符串
            return false;
        auto result = std::from_chars(sv.data(), sv.data() + sv.size(), out);
        return result.ec == std::errc{} && result.ptr == sv.data() + sv.size();
    };

    // 查找第一行结束位置，添加边界检查
    const auto le = data.find("\r\n", pos);
    if (le == std::string_view::npos || le >= data.size() - 1)
        return false;
        
    // 解析第一行（请求行）
    std::vector<std::string_view> parts;
    parts.reserve(8); // 预分配空间避免重复分配
    
    for (std::size_t i = pos, s = pos; i <= le; ++i)
    {
        if (i == le || data[i] == ' ')
        {
            if (i > s && parts.size() < 10) // 限制parts数量防止内存攻击
                parts.push_back(data.substr(s, i - s));
            s = i + 1;
        }
    }
    
    // 严格检查parts数量
    if (parts.size() != 6)
        return false;
        
    // 验证方法和目标不为空且长度合理
    if (parts[0].empty() || parts[0].size() > 10 || 
            parts[1].empty() || parts[1].size() > 2048)
        return false;
        
    temp_method = std::string(parts[0]);
    temp_target = std::string(parts[1]);
    
    std::uint8_t ctype_val;
    if (!safe_parse(parts[2], temp_version) || !safe_parse(parts[3], ctype_val) ||
            !safe_parse(parts[4], temp_checksum_value) || !safe_parse(parts[5], temp_content_length))
        return false;
        
    // 验证枚举值的有效性（检查上下界）
    if (ctype_val < static_cast<std::uint8_t>(auxiliary::checksum_type::CRC32) ||
            ctype_val > static_cast<std::uint8_t>(auxiliary::checksum_type::SHA256))
        return false;
        
    temp_checksum_type = static_cast<auxiliary::checksum_type>(ctype_val);
    pos = le + 2;

    // 安全的trim函数，添加边界检查
    auto safe_trim = [](std::string_view &sv) -> bool
    {
        if (sv.size() > 8192) // 防止过长的字符串
            return false;
            
        const auto start = sv.find_first_not_of(" \t");
        if (start == std::string_view::npos)
        {
            sv = "";
            return true;
        }
        const auto end = sv.find_last_not_of(" \t");
        sv = sv.substr(start, end - start + 1);
        return true;
    };

    // 解析头部字段，添加循环计数器防止无限循环
    std::size_t header_count = 0;
    const std::size_t max_headers = 100; // 限制头部数量
    
    while (pos < data.size() && header_count < max_headers)
    {
        const auto next_le = data.find("\r\n", pos);
        if (next_le == std::string_view::npos)
            break;
        
        // 防止行过长
        if (next_le - pos > 8192)
            return false;
            
        std::string_view line = data.substr(pos, next_le - pos);
        pos = next_le + 2;
        
        if (line.empty())
            break;

        const auto colon = line.find(':');
        if (colon != std::string_view::npos && colon > 0 && colon < line.size() - 1)
        {
            std::string_view k = line.substr(0, colon);
            std::string_view v = line.substr(colon + 1);
            
            if (!safe_trim(k) || !safe_trim(v))
                return false;
                
            // 验证键值对的有效性
            if (k.empty() || k.size() > 256 || v.size() > 8192)
                return false;

            std::string key(k), val(v);
            
            if (key == "User-Agent")
            {
                if (val.size() > 512) // 限制User-Agent长度
                    return false;
                temp_user_agent = val;
            }
            else if (key == "Timestamp")
            {
                std::int64_t ts;
                if (safe_parse(v, ts))
                {
                    // 验证时间戳的合理性（不能是负数或过大）
                    if (ts < 0 || ts > std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch()).count() + 86400000) // 不能超过当前时间+1天
                        return false;
                    temp_timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(ts));
                }
            }
            else
            {
                if (temp_headers.size() >= max_headers - 10) // 为特殊头部预留空间
                    return false;
                temp_headers[key] = val;
            }
        }
        ++header_count;
    }
    
    // 所有解析成功，提交更改（异常安全）
    _headers = std::move(temp_headers);
    _method = std::move(temp_method);
    _target = std::move(temp_target);
    _user_agent = std::move(temp_user_agent);
    _version = temp_version;
    _checksum_value = temp_checksum_value;
    _content_length = temp_content_length;
    _checksum_type = temp_checksum_type;
    _timestamp = temp_timestamp;
    
    return true;
}

bool protocol::response_header::from_string(std::string_view data)
{
    // 边界检查：确保数据不为空且有最小长度
    if (data.empty() || data.size() < 10)
        return false;
        
    // 异常安全：使用RAII清理，只在成功时提交更改
    std::unordered_map<std::string, std::string> temp_headers;
    std::string temp_status_message, temp_server;
    std::uint32_t temp_version = 0, temp_checksum_value = 0, temp_content_length = 0;
    std::uint16_t temp_status_code = 0;
    auxiliary::checksum_type temp_checksum_type = auxiliary::checksum_type::CRC32;
    std::chrono::system_clock::time_point temp_timestamp;
    
    std::size_t pos = 0;

    // 安全的解析函数，添加边界检查
    auto safe_parse = [](std::string_view sv, auto &out) noexcept -> bool
    {
        if (sv.empty() || sv.size() > 20)
            return false;
        auto result = std::from_chars(sv.data(), sv.data() + sv.size(), out);
        return result.ec == std::errc{} && result.ptr == sv.data() + sv.size();
    };

    // 查找第一行结束位置，添加边界检查
    const auto le = data.find("\r\n", pos);
    if (le == std::string_view::npos || le >= data.size() - 1)
        return false;
        
    // 解析状态行
    std::vector<std::string_view> parts;
    parts.reserve(8);
    
    for (std::size_t i = pos, s = pos; i <= le; ++i)
    {
        if (i == le || data[i] == ' ')
        { 
            if (i > s && parts.size() < 10)
                parts.push_back(data.substr(s, i - s));
            s = i + 1;
        }
    }
    
    // 严格检查parts数量
    if (parts.size() != 6)
        return false;

    if (!safe_parse(parts[0], temp_version) || !safe_parse(parts[1], temp_status_code))
        return false;
        
    // 验证状态码的合理性
    if (temp_status_code < 100 || temp_status_code > 599)
        return false;
        
    // 验证状态消息长度
    if (parts[2].empty() || parts[2].size() > 256)
        return false;
        
    temp_status_message = std::string(parts[2]);
    
    std::uint8_t ctype_val;
    if (!safe_parse(parts[3], ctype_val) || !safe_parse(parts[4], temp_checksum_value) ||
            !safe_parse(parts[5], temp_content_length))
        return false;
        
    // 验证枚举值的有效性（检查上下界）
    if (ctype_val < static_cast<std::uint8_t>(auxiliary::checksum_type::CRC32) ||
            ctype_val > static_cast<std::uint8_t>(auxiliary::checksum_type::SHA256))
        return false;
        
    temp_checksum_type = static_cast<auxiliary::checksum_type>(ctype_val);
    pos = le + 2;

    // 安全的trim函数，添加边界检查
    auto safe_trim = [](std::string_view &sv) -> bool
    {
        if (sv.size() > 8192)
            return false;
            
        const auto start = sv.find_first_not_of(" \t");
        if (start == std::string_view::npos)
        { 
            sv = "";
            return true;
        }
        const auto end = sv.find_last_not_of(" \t");
        sv = sv.substr(start, end - start + 1);
        return true;
    };

    // 解析头部字段，添加循环计数器防止无限循环
    std::size_t header_count = 0;
    const std::size_t max_headers = 100;
    
    while (pos < data.size() && header_count < max_headers)
    {
        const auto next_le = data.find("\r\n", pos);
        if (next_le == std::string_view::npos)
            break;
        
        // 防止行过长
        if (next_le - pos > 8192)
            return false;
            
        std::string_view line = data.substr(pos, next_le - pos);
        pos = next_le + 2;
        
        if (line.empty())
            break;

        const auto colon = line.find(':');
        if (colon != std::string_view::npos && colon > 0 && colon < line.size() - 1)
        {
            std::string_view k = line.substr(0, colon);
            std::string_view v = line.substr(colon + 1);
            
            if (!safe_trim(k) || !safe_trim(v))
                return false;
                
            // 验证键值对的有效性
            if (k.empty() || k.size() > 256 || v.size() > 8192)
                return false;

            std::string key(k), val(v);
            
            if (key == "Server")
            {
                if (val.size() > 512)
                    return false;
                temp_server = val;
            }
            else if (key == "Timestamp")
            {
                std::int64_t ts;
                if (safe_parse(v, ts))
                {
                    // 验证时间戳的合理性
                    if (ts < 0 || ts > std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch()).count() + 86400000)
                        return false;
                    temp_timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(ts));
                }
            }
            else
            {
                if (temp_headers.size() >= max_headers - 10)
                    return false;
                temp_headers[key] = val;
            }
        }
        ++header_count;
    }
    
    // 所有解析成功，提交更改（异常安全）
    _headers = std::move(temp_headers);
    _status_message = std::move(temp_status_message);
    _server = std::move(temp_server);
    _version = temp_version;
    _status_code = temp_status_code;
    _checksum_value = temp_checksum_value;
    _content_length = temp_content_length;
    _checksum_type = temp_checksum_type;
    _timestamp = temp_timestamp;
    
    return true;
}
