#pragma once
#include "../agreement/Http.hpp"
#include "../session/Fundamental.hpp"
#include "../session/Conversation.hpp"
#include "../agreement/Json.hpp"
#include "../../sched/ThreadPool.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/Http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/Json.hpp>
#include <boost/system/error_code.hpp>

#include <type_traits>
#include <fstream>
#include <unordered_map>
#include <optional>
#include <functional>
#include <algorithm>
#include <string>
#include <string_view>
#include <future>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace represents
{
    // 连接池默认配置（可直接赋值覆盖）
    struct connection_pool_defaults
    {
        static inline std::uint64_t min_connections{8};
        static inline std::uint64_t max_connections{16};
        static inline std::chrono::milliseconds borrow_timeout{2000};
        static inline std::chrono::milliseconds connect_timeout{1500};
        static inline std::chrono::seconds health_check_interval{10};

    };
    
    /**
   * @brief 文件密钥路径配置
   * @details 用于配置代理类的 `ssl` 证书校验等
   */
    struct transponder_config
    {
        std::string ssl_ca_file;                 // CA 证书文件路径
        std::string ssl_cert_file;               // 客户端证书链文件（可选，用于双向TLS）
        std::string ssl_key_file;                // 客户端私钥文件（可选，用于双向TLS）
        bool ssl_insecure_skip_verify{false};    // 是否跳过证书校验（默认关闭）
    };
    /**
   * @brief 基于 `http` 协议的服务端 `http / https` 请求转发器(代理)
   * @details 用于将客户端的http请求转发到指定的http服务器，并将服务器的响应返回给客户端
   * @note 支持 `http` 和 `https` 协议 , 并且可支持消息拦截过滤规则
   * @note 支持 `server`   --`https`->     `agent`    --`https`->    `client` 来保证安全传输
   * @warning`json` 配置 需要参考 `json` 配置文件格式
   */
    template <class body = boost::beast::http::string_body, class fields = boost::beast::http::fields>
    class transponder
    {
    public:
        using request  = protocol::http::request<body, fields>;
        using response = protocol::http::response<body, fields>;

        using request_func = std::function<void(request&)>;
        using response_func = std::function<void(response&)>;

        using session_ptr = std::shared_ptr<conversation::fundamental::session<request,response>>;
        
        /**
     * @brief 上游代理服务器配置
     */
        struct upstream
        {
            std::string domain; // 域名（用于 Host 匹配与设置）
            std::string host;   // 上游服务器主机名 或 IP
            std::uint16_t port{80}; // 上游服务器端口号
            bool use_https{false}; // 是否使用 HTTPS
        }; // end struct upstream

    private:
        boost::asio::io_context& _io_context; // io上下文
        std::unordered_multimap<std::string, upstream> _upstreams; // 上游代理名单，多IP：key=域名，value=上游配置
        conversation::connection_pool<request, response> _http_pool; // http连接池
        transponder_config _config{}; // 统一配置（可覆盖）

        // 任务管理与执行器
        std::atomic<bool> _stopping{false}; // 停止标志
        std::atomic<std::size_t> _active_tasks{0}; // 当前活跃异步任务数
        std::size_t _max_async_tasks{1024}; // 最大并发异步任务数，防止任务爆炸
        std::mutex _tasks_mutex; // 任务计数互斥
        std::condition_variable _tasks_cv; // 任务计数条件变量
        std::shared_ptr<wan::pool::thread_pool> _async_pool; // 线程池

    public:
        explicit transponder(boost::asio::io_context& io_context, const transponder_config& config = transponder_config())
            : _io_context(io_context), _http_pool(io_context), _config(config) { _http_pool.start(); }

        /**
     * @brief 设置 `CA` 证书文件路径
     */
        void set_ssl_ca_file(const std::string& path) 
        { 
            _config.ssl_ca_file = path; 
        }
        /**
     * @brief 设置 `客户端证书链` 文件路径
     */
        void set_ssl_cert_file(const std::string& path) 
        { 
            _config.ssl_cert_file = path; 
        }
        /**
     * @brief 设置 `客户端私钥` 文件路径
     */
        void set_ssl_key_file(const std::string& path) 
        { 
            _config.ssl_key_file = path; 
        }
        /**
     * @brief 设置是否跳过 `服务器证书校验`
     */
        void set_ssl_insecure_skip_verify(bool v) 
        { 
            _config.ssl_insecure_skip_verify = v; 
        }

        /**
     * @brief 添加或更新上游服务器配置
     * @param domain 名单域名（用于 Host 匹配与设置）
     * @param ip_or_host 上游服务器主机名 或 IP 地址
     * @param port 上游服务器端口号
     * @param use_https 是否使用 `HTTPS` 协议
     */
        void add_upstream(const std::string &domain, const std::string &ip_or_host, std::uint16_t port, bool use_https)
        {
            // 若未提供 IP/主机名，则尝试解析域名得到首个 IP；失败则回退为域名
            std::string host = ip_or_host;
            if (host.empty())
            {
                boost::asio::ip::tcp::resolver resolver(_io_context);
                boost::system::error_code ec;
                auto results = resolver.resolve(domain, std::to_string(port), ec);
                if (!ec)
                {
                    auto it = results.begin();
                    if (it != results.end())
                        host = it->endpoint().address().to_string();
                }
                if (host.empty())
                    host = domain;
            }

            upstream up;
            up.domain = domain;
            up.host = host;
            up.port = port;
            up.use_https = use_https;
            _upstreams.emplace(up.domain, up);

            conversation::endpoint_config cfg;
            cfg.host = up.host;
            cfg.port = up.port;
            cfg.session_cfg._enable_ssl = up.use_https;
            cfg.session_cfg._tls_server_name = up.domain.empty() ? up.host : up.domain;
            cfg.session_cfg._ssl_ca_file = _config.ssl_ca_file;
            cfg.session_cfg._ssl_cert_file = _config.ssl_cert_file;
            cfg.session_cfg._ssl_key_file = _config.ssl_key_file;
            cfg.session_cfg._ssl_insecure_skip_verify = _config.ssl_insecure_skip_verify;
            // 应用命名空间级连接池默认配置
            cfg.min_connections = connection_pool_defaults::min_connections;
            cfg.max_connections = connection_pool_defaults::max_connections;
            cfg.borrow_timeout = connection_pool_defaults::borrow_timeout;
            cfg.connect_timeout = connection_pool_defaults::connect_timeout;
            cfg.health_check_interval = connection_pool_defaults::health_check_interval;
            _http_pool.add_endpoint(cfg);
        }

        /**
     * @brief 移除上游服务器配置
     * @param name 上游服务器名称
     */
        void remove_upstream(const std::string &domain)
        {
            auto range = _upstreams.equal_range(domain);
            for (auto it = range.first; it != range.second; ++it)
            {
                _http_pool.remove_endpoint(it->second.host, it->second.port);
            }
            _upstreams.erase(domain);
        }

        /**
     * @brief 加载json配置文件
     * @param path json文件路径
     * @return 是否加载成功
     */
        bool json_config_file(const std::string &path)
        {
            std::ifstream in(path, std::ios::binary);
            if (!in) return false;
            std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            return load_config_json(data);
        }

        // 停止转发器（拒绝新异步任务，停止连接池）
        bool stop()
        {
            if (_stopping.exchange(true))
                return true;
            _http_pool.stop();
            return true;
        }

        // 关停转发器（等待在途任务完成，可选超时）
        bool shutdown(std::chrono::milliseconds timeout = std::chrono::milliseconds{5000})
        {
            _stopping.store(true, std::memory_order_release);
            _http_pool.stop();

            auto deadline = std::chrono::steady_clock::now() + timeout;
            std::unique_lock<std::mutex> lk(_tasks_mutex);
            while (_active_tasks.load(std::memory_order_relaxed) > 0)
            {
                if (std::chrono::steady_clock::now() >= deadline)
                    break;
                _tasks_cv.wait_until(lk, deadline, [this]{ return _active_tasks.load(std::memory_order_relaxed) == 0; });
            }
            lk.unlock();

            if (_async_pool && _async_pool->is_running())
            {
                // 尝试关停外部线程池
                _async_pool->shutdown(timeout);
            }

            return _active_tasks.load(std::memory_order_relaxed) == 0;
        }

        // 设置外部线程池
        void set_async_executor(std::shared_ptr<wan::pool::thread_pool> pool)
        {
            _async_pool = std::move(pool);
            if (_async_pool && !_async_pool->is_running())
            {
                _async_pool->start();
            }
        }

        /**
     * @brief 同步转发请求
     * @param req 请求
     * @return 响应
     */
        response forward_sync(request req,request_func request_filter = {},response_func response_filter = {})
        {
            namespace http = boost::beast::http;
            bool has_host = req.base().find(http::field::host) != req.base().end();
            const upstream* up = select_upstream_and_apply_route_headers(req);
            if (!up)
            {
                if (has_host)
                    return make_error_response(403, "forbidden", "domain not allowed");
                return make_error_response(502, "proxy resolution failed", "no upstream");
            }

            if (request_filter)
                request_filter(req); // 请求控制

            apply_host_header_if_missing(req, *up);

            // 上游访问
            auto resp = perform_upstream(req, *up);
            if (response_filter) response_filter(resp); // 访问控制
            return resp;
        }
        /**
     * @brief 异步转发请求
     * @param req 请求
     * @param request_filter 请求回调
     * @param response_filter 响应回调
     * @return `std::future<response>` 异步响应
     */
        std::future<response> forward_async(request req,request_func request_filter = {},response_func response_filter = {})
        {
            // 若正在停止，直接返回错误响应
            if (_stopping.load(std::memory_order_acquire))
            {
                std::promise<response> p;
                p.set_value(make_error_response(503, "service stopping", "agent is stopping"));
                return p.get_future();
            }

            // 简单并发门限，防止任务爆炸
            while (_active_tasks.load(std::memory_order_relaxed) >= _max_async_tasks)
            {
                std::unique_lock<std::mutex> lk(_tasks_mutex);
                _tasks_cv.wait_for(lk, std::chrono::milliseconds(5), [this]{
                    return _active_tasks.load(std::memory_order_relaxed) < _max_async_tasks || _stopping.load(std::memory_order_acquire);
                });
                if (_stopping.load(std::memory_order_acquire))
                {
                    std::promise<response> p;
                    p.set_value(make_error_response(503, "service stopping", "agent is stopping"));
                    return p.get_future();
                }
            }

            auto forward_task = [this, req = std::move(req), request_filter, response_filter]() mutable -> response
            {
                // 任务计数 +1
                _active_tasks.fetch_add(1, std::memory_order_relaxed);
                // RAII 计数 -1 并唤醒等待者
                struct Guard 
                { 
                    transponder* self; 
                    ~Guard(){ self->_active_tasks.fetch_sub(1, std::memory_order_relaxed); 
                    std::lock_guard<std::mutex> lk(self->_tasks_mutex); 
                    self->_tasks_cv.notify_all(); } 
                };
                Guard g{this};

                if (_stopping.load(std::memory_order_acquire))
                    return make_error_response(503, "service stopping", "agent is stopping");

                return forward_sync(std::move(req), request_filter, response_filter);
            };

            if (_async_pool && _async_pool->is_running())
            {
                return _async_pool->submit(std::move(forward_task));
            }
            return std::async(std::launch::async, std::move(forward_task));
        }

    private:

        /**
     * @brief 解析 `host` 头，返回主机名与可选端口
     * @param host_hdr `host` 头值
     * @return 主机名与可选端口
     */
        std::pair<std::string, std::optional<std::uint16_t>> parse_host_header(std::string_view host_hdr)
        {
            std::string name(host_hdr);
            // 去除前后空白
            auto lpos = name.find_first_not_of(" \t");
            if (lpos != std::string::npos) name.erase(0, lpos);
            auto rpos = name.find_last_not_of(" \t");
            if (rpos != std::string::npos) name.erase(rpos + 1);

            std::optional<std::uint16_t> port;
            if (!name.empty() && name.front() == '[')
            {
                auto rb = name.find(']');
                if (rb != std::string::npos)
                {
                    std::string core = name.substr(1, rb - 1);
                    if (rb + 1 < name.size() && name[rb + 1] == ':')
                    {
                        try { port = static_cast<std::uint16_t>(std::stoi(name.substr(rb + 2))); } 
                        catch (...) { port.reset(); }
                    }
                    name = core;
                }
            }
            else
            {
                auto pos = name.find(':');
                if (pos != std::string::npos)
                {
                    try { port = static_cast<std::uint16_t>(std::stoi(name.substr(pos + 1))); } catch (...) { port.reset(); }
                    name = name.substr(0, pos);
                }
            }
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return {name, port};
        }

        /**
     * @brief 在白名单中按主机名与端口匹配上游
     * @param host_name 主机名
     * @param req_port 请求端口
     * @return 匹配的上游配置指针，若不存在则返回 `nullptr`
     */
        const upstream* match_upstream(std::string_view host_name, std::optional<std::uint16_t> req_port)
        {
            auto range = _upstreams.equal_range(std::string(host_name));
            if (range.first == range.second) return nullptr;
            if (req_port)
            {
                for (auto it = range.first; it != range.second; ++it)
                    if (it->second.port == *req_port)
                        return &it->second;
            }
            return &range.first->second;
        }

        /**
     * @brief 选择上游并应用头部改写（仅基于 Host 白名单）
     * @param req 请求
     * @return 匹配的上游配置指针，若不存在则返回 `nullptr`
     */
        const upstream* select_upstream_and_apply_route_headers(request &req)
        {
            namespace http = boost::beast::http;
            // 优先使用 Host 进行白名单映射
            auto hit = req.base().find(http::field::host);
            if (hit != req.base().end())
            {
                auto parsed = parse_host_header(std::string(hit->value()));
                return match_upstream(parsed.first, parsed.second);
            }
            return nullptr;
        }

        /**
     * @brief 如果缺失 Host，则兜底设置
     * @param req 请求
     * @param up 上游配置
     */
        void apply_host_header_if_missing(request &req, const upstream &up)
        {
            if (req.base().find(boost::beast::http::field::host) == req.base().end())
            {
                bool https = up.use_https;
                std::uint16_t default_port = https ? 443 : 80;
                const std::string &host_name = up.domain.empty() ? up.host : up.domain; 
                if (up.port != default_port) // 为空 `ip` 不为空域名
                    req.base().set(boost::beast::http::field::host, host_name + ":" + std::to_string(up.port));
                else
                    req.base().set(boost::beast::http::field::host, host_name);
            }
        }


        /**
     * @brief 执行 `HTTP` 上游访问
     * @param req 客户端请求
     * @param up 上游配置
     * @return response 上游响应
     */
        response perform_upstream_plain(const request &req, const upstream &up)
        {
            namespace http = boost::beast::http;
            response parsed;
            auto borrowed = _http_pool.borrow(up.host, up.port);
            if (!borrowed)
                return make_error_response(502, "Bad Gateway", "upstream unavailable");
            auto sp = borrowed.value();

            std::mutex mtx;
            std::condition_variable cv;
            bool done = false;
            std::string buffer;

            auto response_value = [this, &parsed, &mtx, &cv, &done, &buffer, sp](session_ptr /*ptr*/, std::string_view sv)
            {
                std::lock_guard<std::mutex> lock(mtx);
                buffer.append(sv.data(), sv.size());
                response tmp;
                if (tmp.from_string(buffer))
                {
                    parsed = std::move(tmp);
                    done = true;
                    cv.notify_one();
                }
            };

            request sending = req;
            if (sending.base().find(http::field::connection) == sending.base().end())
                sending.base().set(http::field::connection, "keep-alive");
            sp->set_reception_processing(response_value);
            sp->start(); // 启动读取循环
            boost::system::error_code ec = sp->send_request(sending);
            if (ec)
            {
                sp->set_reception_processing(nullptr);
                _http_pool.invalidate(sp);
                return make_error_response(502, "agent exception", std::string("send failed: ") + ec.message());
            }
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::milliseconds(15000), [&]{ return done; });
            }
            if (!done)
            {
                sp->set_reception_processing(nullptr);
                _http_pool.invalidate(sp);
                return make_error_response(504, "no response received", "upstream timeout");
            }
            sp->set_reception_processing(nullptr);
            _http_pool.give_back(sp);
            return parsed;
        }

        /**
     * @brief 执行 `HTTPS` 上游访问
     * @param req 客户端请求
     * @param up 上游配置
     * @return response 上游响应
     */
        response perform_upstream_ssl(const request &req, const upstream &up)
        {
            // SSL 由连接池会话的端点配置决定，这里直接复用 plain 实现
            return perform_upstream_plain(req, up);
        }

        /**
     * @brief 执行上游请求（改为调用拆分后的实现）
     */
        response perform_upstream(const request &req,const upstream &up)
        {
            namespace http = boost::beast::http;
            try
            {
                return up.use_https ? perform_upstream_ssl(req, up) : perform_upstream_plain(req, up);
            }
            catch (const std::exception &e)
            {
                return make_error_response(502, "exception in proxy", std::string("upstream error: ") + e.what());
            }
        }

        /**
     * @brief 从 `JSON` 文件加载配置
     * @param json_text `JSON` string_view 文本
     * @return 是否成功
     */
        bool load_config_json(std::string_view json_text)
        {
            boost::system::error_code ec;
            auto jv = boost::json::parse(json_text, ec);
            if (ec) return false;
            return load_config_value(jv);
        }

        /**
     * @brief 从 `boost::json::value` 值加载配置
     * @param jv `boost::json::value` 值
     * @return 是否成功
     */
        bool load_config_value(const boost::json::value &jv)
        {
            try
            {
                if (jv.is_array())
                    parse_upstreams(jv.as_array());
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        /**
     * @brief 解析上游配置
     * @param arr JSON 数组
     * @warning 如果`ip`为空，将尝试解析`domain`为IP 地址 ，解析错误时`host`回退为域名
     */
        void parse_upstreams(const boost::json::array &arr)
        {
            for (const auto &u : arr)
            {
                const auto &o = u.as_object();

                std::string domain = boost::json::value_to<std::string>(o.at("domain"));
                std::uint16_t port = static_cast<std::uint16_t>(boost::json::value_to<int>(o.at("port")));
                bool https         = boost::json::value_to<bool>(o.at("https"));

                std::string host   = boost::json::value_to<std::string>(o.at("ip"));
                if (host.empty())
                {
                    boost::asio::ip::tcp::resolver resolver(_io_context);
                    boost::system::error_code ec;
                    auto results = resolver.resolve(domain, std::to_string(port), ec);
                    if (!ec)
                    {
                        auto it = results.begin();
                        if (it != results.end())
                            host = it->endpoint().address().to_string();
                    }
                    if (host.empty())
                        host = domain; // 解析失败时回退为域名
                }

                upstream up{domain, host, port, https};
                _upstreams.emplace(domain, up);

                conversation::endpoint_config cfg;
                cfg.host = up.host;
                cfg.port = up.port;
                cfg.session_cfg._enable_ssl = up.use_https;
                cfg.session_cfg._tls_server_name = up.domain.empty() ? up.host : up.domain;
         cfg.session_cfg._ssl_ca_file = _config.ssl_ca_file;
         cfg.session_cfg._ssl_cert_file = _config.ssl_cert_file;
         cfg.session_cfg._ssl_key_file = _config.ssl_key_file;
         cfg.session_cfg._ssl_insecure_skip_verify = _config.ssl_insecure_skip_verify;
                // 应用命名空间级连接池默认配置
                cfg.min_connections = connection_pool_defaults::min_connections;
                cfg.max_connections = connection_pool_defaults::max_connections;
                cfg.borrow_timeout = connection_pool_defaults::borrow_timeout;
                cfg.connect_timeout = connection_pool_defaults::connect_timeout;
                cfg.health_check_interval = connection_pool_defaults::health_check_interval;
                _http_pool.add_endpoint(cfg);
            }
        }

        /**
     * @brief 创建错误响应
     *
     * @param code `HTTP` 状态码
     * @param reason 状态原因
     * @param body_text 响应体文本
     * @return response 错误响应
     */
        response make_error_response(int code, const char *reason, const std::string &body_text)
        {
            response resp;
            resp.base().result(static_cast<boost::beast::http::status>(code));
            resp.base().reason(reason);
            resp.base().set(boost::beast::http::field::content_type, "text/plain");
            resp.body() = body_text;
            resp.base().set(boost::beast::http::field::content_length, std::to_string(resp.body().size()));
            return resp;
        }
    }; // end class transponder
} // end namespace represents
