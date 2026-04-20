/**
 * @file Auxiliary.hpp
 * @brief 协议辅助套件定义
 * @details 提供协议类型、校验类型、协议头等辅助功能
 */

#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <boost/Json.hpp>
#include "./Json.hpp"
#include "../crypt/Encryption.hpp"



namespace protocol
{
    namespace auxiliary {}
} // end namespace protocol

namespace protocol::auxiliary
{
    /**
   * @brief 协议类型枚举
   * @details 定义支持的协议类型，可扩展
   */
    enum class protocol_type : std::uint8_t
    {
        JSON_RPC,      // `JSON-RPC`协议
        WEBSOCKET,     // `WebSocket`协议
        CUSTOM_TCP,    // 自定义`TCP`协议
        BINARY_STREAM, // 二进制流协议
        USER_DEFINED   // 用户自定义协议
    }; // end enum class protocol_type

    /**
   * @brief 数据完整性校验类型
   * @details 支持多种校验算法
   */
    enum class checksum_type : std::uint8_t
    {
        CRC32,
        MD5,
        SHA256,
        CUSTOM
    }; // end enum class checksum_type

    /**
   * @brief 协议头基类
   * @details 提供协议头的基础接口，支持自定义协议类型
   */
    class protocol_header
    {
    protected:
        std::uint32_t _version = 1;                               // 协议版本
        std::uint32_t _checksum_value = 0;                        // 校验值
        std::uint64_t _content_length = 0;                        // 内容长度
        checksum_type _checksum_type = checksum_type::CRC32;      // 校验类型
        std::unordered_map<std::string, std::string> _headers;    // 头部字段
        protocol_type _protocol_type = protocol_type::CUSTOM_TCP; // 协议类型

    protected:
        /**
     * @brief 计算数据校验值
     * @param data 待校验数据
     * @return 校验值
     */
        virtual std::uint32_t calculate_check_code(std::string_view data) const
        {
            switch (_checksum_type)
            {
            case checksum_type::CRC32:
                return encryption::CRC32(std::string(data), data.size());
            case checksum_type::MD5:
            {
                auto md5_hex = encryption::umbrage_hash::MD5(std::string(data));
                std::uint32_t result = 0;
                std::from_chars(md5_hex.data(), md5_hex.data() + 8, result, 16);
                return result;
            }
            case checksum_type::SHA256:
            {
                auto sha256_hex = encryption::umbrage_hash::SHA256(std::string(data));
                std::uint32_t result = 0;
                std::from_chars(sha256_hex.data(), sha256_hex.data() + 8, result, 16);
                return result;
            }
            default:
                return 0;
            }
        }

    public:
        virtual ~protocol_header() = default;
        protocol_header() { _headers.reserve(16); } // 预分配16个头部字段

        protocol_type get_protocol_type() const noexcept { return _protocol_type; }
        void set_protocol_type(protocol_type type) noexcept { _protocol_type = type; }

        checksum_type get_checksum_type() const noexcept { return _checksum_type; }
        void set_checksum_type(checksum_type type) noexcept { _checksum_type = type; }

        std::uint32_t get_checksum_value() const noexcept { return _checksum_value; }
        void set_checksum_value(std::uint32_t value) noexcept { _checksum_value = value; }

        std::uint64_t get_content_length() const noexcept { return _content_length; }
        void set_content_length(std::uint64_t length) noexcept { _content_length = length; }

        std::uint32_t get_version() const noexcept { return _version; }
        void set_version(std::uint32_t version) noexcept { _version = version; }

        // 清空所有头部字段
        void clear_headers() noexcept { _headers.clear(); }

        // 移除头部字段
        bool remove_header(const std::string &key) { return _headers.erase(key) > 0; }

        // 设置头部字段 `key` `value`
        void set_header(const std::string &key, const std::string &value) { _headers[key] = value; }

        //  获取所有头部字段
        const std::unordered_map<std::string, std::string> &get_headers() const noexcept { return _headers; }
        /**
     * @brief 获取头部字段
     * @param key 键
     * @return 值的可选对象
     */
        std::optional<std::string> get_header(const std::string &key) const
        {
            auto it = _headers.find(key);
            if (it != _headers.end())
                return it->second;
            return std::nullopt;
        }
        /**
     * @brief 序列化为字符串
     * @return 序列化后的字符串
     */
        virtual std::string to_string() const = 0;

        /**
     * @brief 从字符串反序列化
     * @param data 字符串数据
     * @return 是否成功
     */
        virtual bool from_string(std::string_view data) = 0;

        /**
     * @brief 计算并设置校验值
     * @param content 内容数据
     * @return 计算得到的校验值
     */
        virtual std::uint32_t calculate_and_set_checksum(std::string_view content)
        {
            _content_length = content.size();
            _checksum_value = calculate_check_code(content);
            return _checksum_value;
        }
        /**
     * @brief 验证数据完整性
     * @param content 内容数据
     * @return 验证是否通过
     */
        virtual bool verify_integrity(std::string_view content) const
        {
            if (content.size() != _content_length)
                return false;

            return calculate_check_code(content) == _checksum_value;
        }

     /**
     * @brief 转换为`JSON`
     * @return `JSON`对象
     */
        virtual protocol::json to_json() const
        {
            protocol::json json_object;
            json_object.set("protocol_type", static_cast<std::uint8_t>(_protocol_type));
            json_object.set("checksum_type", static_cast<std::uint8_t>(_checksum_type));
            json_object.set("checksum_value", _checksum_value);
            json_object.set("content_length", _content_length);
            json_object.set("version", _version);
            for (const auto &[key, value] : _headers)
            {
                json_object.set("header_" + key, value);
            }
            return json_object;
        }
        /**
     * @brief 从`JSON`反序列化
     * @param json_object `JSON`对象
     * @return 是否成功
     */
        virtual bool from_json(const protocol::json &json_object)
        {
            try
            {
                _version = json_object.get<std::uint32_t>("version", 1);
                _content_length = json_object.get<std::uint64_t>("content_length", 0);
                _checksum_value = json_object.get<std::uint32_t>("checksum_value", 0);
                _protocol_type = static_cast<protocol_type>(json_object.get<std::uint8_t>("protocol_type", 1));
                _checksum_type = static_cast<checksum_type>(json_object.get<std::uint8_t>("checksum_type", 1));
                _headers.clear();

                const auto &value = json_object.value();
                if (value.is_object())
                {
                    const auto &obj = value.as_object();
                    for (const auto &[key, val] : obj)
                    {
                        std::string_view key_view(key);
                        if (key_view.starts_with("header_") && val.is_string())
                        {
                            // 使用string_view避免不必要的字符串拷贝
                            std::string_view header_key_view = key_view.substr(7); // 移除"header_"前缀
                            _headers.emplace(std::string(header_key_view), std::string(val.as_string()));
                        }
                    }
                }
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
    }; // end class protocol_header
} // end namespace aux