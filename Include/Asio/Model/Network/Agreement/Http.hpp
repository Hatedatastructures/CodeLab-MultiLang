#pragma once
#include <boost/beast/Http.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <concepts>
#include <sstream>

namespace protocol
{
    namespace http
    {
        using field = boost::beast::http::field;

        // 按照 boost.beast 限制：body 必须满足 is_body，且可实例化为 request/response 的正文
        template <class underlying_structure>
        concept body_structure_constraint = boost::beast::http::is_body<underlying_structure>::value && requires 
        {
            typename boost::beast::http::request<underlying_structure>;
            typename boost::beast::http::response<underlying_structure>;
        };
        /**
     * @brief HTTP 请求类，封装了 boost.beast 的 request 类型
     * 
     * @tparam message_body 请求正文类型，必须满足 boost.beast::http::is_body 概念
     * @tparam fields_container 头部字段容器类型，默认使用 boost.beast::http::fields
     */
        template <body_structure_constraint message_body = boost::beast::http::string_body, class fields_container = boost::beast::http::fields>
        class request
        {
        private:
            boost::beast::http::request<message_body, fields_container> _req;

        public:
            using message_body_type = message_body;
            using fields_container_type = fields_container;
            using underlying_container = boost::beast::http::request<message_body, fields_container>;

            request() = default;
            request(boost::beast::http::verb m, unsigned v, boost::beast::string_view t)
                    : _req{m, v, t} {}

            underlying_container &base() { return _req; }
            underlying_container const &base() const { return _req; }

            /**
       * @brief 获取/设置 HTTP 请求方法
       * @return boost::beast::http::verb 当前请求方法
       */
            boost::beast::http::verb method() const { return _req.method(); }
            /**
       * @brief 设置 HTTP 请求方法
       * @param v 要设置的请求方法
       */
            void method(boost::beast::http::verb method_value) { _req.method(method_value); }
            /**
       * @brief 获取/设置 HTTP 请求方法的字符串表示
       * @return std::string 引用当前请求方法的字符串表示
       */
            boost::beast::string_view method_string() { return _req.method_string(); }
            /**
       * @brief 获取 HTTP 请求方法的字符串表示（只读）
       * @return boost::beast::string_view const& 当前请求方法的字符串表示
       */
            boost::beast::string_view method_string() const { return _req.method_string(); }
            /**
       * @brief 获取/设置 HTTP 请求目标路径
       * @return boost::beast::string_view 引用当前请求目标路径
       */
            boost::beast::string_view target() { return _req.target(); }
            /**
       * @brief 设置 HTTP 请求目标路径
       * @param t 要设置的请求目标路径
       */
            void target(boost::beast::string_view target_value) { _req.target(target_value); }
            /**
       * @brief 获取 HTTP 请求目标路径（只读）
       * @return boost::beast::string_view 当前请求目标路径
       */
            boost::beast::string_view target() const { return _req.target(); }
            /**
       * @brief 获取/设置 HTTP 请求版本
       * @return unsigned 当前请求版本
       */
            unsigned version() const { return _req.version(); }
            /**
       * @brief 设置 HTTP 请求版本
       * @param v 要设置的请求版本
       */
            void version(unsigned version_value) { _req.version(version_value); }

            /**
       * @brief 获取/设置 HTTP 请求是否保持连接
       * @return bool 当前是否保持连接
       */
            bool keep_alive() const { return _req.keep_alive(); }
            /**
       * @brief 设置 HTTP 请求是否保持连接
       * @param v 是否保持连接
       */
            void keep_alive(bool keep_alive_value) { _req.keep_alive(keep_alive_value); }

