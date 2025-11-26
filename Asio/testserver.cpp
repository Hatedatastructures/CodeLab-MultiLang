// client.cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>


// 协程函数：处理单个客户端连接的生命周期
boost::asio::awaitable<void> client_session(boost::asio::ip::tcp::endpoint endpoint, int client_id)
{
    try
    {
        boost::asio::ip::tcp::socket socket(co_await boost::asio::this_coro::executor);
        co_await socket.async_connect(endpoint, boost::asio::use_awaitable);
        std::cout << "客户端 " << client_id << " 连接成功." << std::endl;

        // 模拟发送请求
        std::string request_message = "Hello from client " + std::to_string(client_id) + "!";
        co_await boost::asio::async_write(socket, 
            boost::asio::buffer(request_message), boost::asio::use_awaitable);

        // 模拟接收响应
        std::vector<char> reply_buffer(128);
        size_t reply_length = co_await socket.async_read_some(
            boost::asio::buffer(reply_buffer), boost::asio::use_awaitable);
        std::cout << "客户端 " << client_id <<  std::endl;

        // 可以根据需要保持连接或关闭
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();
        std::cout << "客户端 " << client_id << " 连接关闭." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "客户端 " << client_id << " 错误: " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 8080); // 服务器地址和端口

        int num_clients = 10000; // 模拟的并发客户端数量
        std::vector<std::thread> threads;
        int num_threads = std::thread::hardware_concurrency(); // 使用CPU核心数作为线程数

        // 启动多个协程来模拟并发客户端
        for (int i = 0; i < num_clients; ++i)
        {
            boost::asio::co_spawn(io_context, 
                client_session(endpoint, i), boost::asio::detached);
        }

        // 启动线程池运行io_context
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([&io_context]()
                                 { io_context.run(); });
        }

        // 等待所有线程完成
        for (auto &t : threads)
        {
            t.join();
        }

        std::cout << "所有客户端测试完成." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "主函数错误: " << e.what() << std::endl;
    }
    return 0;
}