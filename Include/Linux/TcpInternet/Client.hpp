#include "boost/asio.hpp"
#include <iostream>
#include <memory>

using namespace boost::asio::ip;
class client
{
    tcp::socket _socket;
    tcp::endpoint _acceptor;
    std::string _data; //接受数据缓冲区
public:
    client(boost::asio::io_context& io_context,const std::string string_ip, short port)
    :_socket(io_context),_acceptor(tcp::endpoint(make_address(string_ip),port))
    {
        std::cout << "连接到的服务器端口号是：" << port << std::endl;
        _data.resize(1024);
    }
    void start()
    { 
        auto connection_processing = [this](boost::system::error_code ec)
        {
            if(!ec)
            {
                std::cout << "连接成功, 服务器地址" << _acceptor.address().to_string() << " " << _acceptor.port() << std::endl;
                startReceive();
            }
            else
            {
                std::cout << "连接失败" << ec.message() << std::endl;
            }
        };
        _socket.async_connect(_acceptor, connection_processing);
    }
    void sendData(const std::string& message)
    { //外部处理发送数据
        if(!_socket.is_open()) //判断是否连接
        {
            std::cerr << "连接已关闭，无法发送" << std::endl;
            return;
        }
        auto send_processing = [this](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if(!ec)
            {
                std::cout << "发送字节：" << bytes_transferred <<" 到服务器" << std::endl;
            }
            else
            {
                std::cerr << "发送失败" << ec.message() << std::endl;
                closeConnection();
            }
        };
        boost::asio::async_write(_socket, boost::asio::buffer(message), send_processing);
    }
    void closeConnection()
    { //关闭字节流
        if(_socket.is_open())
        {
            boost::system::error_code ec;
            _socket.close(ec);
            if(ec)
            {
                std::cerr << "关闭连接失败" << ec.message() << std::endl;
            }
            else
            {
                std::cout << "关闭连接成功" << std::endl;
            }
        }
    }
private:
    void startReceive()
    { //异步接受数据
        auto accept_processing = [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if(!ec)
            {
                if(!_data.empty())
                {
                    std::cout << "收到服务器回复：" << _data.substr(0, bytes_transferred) << std::endl;
                    startReceive();
                }

            }
            else if(ec != boost::asio::error::eof)
            {
                std::cerr << "接受数据流失败" << ec.message() << std::endl;
                closeConnection();
            }
            else
            {
                std::cout << "服务器断开连接" << ec.message() << std::endl;
                closeConnection();
            }
        };
        _socket.async_receive(boost::asio::buffer(_data,1024), accept_processing);
    }
};