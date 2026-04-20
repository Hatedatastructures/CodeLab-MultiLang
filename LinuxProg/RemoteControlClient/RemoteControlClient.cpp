#include "RemoteControlClient.hpp"
int main()
{
    try
    {
        boost::asio::io_context io_context;
        std::unique_ptr<client> send(new client(io_context, "127.0.0.1", 6779));
        send->start();
        std::thread tmp ([&io_context]{io_context.run();});
        send->send();
        tmp.join();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}