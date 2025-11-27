#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// 命名空间别名简化代码
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// Asio协程相关类型别名
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

/**
 * @brief 处理单个客户端连接的协程
 * @param socket 已连接的客户端socket
 */
awaitable<void> handle_client(tcp::socket socket)
{
    try
    {
        std::string buffer; // 数据缓冲区
        buffer.resize(1024);
        while (true)
        {
            // 异步读取客户端数据（co_await等待完成）
            std::size_t n = co_await socket.async_read_some(
                asio::buffer(buffer), use_awaitable);

            // 打印接收到的数据
            std::cout << "来自 " << socket.remote_endpoint() << ": "
                      << buffer.substr(0, n) << "数据：" << buffer.substr(n) <<  std::flush;

            // 异步回显数据给客户端
            co_await asio::async_write(
                socket, asio::buffer(buffer, n), use_awaitable);
        }
    }
    catch (const std::exception &e)
    {
        // 捕获异常（如客户端断开连接）
        std::cerr << "Client " << socket.remote_endpoint() << " error: "
                  << e.what() << std::endl;
    }
}

/**
 * @brief 循环接受客户端连接的协程
 * @param acceptor 服务器监听器
 */
awaitable<void> accept_loop(tcp::acceptor &acceptor)
{
    while (true)
    {
        // 异步接受新连接（co_await等待连接）
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);

        // 打印新客户端连接信息
        std::cout << "New client connected: " << socket.remote_endpoint() << std::endl;

        // 启动客户端处理协程（detached：后台运行，不阻塞当前协程）
        co_spawn(acceptor.get_executor(),
            handle_client(std::move(socket)),
            detached);
    }
}

/**
 * @brief TCPServer类
 */
class TcpServer
{
public:
    /**
     * @brief 构造函数：初始化监听器并启动接受循环
     * @param io_context Asio的IO上下文
     * @param port 监听端口
     */
    TcpServer(asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        // 启动连接接受协程
        co_spawn(io_context, accept_loop(acceptor_), detached);
    }

private:
    tcp::acceptor acceptor_; // 服务器监听器
};

int main()
{
    try
    {
        asio::io_context io_context;        // Asio核心IO上下文
        TcpServer server(io_context, 8080); // 启动服务器（监听8080端口）

        std::cout << "TCPServer running on port 8080..." << std::endl;
        std::vector<std::jthread> threads;
        for (int i = 0; i < 16; ++i)
        {
            threads.emplace_back([&io_context]()
                                 { io_context.run(); });
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}