#include "boost/asio.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <thread>
using namespace boost::asio::ip;
class client
{
    std::shared_ptr<udp::socket> _socket;
    udp::endpoint _server_endpoint;
    std::thread _thread;
public:
    client(boost::asio::io_context& context,const std::string& server_ip,int server_port)
    :_socket(std::make_shared<udp::socket>(context,udp::endpoint(udp::v4(),0)))
    {
        udp::resolver resolver(context);
        _server_endpoint = *resolver.resolve(udp::v4(),server_ip,std::to_string(server_port)).begin();
        _thread  = std::thread([this](){thread_cin();});
    }
    void send(const std::string& message)
    {
        auto message_task = [this](boost::system::error_code error, uint64_t bytes_size)
        {
            if(!error)
            {
                std::cout << "发送成功 " << bytes_size << "字节到服务器" << std::endl;
            }
            else
            {
                std::cout << "发送失败"  << error.message() << std::endl;
            }
        };
        _socket->async_send_to(boost::asio::buffer(message),_server_endpoint,message_task);
    }
    void thread_cin()
    {
        std::string message;
        std::cout << "输入要发送的内容 -> ";
        while(std::getline(std::cin,message))
        {
            send(message);
            std::cout << "输入要发送的内容 -> "; 
        }
    }
    ~client()
    {
        _thread.join();
    }
};