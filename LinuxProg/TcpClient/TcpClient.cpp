#include "TcpClient.hpp"
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <chrono>

int main()
{
    SetConsoleCP(65001);
    try
    {
        boost::asio::io_context io_context;
        const std::string server_ip = "124.71.136.228";
        const uint16_t server_port = 6779;
        client client(io_context, server_ip, server_port);
        client.start();
        std::thread _thread([&io_context](){ io_context.run(); });

        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (true)
        {
            std::string msg;
            std::cin >> msg;
            client.sendData(msg);
        }
        _thread.join();
        client.closeConnection();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}