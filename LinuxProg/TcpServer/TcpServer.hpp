#include <boost/asio.hpp>
#include "Module/ThreadPool.hpp"
#include <boost/beast/http.hpp>
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>

using namespace boost::asio::ip;

class html_data
{
    std::ifstream _html_stream; //html文件流
public:
    std::string _html_data; //html数据
    std::uint64_t _html_size; //html文件大小
    html_data(const std::string html_data)
    {
        _html_stream.open(html_data);
        if (!_html_stream.is_open()) 
        {
            throw std::runtime_error("Failed to open HTML file: " + html_data);
        }
    }
    bool read()
    {
        _html_stream.seekg(0, std::ios::end);
        size_t size = _html_stream.tellg();
        _html_stream.seekg(0, std::ios::beg);
        
        // 调整字符串大小并读取
        _html_data.resize(size);
        _html_stream.read(&_html_data[0], size);
        
        return _html_stream.gcount() > 0;
    }
    ~html_data()
    {
        _html_stream.close();
    }
};
class server
{
    std::unique_ptr<pool::thread_pool> _thread; // 动态线程池
    tcp::acceptor _acceptor;  //tcp接收器
    std::string _html; //html数据
    void responseInformation(tcp::socket& socket,const std::string temp_data)
    {
        // std::string response_data = "服务器收到数据 : " + temp_data;
        (void)(temp_data);
        std::string http_header = "HTTP/1.1 200 OK\r\n";
        http_header += "Content-Type: text/html; charset=utf-8\r\n";  // 声明内容为HTML
        http_header += "Content-Length: " + std::to_string(_html.size()) + "\r\n";  // 内容长度
        http_header += "\r\n";  // 空行分隔头和体
        std::string response_data = http_header + _html;
        auto response_func = [this](boost::system::error_code s,uint64_t len)
        {
            if(s)
            {
                std::cout << "回复客户端失败" << s.message() << " 数据长度 : " << len << std::endl;
            }
        }; //回传数据
        socket.async_write_some(boost::asio::buffer(response_data),response_func);
    }
    void informationStream(tcp::socket socket)
    { //接受数据流
        std::shared_ptr<tcp::socket> client_socket = std::make_shared<tcp::socket>(std::move(socket));
        auto _data = std::make_shared<std::string>();
        _data->resize(1024);
        auto process_stream = [this,_data,client_socket](boost::system::error_code s,uint64_t len)
        {
            if(!s)
            {
                if(len > 0)
                {
                    std::cout << "收到来自 " << client_socket->remote_endpoint() << " 的数据长度为 : " << len 
                    << " 数据 : " << _data->substr(0,len) << std::endl;
                    //回复客户端数据
                    responseInformation(*client_socket,_data->substr(0,len)); //回传数据
                    informationStream(std::move(*client_socket)); //继续接受数据
                }
                // std::cout << "收到来自 " << client_socket->remote_endpoint() << " 的数据长度为 : " << len 
                // << " 数据 : " << _data->substr(0,len) << std::endl;
                // //回复客户端数据
                // responseInformation(*client_socket,_data->substr(0,len)); //回传数据
                // informationStream(std::move(*client_socket)); //继续接受数据
            }
        }; //异步执行
        client_socket->async_read_some(boost::asio::buffer(*_data),process_stream);
    }
    public:
    server(boost::asio::io_context& context,uint16_t port)
    :_acceptor(context,tcp::endpoint(tcp::v4(),port))
    {
        _thread = con::make_lightweight_pool(10);
        _thread->start();
        html_data html_data(std::string("Foundation.html"));
        if(html_data.read())
        {
            _html = html_data._html_data;
        }
        std::cout << "当前tcp服务器端口号是 : " << port << std::endl;
    }
    void accptor()
    {
        auto func = [this](boost::system::error_code s,tcp::socket socket)
        {
            processInformation(std::move(socket),s); 
        }; //异步执行
        _acceptor.async_accept(func);
    }
    void processInformation(tcp::socket&& socket,boost::system::error_code s)
    {
        if(!s)
        {
            auto socket_ptr = std::make_shared<tcp::socket>(std::move(socket));
            auto task = [this,socket_ptr]()
            {
                informationStream(std::move(*socket_ptr));
            };
            _thread->submit(task); //调用线程池来执行任务
        }
        else
        {
            std::cout << "客户端连接失败" << s.message() << std::endl;
        }
        accptor();
    }
};