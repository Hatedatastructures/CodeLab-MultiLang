#include <iostream>
#include <stdio.h>
#include <cstring>
#include <boost/asio.hpp>

using namespace boost::asio::ip;
class server
{
    std::shared_ptr<udp::socket> _connect_socket; //连接端socket
    boost::asio::io_context& _io_service; //调度器引用
    udp::endpoint _sender_message; //发送端信息

    std::string  _command; //远程命令
    std::string _data;
    void conmmand_func(const std::string& command) 
    {
        auto call_back = [this,command]
        {
            FILE* fp = popen(command.c_str(),"r");
            if(fp == nullptr) 
            {
                std::cout << "临时进程创建失败: " << command << std::endl;
                return; // 提前返回，避免后续操作空指针
            }
            std::string output_commands;
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));  // 修正memset参数顺序
            while(fgets(buffer,sizeof(buffer),fp))
            {
                output_commands += buffer;
            }
            if(pclose(fp) == -1 )
            {
                std::cout << "关闭进程失败: " << command << std::endl;
            }
            auto call_back_ = [this](boost::system::error_code erro,uint64_t bytes_transferred)
            {
                if(!erro)
                {
                    std::cout << "[连接方ip: " << _sender_message.address().to_string() << "] : " <<  " [发送字节数 : " << bytes_transferred  << "]  命令："<< _command << std::endl;
                } //记录发送端ip行为
            };
            _connect_socket->async_send_to(boost::asio::buffer(output_commands),_sender_message,call_back_);
        }; //发送数据
        boost::asio::post(_io_service,call_back); //将任务放到io线程队列中
    }
public:
    server(boost::asio::io_context& io_service,uint16_t port)
    :_io_service(io_service)
    {
        _connect_socket = std::make_shared<udp::socket>(io_service, udp::endpoint(udp::v4(), port));
        std::cout << "server port :" << port << std::endl;
    }
    void start()
    {
        _data.resize(1024);
        auto call_back = [this](boost::system::error_code erro,uint64_t bytes_transferred)
        {
            if(!erro)
            {
                _command = _data.substr(0,bytes_transferred);
                conmmand_func(_command);
                start(); //递归调用，等待下一次接收
            }
        };
        _connect_socket->async_receive_from(boost::asio::buffer(_data,1024),_sender_message,call_back);
    }
};