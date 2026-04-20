#include "boost/asio.hpp"
#include <iostream>

using namespace boost::asio::ip;
class server
{
    std::shared_ptr<udp::socket> _socket; //创建udp套接字
    udp::endpoint _sender;  //客户端信息
    std::string _data;  //接收到的数据
    void dispose(uint64_t bytes_transferred)
    {
        std::cout << "[" << _sender.address().to_string() << "] : " <<  " [字节数 : " << bytes_transferred  << "] "<< _data << std::endl;
    }
    void start()
    {
        _data.resize(1024);
        auto call_back = [this](boost::system::error_code erro,uint64_t bytes_transferred)
        {
            if(!erro){dispose(bytes_transferred);start();}
        };
        _socket->async_receive_from(boost::asio::buffer(_data,1024),_sender,call_back);
    }
public:
    server(boost::asio::io_context& scheduler, uint16_t port)
    {
        _socket = std::make_shared<udp::socket>(scheduler,udp::endpoint(udp::v4(),port));
        std::cout << "server port : " << port << std::endl;
        start();
    }
};
