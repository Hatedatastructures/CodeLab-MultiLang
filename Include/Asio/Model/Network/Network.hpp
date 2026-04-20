#pragma once

#include "./crypt/Encryption.hpp" // 加密 哈希

#include "./agreement/Http.hpp"       // http协议
#include "./agreement/Json.hpp"       // json协议
#include "./agreement/Auxiliary.hpp"  // tcp协议头基类
#include "./agreement/Protocol.hpp"   // tcp协议头和协议封装
#include "./agreement/Conversion.hpp" // tcp协议转换

#include "./session/Fundamental.hpp"  // 会话封装
#include "./session/Conversation.hpp" // 会话管理

#include "./business/Forwarder.hpp" // 服务端http / https 代理类

/**
 * @brief 网络模块
 * @note 该模块提供网络协议、加密、会话管理、转发劫持等功能
 */
namespace wan::network
{
    /**
   * @brief 协议模块
   * @note 提供`tcp`协议的定义、转换、校验等功能
   */
    namespace agreement
    {
        using protocol::json;
        using protocol::request;
        using protocol::request_header;
        using protocol::response;
        using protocol::response_header;

        using protocol::auxiliary::checksum_type;
        using protocol::auxiliary::protocol_header;
        using protocol::auxiliary::protocol_type;

        using protocol::conversion::protocol_converter;
    } // end namespace agreement
    /**
   * @brief http模块
   * @note 提供http协议的封装等功能
   */
    namespace http
    {
        using namespace protocol::http;
    } // end namespace http
    /**
   * @brief 加密模块
   * @note 提供加密、解密、哈希等功能
   */
    namespace ciphertext
    {
        using namespace encryption;
    } // end namespace ciphertext

    /**
   * @brief 会话模块
   */
    namespace session
    {
        using namespace conversation::fundamental;

        using conversation::connection_pool;
        using conversation::endpoint_config;

        using conversation::session_management;
        using conversation::session_management_config;
    } // end namespace session
    /**
   * @brief 代理模块
   * @note 提供业务逻辑的实现，如http代理等
   */
    namespace business
    {
        using namespace represents;
    } // end namespace business
} // end namespace wan
