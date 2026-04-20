/**
 * @file Fundamental.hpp
 * @brief 会话辅助类定义
 * @details 提供会话管理、状态监控、错误处理等功能
 */
#pragma once
#include <memory>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <cstdlib>

#include "../agreement/Json.hpp"
#include "../agreement/Auxiliary.hpp"
#include "../agreement/Protocol.hpp"
#include "../agreement/Conversion.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/pool/object_pool.hpp>
#include <openssl/x509v3.h>

namespace conversation
{
    namespace fundamental
    {
    } // end namespace fundamental
} // end namespace conversation

namespace conversation::fundamental
{

    using request = protocol::request<protocol::request_header>;
    using response = protocol::response<protocol::response_header>;
    /**
     * @brief 会话状态枚举
     */
    enum class session_state : std::uint8_t
    {
        DISCONNECTED,  // 未连接
        CONNECTING,	   // 连接中
        CONNECTED,	   // 已连接
        DISCONNECTING, // 断开连接中
        ERROR_STATE	   // 错误状态
    };
    /**
     * @brief 会话类型枚举
     */
    enum class session_type : std::uint8_t
    {
        TCP_CLIENT, // TCP客户端
        TCP_SERVER, // TCP服务端
        UDP_CLIENT, // UDP客户端
        UDP_SERVER, // UDP服务端
        SSL_CLIENT, // SSL客户端
        SSL_SERVER	// SSL服务端
    };
    /**
     * @brief 会话事件类型
     */
    enum class session_event : std::uint8_t
    {
        CONNECTED = 0,	// 连接建立
        DISCONNECTED,	// 连接断开
        DATA_RECEIVED,	// 数据接收
        DATA_SENT,		// 数据发送
        ERROR_OCCURRED, // 错误发生
        TIMEOUT			// 超时
    };
    /**
     * @brief 会话统计信息
     */
    struct session_statistics
    {
        std::atomic<std::uint64_t> _bytes_sent{0};			  // 发送字节数
        std::atomic<std::uint64_t> _bytes_received{0};		  // 接收字节数
        std::atomic<std::uint64_t> _messages_sent{0};		  // 发送消息数
        std::atomic<std::uint64_t> _messages_received{0};	  // 接收消息数
        std::chrono::system_clock::time_point _created_time;  // 创建时间
        std::chrono::system_clock::time_point _last_activity; // 最后活动时间

        session_statistics() : _created_time(std::chrono::system_clock::now()),
                               _last_activity(std::chrono::system_clock::now()) {}

        /**
         * @brief 更新最后活动时间
         */
        void renewal_activity() noexcept
        {
            _last_activity = std::chrono::system_clock::now();
        }