            /**
       * @brief 设置 HTTP 请求头部字段
       * @param boost::beast::http::field key 要设置的头部字段 
       * @param std::string value 要设置的字段值
       */
            void set(boost::beast::http::field key, std::string value) { _req.set(key, value); }
            /**
       * @brief 获取 HTTP 请求头部字段值（只读）
       * @param boost::beast::http::field key 要获取的头部字段
       * @return std::string 当前字段值
       */
            std::string at(boost::beast::http::field key) const { return _req.at(key); }
            /**
       * @brief 移除 HTTP 请求头部字段
       * @param boost::beast::http::field key 要移除的头部字段
       */
            void erase(boost::beast::http::field key) { _req.erase(key); }

            /**
       * @brief 获取/设置 HTTP 请求正文
       * @return typename message_body::value_type& 引用当前请求正文
       */
            typename message_body::value_type &body() { return _req.body(); }
            /**
       * @brief 获取 HTTP 请求正文（只读）
       * @return typename message_body::value_type const& 当前请求正文
       */
            typename message_body::value_type const &body() const { return _req.body(); }
            /**
       * @brief 获取/设置 HTTP 请求正文最大长度
       * @return std::uint64_t 当前最大长度
       */
            std::uint64_t body_limit() const { return _req.body_limit(); }
            /**
       * @brief 设置 HTTP 请求正文最大长度
       * @param n 要设置的最大长度
       */
            void body_limit(std::uint64_t len) { _req.body_limit(len); }
            /**
       * @brief 准备 HTTP 请求正文，设置 Content-Length 头部字段
       */
            void prepare_payload() { _req.prepare_payload(); }
            /**
       * @brief 检查 HTTP 请求是否包含 Content-Length 头部字段
       * @return bool 是否包含 Content-Length 头部字段
       */
            bool has_content_length() const { return _req.has_content_length(); }
            /**
       * @brief 获取 HTTP 请求正文长度
       * @return std::uint64_t 当前请求正文长度
       */
            std::uint64_t content_length() const { return _req.content_length(); }

            std::string to_string() const
            {
                std::ostringstream os;
                os << _req;
                return os.str();
            }
            bool from_string(std::string_view sv)
            {
                boost::beast::error_code ec;
                boost::beast::http::request_parser<message_body> parser;
                parser.eager(true);
                parser.body_limit(64 * 1024 * 1024);
                parser.put(boost::asio::buffer(sv.data(), sv.size()), ec);
                if (ec)
                    return false;
                _req = parser.get();
                return true;
            }
        }; // end class request
        /**
     * @brief HTTP 响应类
     * @details 用于表示 HTTP 响应，包含状态码、版本、头部字段和正文等信息
     * @tparam message_body 响应正文类型，默认使用 `boost::beast::http::string_body`
     * @tparam fields_container 响应头部字段容器类型，默认使用 `boost::beast::http::fields`
     */
        template <body_structure_constraint message_body = boost::beast::http::string_body, class fields_container = boost::beast::http::fields>
        class response
        {
        private:
            boost::beast::http::response<message_body, fields_container> _res;

        public:
            using underlying_container = boost::beast::http::response<message_body, fields_container>;
            using message_body_type = message_body;
            using fields_container_type = fields_container;

            response() = default;
            response(boost::beast::http::status s, unsigned v) : _res{s, v} {}

            /**
       * @brief 获取/设置 HTTP 响应底层容器
       * @return underlying_container& 引用当前响应底层容器
       */
            underlying_container &base() { return _res; }
            /**
       * @brief 获取 HTTP 响应底层容器（只读）
       * @return underlying_container const& 当前响应底层容器
       */
            underlying_container const &base() const { return _res; }

