#include "RemoteControlServer.hpp"

int main()
{
    try
    {
        boost::asio::io_context context;
        std::unique_ptr<server> remote_server(new server(context,6779));
        remote_server->start();
        context.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}