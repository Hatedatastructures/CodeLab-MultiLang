/**
 * @file Conversion.hpp
 * @brief 协议转换定义
 * @details 提供协议对象与`JSON`之间的互相转换
 */
#pragma once
#include <boost/Json.hpp>
#include "./Protocol.hpp"
#include "./Json.hpp"
#include <optional>

namespace protocol
{
    namespace conversion
    {
    } // end namespace conversion
} // end namespace protocol

namespace protocol::conversion
{
    /**
   * @brief 协议转换器
   * @details 提供协议对象与`JSON`之间的转换功能
   */
    class protocol_converter
    {
    public:
        /**
     * @brief 请求转`JSON`
     * @tparam header_t 头部类型
     * @param req 请求对象
     * @return `JSON`对象
     */
        template <header_constraint header_t = request_header>
        static json request_to_json(const request<header_t> &req)
        {
            return req.to_json();
        }
        /**
     * @brief `JSON`转请求
     * @tparam header_t 头部类型
     * @param json_object `JSON`对象
     * @return 请求对象
     */
        template <header_constraint header_t = request_header>
        static std::optional<request<header_t>> json_to_request(const json &json_object)
        {
            request<header_t> req;
            if (req.from_json(json_object))
                return req;
            return std::nullopt;
        }
        /**
     * @brief 响应转`JSON`
     * @tparam header_t 头部类型
     * @param resp 响应对象
     * @return `JSON`对象
     */
        template <header_constraint header_t = response_header>
        static json response_to_json(const response<header_t> &resp)
        {
            return resp.to_json();
        }
        /**
     * @brief `JSON`转响应
     * @tparam header_t 头部类型
     * @param json_object `JSON`对象
     * @return 响应对象
     */
        template <header_constraint header_t = response_header>
        static std::optional<response<header_t>> json_to_response(const json &json_object)
        {
            response<header_t> resp;
            if (resp.from_json(json_object))
                return resp;
            return std::nullopt;
        }
    }; // end class protocol_converter
} // end namespace conversion