        /**
         * @brief 获取会话持续时间（秒）
         * @return 持续时间
         */
        std::chrono::seconds get_duration() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _created_time);
        }

        /**
         * @brief 获取空闲时间（秒）
         * @return 空闲时间
         */
        std::chrono::seconds get_idle_time() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _last_activity);
        }
    };

    /**
     * @brief 会话配置
     */
    struct session_config
    {
        std::chrono::milliseconds _read_timeout{30000};	   // 读取超时
        std::chrono::milliseconds _write_timeout{30000};   // 写入超时
        std::chrono::milliseconds _connect_timeout{30000}; // 连接超时

        std::chrono::milliseconds _heartbeat_interval{600000}; // 心跳间隔，默认 10min

        bool _enable_heartbeat{true};		 // 启用心跳
        bool _enable_ssl{false};			 // 启用SSL
        bool _enable_async_processing{true}; // 启用异步处理

        std::string _ssl_cert_file;			   // SSL证书文件
        std::string _ssl_key_file;			   // SSL私钥文件
        std::string _ssl_ca_file;			   // CA证书文件（仅此处加载）
        std::string _tls_server_name;		   // SNI与主机名验证的服务器名
        bool _ssl_insecure_skip_verify{false}; // 跳过证书校验（开发/测试用）

        std::size_t _max_buffer_size{65536};	// 最大缓冲区大小
        std::size_t _max_message_size{1048576}; // 最大消息大小
    };

    /**
     * @brief 协议约束概念
     * @details 定义协议类的约束条件，包括to_string和from_string方法
     */
    template <class protocol_t>
    concept stringable_constraints = requires(protocol_t p, std::string_view str_value) {
        { p.to_string() } -> std::convertible_to<std::string>;
        { p.from_string(str_value) } -> std::same_as<bool>;
    };
    /**
     * @class session
     * @brief 通用会话管理类（支持 `TCP`/`SSL`，覆盖客户端与服务端场景）
     * @tparam request_t 请求协议类型，需满足 stringable_constraints 约束
     * @tparam response_t 响应协议类型，需满足 stringable_constraints 约束
     *
     * @note
     * - 支持同步/异步发送原始字节，以及基于请求/响应的封装接口,客户端用 `async_connect(host, port, ...)` 建立连接；
     *  服务端可用已 `accept()` 的 `socket` 构造，直接进入已连接状态。
     *
     * @warning
     * - `start()` 仅在“已连接”状态下生效；请先调用 `async_connect(...)`（客户端）或使用已连接 `socket `构造（服务端）。
     *
     * - 请用 `std::shared_ptr` 管理会话对象；内部使用 `shared_from_this()` 保障异步期间的生命周期。
     */
    template <stringable_constraints request_t = request, stringable_constraints response_t = response>
    class session : public std::enable_shared_from_this<session<request_t, response_t>>
    {
    public:
        using session_ptr = std::shared_ptr<session<request_t, response_t>>; // 会话指针类型
        using reception_processing = std::function<void(session_ptr, std::string_view)>;

    private:
        boost::asio::io_context &_io_context; // 引用IO上下文

        boost::asio::steady_timer _timer; // 定时器

        boost::asio::ip::tcp::socket _socket;												 // TCP套接字
        std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _ssl_socket; // SSL套接字
        std::unique_ptr<boost::asio::ssl::context> _ssl_context;							 // SSL上下文（保持生命周期）

        session_type _type;								   // 会话类型
        session_config _config;							   // 会话配置
        session_statistics _statistics;					   // 会话统计信息
        session_state _state{session_state::DISCONNECTED}; // 会话状态

        std::string _session_id;	   // 会话ID
        std::string _remote_address;   // 远程地址
        std::uint16_t _remote_port{0}; // 远程端口

        mutable std::shared_mutex _state_mutex; // 共享互斥锁

        std::string _received_data;	   // 读取缓冲区
        reception_processing _on_data; // 读取数据回调（字节视图）
    private:
        /**
         * @brief 生成唯一会话`ID`
         * @return 会话`ID`
         * @note 基于`64`字节随机数据混入后的`SHA256`哈希
         */
        std::string _generate_session_id()
        {
            return encryption::umbrage_hash::SHA256(encryption::mix64());
        }
        /**
         * @brief 创建SSL上下文
         * @return SSL上下文
         * @note 基于会话配置初始化SSL上下文，包括证书验证、密码套件等
         */
        void _init_ssl_context(boost::asio::ssl::context &ssl_context)
        {
            try
            {
                ssl_context.set_options(
                    boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::no_sslv3 |
                    boost::asio::ssl::context::single_dh_use);
                if (_type == session_type::SSL_SERVER)
                {
                    if (!_config._ssl_cert_file.empty())
                        ssl_context.use_certificate_chain_file(_config._ssl_cert_file);
                    if (!_config._ssl_key_file.empty())
                        ssl_context.use_private_key_file(_config._ssl_key_file, boost::asio::ssl::context::pem);
                }
                else if (_type == session_type::SSL_CLIENT)
                {
                    if (_config._ssl_insecure_skip_verify)
                        ssl_context.set_verify_mode(boost::asio::ssl::verify_none);
                    else
                        ssl_context.set_verify_mode(boost::asio::ssl::verify_peer);

                    // 仅从配置的 CA 路径加载（不使用默认/环境变量/其他路径）
                    if (!_config._ssl_insecure_skip_verify && !_config._ssl_ca_file.empty())
                    {
                        try
                        {
                            ssl_context.load_verify_file(_config._ssl_ca_file);
                        }
                        catch (...)
                        {
                        }
                    }

                    // 可选客户端证书
                    if (!_config._ssl_cert_file.empty())
                        ssl_context.use_certificate_chain_file(_config._ssl_cert_file);
                    if (!_config._ssl_key_file.empty())
                        ssl_context.use_private_key_file(_config._ssl_key_file, boost::asio::ssl::context::pem);
                }
            }
            catch (const std::exception &e)
            {
                // SSL 上下文初始化失败时保持静默，交由调用方在连接阶段感知错误
            }
        }
        /**
         * @brief 设置会话状态
         * @param state 新状态
         */
        void _set_state(session_state state)
        {
            std::lock_guard<std::shared_mutex> lock(_state_mutex);
            _state = state;
        }
        /**
         * @brief 启动读取数据操作
         */
        void _start_read()
        {
            if (_state != session_state::CONNECTED)
                return;
            _received_data.resize(static_cast<size_t>(_config._max_buffer_size));
            auto self = this->shared_from_this();
            if (_config._enable_ssl && _ssl_socket)
            {
                auto ssl_function = [self](const boost::system::error_code &ec, std::uint64_t bytes_transferred)
                {
                    self->_handle_read(ec, bytes_transferred);
                };
                _ssl_socket->async_read_some(boost::asio::buffer(_received_data), ssl_function);
            }
            else
            {
                auto tcp_function = [self](const boost::system::error_code &ec, std::uint64_t bytes_transferred)
                {
                    self->_handle_read(ec, bytes_transferred);
                };
                _socket.async_read_some(boost::asio::buffer(_received_data), tcp_function);
            }
        }
        /**
         * @brief 处理读取数据完成
         * @param ec 错误码
         * @param bytes_transferred 传输字节数
         */
        void _handle_read(const boost::system::error_code &ec, std::uint64_t bytes_transferred)
        {
            if (ec)
            {
                _handle_error(ec);
                return;
            }

            _statistics._bytes_received += bytes_transferred;
            _statistics._messages_received++;
            _statistics.renewal_activity();
            // 将原始字节视图交给读取回调，由外部进行协议解析与处理
            if (_on_data && bytes_transferred > 0)
            {
                std::string_view view(_received_data.data(), static_cast<size_t>(bytes_transferred));
                _on_data(this->shared_from_this(), view);
            }

            // 循环调用
            _start_read();
        }
        /**
         * @brief 处理协议数据
         * @param data 数据
         */
        // 协议解析与请求处理已移除，外部通过读取回调自行处理

        /**
         * @brief 处理错误
         * @param ec 错误码
         * @warning 该函数会关闭会话
         */
        void _handle_error(const boost::system::error_code &ec)
        {
            (void)ec;
            if (_state == session_state::DISCONNECTED || _state == session_state::DISCONNECTING)
                return;

            _set_state(session_state::ERROR_STATE);
            close();
        }
        /**
         * @brief 启动心跳定时器
         * @details 心跳定时器用于检测会话是否超时，超时后会触发错误处理
         */
        void _start_heartbeat_timer()
        {
            if (!_config._enable_heartbeat)
                return;

            _timer.expires_after(_config._heartbeat_interval);
            auto self = this->shared_from_this();
            auto timer_function = [self](const boost::system::error_code &ec)
            {
                if (!ec)
                    self->_handle_heartbeat();
            };
            _timer.async_wait(timer_function);
        }
        /**
         * @brief 处理心跳
         * @details 检查会话是否超时，超时后会触发错误处理
         */
        void _handle_heartbeat()
        {
            if (_state != session_state::CONNECTED)
                return;

            // 检查空闲时间
            auto idle_time = _statistics.get_idle_time();
            if (idle_time > _config._heartbeat_interval * 2)
            {
                // 超时，关闭连接
                close();
                return;
            }
            _start_heartbeat_timer(); // 继续心跳
        }

    public:
        session(boost::asio::io_context &io_context, session_type type = session_type::TCP_CLIENT,
                const session_config &config = session_config{})
            : _io_context(io_context), _timer(io_context), _socket(io_context), _type(type), _config(config),
              _session_id(_generate_session_id())
        {
            if (_config._enable_ssl)
            {
                _ssl_context = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                _init_ssl_context(*_ssl_context);
                // 使用已创建的 TCP 套接字构造 SSL 流（保持上下文生命周期）
                _ssl_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(std::move(_socket), *_ssl_context);
            }
        }
        /**
         * @brief 基于远程地址与端口的便捷构造（客户端）
         * @param io_context IO 上下文
         * @param host 远程主机地址
         * @param port 远程端口
         * @param type 会话类型（默认 TCP_CLIENT）
         * @param config 会话配置
         * @details
         *   - 仅初始化远端信息与会话配置，不会发起连接；
         *   - 适合“先构造、后在适当时机连接”的使用场景；
         *   - 若启用 `SSL`（`config._enable_ssl=true`），会同步初始化 SSL 上下文与流。
         * @note
         *   - 请使用 `async_connect(host, port)` 或同步连接流程启动连接；
         *   - 构造函数内不会调用 `shared_from_this()`，避免未由 `shared_ptr` 管理时的未定义行为。
         * @warning
         *   - 构造后尚未连接，发送接口会返回 `not_connected`；
         *   - 连接成功后，统计信息与远端地址会在握手完成时更新。
         */
        session(boost::asio::io_context &io_context, const std::string &host, std::uint16_t port,
                session_type type = session_type::TCP_CLIENT, const session_config &config = session_config{})
            : _io_context(io_context), _timer(io_context), _socket(io_context), _type(type), _config(config),
              _session_id(_generate_session_id()), _remote_address(host), _remote_port(port)
        {
            if (_config._enable_ssl)
            {
                _ssl_context = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                _init_ssl_context(*_ssl_context);
                // 使用已创建的 TCP 套接字构造 SSL 流（保持上下文生命周期）
                _ssl_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(std::move(_socket), *_ssl_context);
            }
        }
        /**
         * @brief 服务端的快捷初始化方式
         */
        session(boost::asio::ip::tcp::socket &&socket, session_type type = session_type::TCP_SERVER,
                const session_config &config = session_config{})
            : _io_context(static_cast<boost::asio::io_context &>(socket.get_executor().context())), _timer(_io_context),
              _socket(std::move(socket)), _type(type), _config(config), _session_id(_generate_session_id())
        {
            if (_socket.is_open())
            {
                boost::system::error_code ep_ec;
                auto ep = _socket.remote_endpoint(ep_ec);
                if (!ep_ec)
                {
                    _remote_address = ep.address().to_string();
                    _remote_port = ep.port();
                    _set_state(session_state::CONNECTED);
                }
                else
                {
                    _set_state(session_state::DISCONNECTED);
                }
            }

            if (_config._enable_ssl)
            {
                _ssl_context = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                _init_ssl_context(*_ssl_context);
                _ssl_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(std::move(_socket), *_ssl_context);
            }
        }
        ~session()
        {
            close();
        }
        session(const session &) = delete;
        session &operator=(const session &) = delete;
        /**
         * @brief 设置接受数据操作
         * @warning 设置该函数需要在会话未连接状态下调用，并且需要自动按照既定状态处理接受的数据
         * @param handler 读取到字节视图时调用，外部负责协议解析
         * @note 类型需要符合`std::function<void(session_ptr, std::string_view)>`
         */
        void set_reception_processing(reception_processing handler)
        {
            _on_data = std::move(handler);
        }
        /**
         * @brief 获取会话`ID`
         * @return 会话`ID`
         */
        const std::string &get_session_id() const
        {
            return _session_id;
        }
        /**
         * @brief 获取会话状态
         * @return 会话状态
         */
        session_state get_state() const
        {
            std::lock_guard<std::shared_mutex> lock(_state_mutex);
            return _state;
        }
        /**
         * @brief 获取会话类型
         * @return 会话类型
         */
        session_type get_type() const
        {
            return _type;
        }
        /**
         * @brief 获取远程地址
         * @return 远程地址
         */
        std::string get_remote_address() const
        {
            return _remote_address;
        }
        /**
         * @brief 获取远程端口
         * @return 远程端口
         */
        std::uint16_t get_remote_port() const
        {
            return _remote_port;
        }
        /**
         * @brief 获取统计信息
         * @return 统计信息
         * @note 统计信息为只读，不能修改
         */
        const session_statistics &get_statistics() const noexcept
        {
            return _statistics;
        }
        /**
         * @brief 检查是否已连接
         * @return 是否已连接
         */
        bool is_connected() const noexcept
        {
            return get_state() == session_state::CONNECTED;
        }
        /**
         * @brief 异步连接到远程地址
         * @param host 主机地址
         * @param port 端口
         * @param callback 连接完成回调
         * @warning 回调参数须接受 `boost::system::error_code` 类型参数
         */
        void async_connect(const std::string &host, std::uint16_t port,
                           std::function<void(const boost::system::error_code &)> callback = nullptr)
        {
            if (_state != session_state::DISCONNECTED)
            {
                if (callback)
                    callback(boost::asio::error::already_connected);
                return;
            }
            _set_state(session_state::CONNECTING);
            _remote_address = host;
            _remote_port = port;

            boost::asio::ip::tcp::resolver resolver(_io_context);
            auto self = this->shared_from_this();

            auto ssl_connect = [self, callback](const boost::system::error_code &ec, const boost::asio::ip::tcp::endpoint &)
            {
                if (ec)
                {
                    self->_set_state(session_state::DISCONNECTED);
                    if (callback)
                        callback(ec);
                    return;
                }
                auto ssl_handshake = [self, callback](const boost::system::error_code &handshake_ec)
                {
                    if (handshake_ec)
                    {
                        self->_set_state(session_state::DISCONNECTED);
                        if (callback)
                            callback(handshake_ec);
                        return;
                    }
                    self->_set_state(session_state::CONNECTED);
                    self->_start_read();			// 启动异步读取
                    self->_start_heartbeat_timer(); // 启动心跳定时器
                    if (callback)
                        callback(boost::system::error_code());
                };
                if (!self->_config._tls_server_name.empty())
                {
                    SSL_set_tlsext_host_name(self->_ssl_socket->native_handle(), self->_config._tls_server_name.c_str());
                    self->_ssl_socket->set_verify_callback(
                        [server_name = self->_config._tls_server_name](bool preverified, boost::asio::ssl::verify_context &ctx)
                        {
                            if (!preverified)
                                return false;
                            auto *store_ctx = ctx.native_handle();
                            X509 *cert = store_ctx ? X509_STORE_CTX_get_current_cert(store_ctx) : nullptr;
                            if (!cert)
                                return false;
                            return X509_check_host(cert, server_name.c_str(), 0, 0, nullptr) == 1;
                        });
                }
                self->_ssl_socket->async_handshake(boost::asio::ssl::stream_base::client, ssl_handshake);
            };

            auto tcp_connect = [self, callback](const boost::system::error_code &ec, const boost::asio::ip::tcp::endpoint &)
            {
                if (ec)
                {
                    self->_set_state(session_state::DISCONNECTED);
                    if (callback)
                        callback(ec);
                    return;
                }
                self->_set_state(session_state::CONNECTED);
                self->_start_read();			// 启动异步读取
                self->_start_heartbeat_timer(); // 启动心跳定时器
                if (callback)
                    callback(boost::system::error_code());
            };

            // 若 host 为纯 IP，优先使用直连以规避解析差异
            {
                boost::system::error_code addr_ec;
                auto addr = boost::asio::ip::make_address(host, addr_ec);
                if (!addr_ec)
                {
                    boost::asio::ip::tcp::endpoint endpoint(addr, port);
                    if (self->_config._enable_ssl && self->_ssl_socket)
                    {
                        // 直连 SSL：签名仅含 error_code，需要单独定义回调
                        auto ssl_connect_direct = [self, callback](const boost::system::error_code &ec)
                        {
                            if (ec)
                            {
                                self->_set_state(session_state::DISCONNECTED);
                                if (callback)
                                    callback(ec);
                                return;
                            }
                            auto ssl_handshake = [self, callback](const boost::system::error_code &handshake_ec)
                            {
                                if (handshake_ec)
                                {
                                    self->_set_state(session_state::DISCONNECTED);
                                    if (callback)
                                        callback(handshake_ec);
                                    return;
                                }
                                self->_set_state(session_state::CONNECTED);
                                self->_start_read();
                                self->_start_heartbeat_timer();
                                if (callback)
                                    callback(boost::system::error_code());
                            };
                            if (!self->_config._tls_server_name.empty())
                            {
                                SSL_set_tlsext_host_name(self->_ssl_socket->native_handle(), self->_config._tls_server_name.c_str());
                                self->_ssl_socket->set_verify_callback(
                                    [server_name = self->_config._tls_server_name](bool preverified, boost::asio::ssl::verify_context &ctx)
                                    {
                                        if (!preverified)
                                            return false;
                                        auto *store_ctx = ctx.native_handle();
                                        X509 *cert = store_ctx ? X509_STORE_CTX_get_current_cert(store_ctx) : nullptr;
                                        if (!cert)
                                            return false;
                                        return X509_check_host(cert, server_name.c_str(), 0, 0, nullptr) == 1;
                                    });
                            }
                            self->_ssl_socket->async_handshake(boost::asio::ssl::stream_base::client, ssl_handshake);
                        };
                        self->_ssl_socket->lowest_layer().async_connect(endpoint, ssl_connect_direct);
                    }
                    else
                    {
                        // 直连 TCP：仅含 error_code，需要单独定义回调
                        auto tcp_connect_direct = [self, callback](const boost::system::error_code &ec)
                        {
                            if (ec)
                            {
                                self->_set_state(session_state::DISCONNECTED);
                                if (callback)
                                    callback(ec);
                                return;
                            }
                            self->_set_state(session_state::CONNECTED);
                            self->_start_read();
                            self->_start_heartbeat_timer();
                            if (callback)
                                callback(boost::system::error_code());
                        };
                        self->_socket.async_connect(endpoint, tcp_connect_direct);
                    }
                    return; // 已发起直连，无需解析
                }
            }

            auto asynchronous_function = [self, callback, tcp_connect, ssl_connect](const boost::system::error_code &ec,
                                                                                    boost::asio::ip::tcp::resolver::results_type results)
            {
                if (ec)
                {
                    self->_set_state(session_state::DISCONNECTED);
                    if (callback)
                        callback(ec);
                    return;
                }
                if (self->_config._enable_ssl && self->_ssl_socket)
                    boost::asio::async_connect(self->_ssl_socket->lowest_layer(), results, ssl_connect);
                else
                    boost::asio::async_connect(self->_socket, results, tcp_connect);
            };

            resolver.async_resolve(host, std::to_string(port), asynchronous_function);
        }
        /**
         * @brief 同步连接到远程地址
         * @param host 主机地址
         * @param port 端口
         * @return 错误码（成功为 0）
         * @details 与异步连接保持一致的握手与状态更新语义；成功后立即启动读取与心跳
         */
        boost::system::error_code connect(const std::string &host, std::uint16_t port)
        {
            if (_state != session_state::DISCONNECTED)
                return boost::asio::error::already_connected;

            _set_state(session_state::CONNECTING);
            _remote_address = host;
            _remote_port = port;

            boost::system::error_code ec;
            // 优先尝试纯 IP 直连
            {
                boost::system::error_code addr_ec;
                auto addr = boost::asio::ip::make_address(host, addr_ec);
                if (!addr_ec)
                {
                    boost::asio::ip::tcp::endpoint endpoint(addr, port);
                    if (_config._enable_ssl && _ssl_socket)
                        _ssl_socket->lowest_layer().connect(endpoint, ec);
                    else
                        _socket.connect(endpoint, ec); // 拿远程ip和端口来连接，连接成功后socket 就可以发送数据
                }
                else
                {
                    boost::asio::ip::tcp::resolver resolver(_io_context); // 解析并连接
                    auto results = resolver.resolve(host, std::to_string(port), ec);
                    if (ec)
                    {
                        _set_state(session_state::DISCONNECTED);
                        return ec;
                    }
                    if (_config._enable_ssl && _ssl_socket)
                        boost::asio::connect(_ssl_socket->lowest_layer(), results, ec);
                    else
                        boost::asio::connect(_socket, results, ec);
                }
            }

            if (ec)
            {
                _set_state(session_state::DISCONNECTED);
                return ec;
            }

            if (_config._enable_ssl && _ssl_socket)
            {
                if (!_config._tls_server_name.empty())
                {
                    SSL_set_tlsext_host_name(_ssl_socket->native_handle(), _config._tls_server_name.c_str());
                    _ssl_socket->set_verify_callback(
                        [server_name = _config._tls_server_name](bool preverified, boost::asio::ssl::verify_context &ctx)
                        {
                            if (!preverified)
                                return false;
                            auto *store_ctx = ctx.native_handle();
                            X509 *cert = store_ctx ? X509_STORE_CTX_get_current_cert(store_ctx) : nullptr;
                            if (!cert)
                                return false;
                            return X509_check_host(cert, server_name.c_str(), 0, 0, nullptr) == 1;
                        });
                }
                _ssl_socket->handshake(boost::asio::ssl::stream_base::client, ec);
                if (ec)
                {
                    _set_state(session_state::DISCONNECTED);
                    return ec;
                }
            }

            _set_state(session_state::CONNECTED);
            {
                boost::system::error_code ep_ec;
                auto ep = (_config._enable_ssl && _ssl_socket)
                              ? _ssl_socket->lowest_layer().remote_endpoint(ep_ec)
                              : _socket.remote_endpoint(ep_ec);
                if (!ep_ec)
                {
                    _remote_address = ep.address().to_string();
                    _remote_port = ep.port();
                }
            }

            _start_read();
            _start_heartbeat_timer();
            return ec;
        }
        /**
         * @brief 在断开状态下接管外部 socket（用于与连接池协作）
         * @param socket 外部已打开的 socket（需与当前 io_context 同源）
         * @param type 接管后的会话类型（默认服务端）
         * @return 是否接管成功
         * @note SSL 客户端将同步握手；SSL 服务端握手在 start() 进行
         */
        bool adopt_socket(boost::asio::ip::tcp::socket &&socket, session_type type = session_type::TCP_SERVER)
        {
            if (_state != session_state::DISCONNECTED)
                return false;
            // 必须同源 io_context
            auto &ctx = static_cast<boost::asio::io_context &>(socket.get_executor().context());
            if (&ctx != &_io_context)
                return false;

            if ((type == session_type::SSL_CLIENT || type == session_type::SSL_SERVER) && !_config._enable_ssl)
                return false;

            _type = type;
            _socket = std::move(socket);

            {
                boost::system::error_code ep_ec;
                auto ep = _socket.remote_endpoint(ep_ec);
                if (!ep_ec)
                {
                    _remote_address = ep.address().to_string();
                    _remote_port = ep.port();
                }
            }

            if (_config._enable_ssl)
            {
                if (!_ssl_context)
                {
                    _ssl_context = std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
                    _init_ssl_context(*_ssl_context);
                }
                _ssl_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(std::move(_socket), *_ssl_context);

                if (_type == session_type::SSL_CLIENT)
                {
                    boost::system::error_code hs_ec;
                    if (!_config._tls_server_name.empty())
                    {
                        SSL_set_tlsext_host_name(_ssl_socket->native_handle(), _config._tls_server_name.c_str());
                        _ssl_socket->set_verify_callback(
                            [server_name = _config._tls_server_name](bool preverified, boost::asio::ssl::verify_context &ctx)
                            {
                                if (!preverified)
                                    return false;
                                auto *store_ctx = ctx.native_handle();
                                X509 *cert = store_ctx ? X509_STORE_CTX_get_current_cert(store_ctx) : nullptr;
                                if (!cert)
                                    return false;
                                return X509_check_host(cert, server_name.c_str(), 0, 0, nullptr) == 1;
                            });
                    }
                    _ssl_socket->handshake(boost::asio::ssl::stream_base::client, hs_ec);
                    if (hs_ec)
                    {
                        _set_state(session_state::DISCONNECTED);
                        return false;
                    }
                }
            }

            _set_state(session_state::CONNECTED);
            return true;
        }
        /**
         * @brief 启动会话
         * @warning 仅在“已连接”状态下启动读取与心跳
         * @note 不负责建立连接；请先调用 `async_connect(host, port, ...)`或`adopt_socket(socket, type)`
         */
        void start()
        {
            if (_state == session_state::CONNECTED)
            {
                if (_config._enable_ssl && _ssl_socket && _type == session_type::SSL_SERVER)
                {
                    auto self = this->shared_from_this();
                    auto ssl_handshake = [self](const boost::system::error_code &handshake_ec)
                    {
                        if (handshake_ec)
                        {
                            self->_set_state(session_state::DISCONNECTED);
                            return;
                        }
                        self->_start_read();			// 启动异步读取
                        self->_start_heartbeat_timer(); // 启动心跳定时器
                    };
                    _ssl_socket->async_handshake(boost::asio::ssl::stream_base::server, ssl_handshake);
                }
                else
                {
                    _start_read();			  // 启动异步读取
                    _start_heartbeat_timer(); // 启动心跳定时器
                }
            }
        }
        /**
         * @brief 同步发送字符串
         * @param data 字符串视图
         * @return 发送结果错误码（成功为 0）
         */
        boost::system::error_code send_bytes(std::string_view data)
        {
            if (_state != session_state::CONNECTED)
                return boost::asio::error::not_connected;

            boost::system::error_code ec;
            std::size_t bytes_transferred = 0;
            if (_config._enable_ssl && _ssl_socket)
                bytes_transferred = boost::asio::write(*_ssl_socket, boost::asio::buffer(data), ec);
            else
                bytes_transferred = boost::asio::write(_socket, boost::asio::buffer(data), ec);

            if (!ec)
            {
                _statistics._bytes_sent += bytes_transferred;
                _statistics._messages_sent++;
                _statistics.renewal_activity();
            }
            else
                _handle_error(ec);
            return ec;
        }
        /**
         * @brief 异步发送请求
         * @param request 请求
         * @param callback 发送完成回调
         */
        void async_send_request(const request_t &request, std::function<void(const boost::system::error_code &)> callback = nullptr)
        {
            if (_state != session_state::CONNECTED)
            {
                if (callback)
                    callback(boost::asio::error::not_connected);
                return;
            }
            try
            {
                std::string data = request.to_string();
                async_send_bytes(data, callback);
            }
            catch (const std::exception &)
            {
                if (callback)
                    callback(boost::asio::error::invalid_argument);
            }
        }
        /**
         * @brief 同步发送请求
         * @param request 请求对象
         * @return 发送结果错误码（成功为 0）
         */
        boost::system::error_code send_request(const request_t &request)
        {
            if (_state != session_state::CONNECTED)
                return boost::asio::error::not_connected;
            try
            {
                std::string data = request.to_string();
                return send_bytes(data);
            }
            catch (const std::exception &)
            {
                return boost::asio::error::invalid_argument;
            }
        }
        /**
         * @brief 异步发送响应
         * @param response 响应对象
         * @param callback 发送完成回调
         */
        void async_send_response(const response_t &response, std::function<void(const boost::system::error_code &)> callback = nullptr)
        {
            if (_state != session_state::CONNECTED)
            {
                if (callback)
                    callback(boost::asio::error::not_connected);
                return;
            }
            try
            {
                std::string data = response.to_string();
                async_send_bytes(data, callback);
            }
            catch (const std::exception &)
            {
                if (callback)
                    callback(boost::asio::error::invalid_argument);
            }
        }
        /**
         * @brief 同步发送响应
         * @param response 响应对象
         * @return 发送结果错误码（成功为 0）
         */
        boost::system::error_code send_response(const response_t &response)
        {
            if (_state != session_state::CONNECTED)
                return boost::asio::error::not_connected;
            try
            {
                std::string data = response.to_string();
                return send_bytes(data);
            }
            catch (const std::exception &)
            {
                return boost::asio::error::invalid_argument;
            }
        }
        /**
         * @brief 异步发送字符串
         * @param data 原始字节视图
         * @param callback 发送完成回调
         * @details
         *   - 内部会复制 `data` 到共享字符串，确保异步写期间缓冲区有效；
         *   - 支持 `TCP` 与 `SSL` 分支；完成回调会在 `IO` 线程调度。
         * @return
         *   - 若当前未连接：立即调用 `callback(not_connected)` 并返回；
         *   - 若已连接：在写完成或出错后以 `error_code` 通知。
         * @note
         *   - 成功发送后会更新统计`_bytes_sent`、`_messages_sent`、`_last_activity`；
         *   - 写入出错时，会调用 `_handle_error(ec)` 并进行必要的状态更新。
         */
        void async_send_bytes(std::string_view data, std::function<void(const boost::system::error_code &)> callback = nullptr)
        {
            if (_state != session_state::CONNECTED)
            {
                if (callback)
                    callback(boost::asio::error::not_connected);
                return;
            }
            // 保证异步写期间缓冲区的生命周期（复制到共享字符串）
            auto buffer_ptr = std::make_shared<std::string>(data);
            auto self = this->shared_from_this();
            if (_config._enable_ssl && _ssl_socket)
            {
                auto ssl_send_function = [self, callback, buffer_ptr](const boost::system::error_code &ec, std::uint64_t bytes_transferred)
                {
                    if (!ec)
                    {
                        self->_statistics._bytes_sent += bytes_transferred;
                        self->_statistics._messages_sent++;
                        self->_statistics.renewal_activity();
                    }
                    else
                        self->_handle_error(ec);
                    if (callback)
                        callback(ec);
                };
                boost::asio::async_write(*_ssl_socket, boost::asio::buffer(*buffer_ptr), ssl_send_function);
            }
            else
            {
                auto tcp_send_function = [self, callback, buffer_ptr](const boost::system::error_code &ec, std::uint64_t bytes_transferred)
                {
                    if (!ec)
                    {
                        self->_statistics._bytes_sent += bytes_transferred;
                        self->_statistics._messages_sent++;
                        self->_statistics.renewal_activity();
                    }
                    else
                        self->_handle_error(ec);
                    if (callback)
                        callback(ec);
                };
                boost::asio::async_write(_socket, boost::asio::buffer(*buffer_ptr), tcp_send_function);
            }
        }
        /**
         * @brief 关闭会话
         * @details 关闭会话，释放资源
         */
        void close()
        {
            if (_state == session_state::DISCONNECTED || _state == session_state::DISCONNECTING)
                return;
            _set_state(session_state::DISCONNECTING);
            boost::system::error_code ec;
            _on_data = {};
            _timer.cancel();
            if (_ssl_socket)
                _ssl_socket->lowest_layer().close(ec);
            else
                _socket.close(ec);
            _set_state(session_state::DISCONNECTED);
        }
    }; // end class session

} // end namespace fundamental
