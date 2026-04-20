#include <boost/asio.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <string>

int main()
{
    try
    {
        // 创建io_context对象
        boost::asio::io_context io_service;

        // 创建UDP socket
        boost::asio::ip::udp::socket socket(io_service);
        socket.open(boost::asio::ip::udp::v4());

        // 绑定socket到本地端口
        boost::asio::ip::udp::endpoint local_endpoint(boost::asio::ip::udp::v4(), 8888);
        socket.bind(local_endpoint);

        std::cout << "Server is listening for broadcast messages on port 8888" << std::endl;

        // 接收广播消息
        boost::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t bytes_received = socket.receive_from(boost::asio::buffer(buffer), sender_endpoint);

        std::cout << "Received broadcast message: " << std::string(buffer.data(), bytes_received) << std::endl;
        std::cout << "From IP: " << sender_endpoint.address() << ", Port: " << sender_endpoint.port() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}