#pragma once
#include "Asio/Model/Network/Network.hpp"
#include "Asio/Model/Sched/ThreadPool.hpp"

#include <memory>
#include <string>
#include <iostream>

using namespace wan;
using namespace wan::network;

class agent_service : public std::enable_shared_from_this<agent_service>
{
    // std::string web_root;
public:
    using session_ptr = std::shared_ptr<session::session<http::request<>, http::response<>>>;

private:
    std::function<void(session_ptr, std::string_view)> reception_processing; // 接收处理函数
private:
    std::shared_ptr<pool::thread_pool> thread_pool;                                                     // 异步执行流
    std::shared_ptr<business::transponder<>> transponder;                                               // 转发器
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;                                           // 监听器
    std::shared_ptr<session::session_management<http::request<>, http::response<>>> session_management; // 会话管理器
public:
    // 构造时仅创建对象，不主动打开/绑定监听器，避免重复 open 导致的异常
    agent_service(boost::asio::io_context &io_context, std::shared_ptr<pool::thread_pool> stream_pool)
        : thread_pool(stream_pool),
          transponder(std::make_shared<business::transponder<>>(io_context)),
          acceptor(std::make_shared<boost::asio::ip::tcp::acceptor>(io_context)),
          session_management(std::make_shared<session::session_management<http::request<>, http::response<>>>(io_context))
    {
    }

    // 提供带端口的构造重载，内部调用 bind_port 做安全绑定
    agent_service(boost::asio::io_context &io_context, std::uint16_t port)
        : transponder(std::make_shared<business::transponder<>>(io_context)),
          acceptor(std::make_shared<boost::asio::ip::tcp::acceptor>(io_context)),
          session_management(std::make_shared<session::session_management<http::request<>, http::response<>>>(io_context))
    {
        bind_port(port);
    }

    /**
     * @brief 增加上游代理端点
     */
    void increase_endpoint(std::string domain, std::string host, std::uint16_t port, bool https = false)
    {
        transponder->add_upstream(domain, host, port, https);
    }

    /**
     * @brief 绑定监听端口并进入监听状态
     * @param port 监听端口
     */
    bool bind_port(std::uint16_t port)
    {
        boost::system::error_code ec;
        if (acceptor->is_open())
            acceptor->close(ec); // 重新绑定前确保关闭

        acceptor->open(boost::asio::ip::tcp::v4(), ec);
        if (ec)
            return false;

        // 复用地址，避免 TIME_WAIT 等导致的 bind 失败
        acceptor->set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec)
            return false;

        acceptor->bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), ec);
        if (ec)
            return false;

        acceptor->listen(boost::asio::socket_base::max_listen_connections, ec);
        return !ec;
    }

    auto transponder_ptr()
        -> std::shared_ptr<business::transponder<>>
    {
        return transponder;
    }

    /**
     * @brief 增加异步执行流
     */
    void set_execution_stream(std::shared_ptr<wan::pool::thread_pool> stream)
    {
        thread_pool = stream;
        transponder->set_async_executor(thread_pool); // 所有异步转发逻辑经过线程池来调度
    }

    /**
     * @brief 访问异步执行流
     * @return std::shared_ptr<pool::thread_pool> 异步执行流
     */
    std::shared_ptr<pool::thread_pool> execution_stream()
    {
        return thread_pool;
    }

    /**
     * @brief 设置SSL CA文件路径
     * @param path CA文件路径
     */
    void CA_path(std::string path)
    {
        transponder->set_ssl_ca_file(path);
        // 默认启用证书校验以避免中间人攻击；如需跳过校验，提供专门的接口。
        transponder->set_ssl_insecure_skip_verify(false);
    }

    void set_insecure_skip_verify(bool enable)
    {
        transponder->set_ssl_insecure_skip_verify(enable);
    }

    void set_reception_processing(std::function<void(session_ptr, std::string_view)> processing)
    {
        reception_processing = processing;
    }

    /**
     * @brief 转发请求
     */
    bool start()
    {
        if (!(acceptor && acceptor->is_open()))
            return false; // 未绑定端口

        if (thread_pool && session_management)
        {
            const bool pool_ok = (thread_pool->is_running() || thread_pool->start());
            const bool sm_ok = session_management->start();
            if (pool_ok && sm_ok)
            {
                enter_monitoring();
                return true;
            }
        }
        return false;
    }

    void stop()
    {
        boost::system::error_code ec;
        if (acceptor && acceptor->is_open())
            acceptor->close(ec);
        if (session_management)
            session_management->stop();
        if (thread_pool)
            thread_pool->stop();
    }

    void enter_monitoring()
    {
        auto monitoring_function = [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                boost::system::error_code re_ec;
                auto ep = socket.remote_endpoint(re_ec); // 获取客户端 ip 地址 信息
                if (!re_ec)                              // 输出日志
                    std::cout << std::format("[{:%Y-%m-%d %H:%M:%S}] [conversationss : {}] Accepted connection from: {}:{}\n",
                                             std::chrono::system_clock::now(), session_management->get_session_count(), ep.address().to_string(), ep.port());
                else
                    std::cout << std::format("[{:%Y-%m-%d %H:%M:%S}] [conversationss : {}] Accepted connection\n",
                                             std::chrono::system_clock::now(), session_management->get_session_count());
                // 捕获到共享指针，保证任务可复制以适配 std::function 存储
                auto self = this->shared_from_this();
                auto socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket));
                auto task = [self, socket_ptr]() mutable
                {
                    self->forward_processing(std::move(*socket_ptr));
                };
                thread_pool->submit(std::move(task)); // 直接提交可调用对象，防止形参无法进行资源移动
            }
            else
                std::cerr << "accept error: " << ec.message() << std::endl; // 监听错误
            enter_monitoring();
        };
        if (acceptor && acceptor->is_open())
            acceptor->async_accept(std::move(monitoring_function));
    }
    void forward_processing(boost::asio::ip::tcp::socket &&socket)
    {
        auto pair = session_management->create_server_session(std::move(socket));
        if (!pair.second)
            return; // 创建会话失败
        if (reception_processing)
            pair.second->set_reception_processing(reception_processing);
        pair.second->start();
    }
};