            /**
       * @brief 获取/设置 HTTP 响应状态码
       * @return boost::beast::http::status 当前状态码
       */
            boost::beast::http::status result() const { return _res.result(); }
            /**
       * @brief 设置 HTTP 响应状态码
       * @param s 要设置的状态码
       */
            void result(boost::beast::http::status status_code) { _res.result(status_code); }
            /**
       * @brief 获取 HTTP 响应状态码整数表示
       * @return unsigned 当前状态码整数表示
       */
            unsigned result_int() const { return _res.result_int(); }
            /**
       * @brief 获取/设置 HTTP 响应原因短语
       * @return std::string& 引用当前原因短语
       */
            std::string &reason() { return _res.reason(); }
            /**
       * @brief 获取 HTTP 响应原因短语（只读）
       * @return std::string const& 当前原因短语
       */
            std::string const &reason() const { return _res.reason(); }
            /**
       * @brief 获取/设置 HTTP 响应原因短语
       * @param s 要设置的原因短语
       */
            void reason(boost::beast::string_view s) { _res.reason(s); }

            /**
       * @brief 获取/设置 HTTP 响应版本
       * @return unsigned 当前版本
       */
            unsigned version() const { return _res.version(); }
            /**
       * @brief 设置 HTTP 响应版本
       * @param v 要设置的版本
       */
            void version(unsigned v) { _res.version(v); }
            /**
       * @brief 获取/设置 HTTP 响应长连接状态
       * @return bool 当前长连接状态
       */
            bool keep_alive() const { return _res.keep_alive(); }
            /**
       * @brief 设置 HTTP 响应长连接状态
       * @param v 要设置的长连接状态
       */
            void keep_alive(bool v) { _res.keep_alive(v); }

            /**
       * @brief 设置 HTTP 响应头部字段
       * @param key 要设置的头部字段
       * @param value 要设置的头部字段值
       */
            void set(boost::beast::http::field key, boost::beast::string_view value) { _res.set(key, value); }
            /**
       * @brief 获取 HTTP 响应头部字段值
       * @param key 要获取的头部字段
       * @return boost::beast::string_view 当前头部字段值
       */
            boost::beast::string_view at(boost::beast::http::field key) const { return _res.at(key); }
            /**
       * @brief 移除 HTTP 响应头部字段
       * @param key 要移除的头部字段
       */
            void erase(boost::beast::http::field key) { _res.erase(key); }

            /**
       * @brief 获取/设置 HTTP 响应正文
       * @return typename message_body::value_type& 引用当前正文
       */
            typename message_body::value_type &body() { return _res.body(); }
            /**
       * @brief 获取 HTTP 响应正文（只读）
       * @return typename message_body::value_type const& 当前正文
       */
            typename message_body::value_type const &body() const { return _res.body(); }
            /**
       * @brief 获取/设置 HTTP 响应正文最大长度
       * @return std::uint64_t 当前最大长度
       */
            std::uint64_t body_limit() const { return _res.body_limit(); }
            /**
       * @brief 设置 HTTP 响应正文最大长度
       * @param n 要设置的最大长度
       */
            void body_limit(std::uint64_t n) { _res.body_limit(n); }
            /**
       * @brief 准备 HTTP 响应正文负载
       * @details 计算并设置 Content-Length 头部字段
       */
            void prepare_payload() { _res.prepare_payload(); }
            /**
       * @brief 检查 HTTP 响应是否包含 Content-Length 头部字段
       * @return bool 是否包含 Content-Length 头部字段
       */
            bool has_content_length() const { return _res.has_content_length(); }
            /**
       * @brief 获取 HTTP 响应 Content-Length 头部字段值
       * @return std::uint64_t 当前 Content-Length 值
       */
            std::uint64_t content_length() const { return _res.content_length(); }

            
            std::string to_string() const
            {
                std::ostringstream os;
                os << _res;
                return os.str();
            }
            bool from_string(std::string_view sv)
            {
                boost::beast::error_code ec;
                boost::beast::http::response_parser<message_body> parser;
                parser.eager(true);
                parser.body_limit(64 * 1024 * 1024);
                parser.put(boost::asio::buffer(sv.data(), sv.size()), ec);
                if (ec)
                    return false;
                _res = parser.get();
                return true;
            }
        }; // end class response
    }
} // end namespace protocol