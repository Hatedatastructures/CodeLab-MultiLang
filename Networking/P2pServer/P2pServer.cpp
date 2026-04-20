#include <boost/asio.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <string>
#include <boost/beast/http.hpp>

int main()
{
    try
    { // udp 广播
        // 创建io_context对象
        boost::asio::io_context io_service;

        // 创建UDP socket
        boost::asio::ip::udp::socket socket(io_service);
        socket.open(boost::asio::ip::udp::v4());

        // 设置socket为广播模式
        socket.set_option(boost::asio::socket_base::broadcast(true));

        // 设置广播地址和端口
        boost::asio::ip::udp::endpoint broadcast_endpoint(boost::asio::ip::make_address("127.255.255.255"), 8888);

        // 发送广播消息
        std::string message = "Hello, I am a P2P node!";
        socket.send_to(boost::asio::buffer(message), broadcast_endpoint);

        std::cout << "Broadcast message sent" << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    boost::asio::ip::udp::socket::broadcast(false);
    return 0;
}