#include "InternetServer.hpp"
int main()
{
    try
    {
        boost::asio::io_context context;
        server current_service(context,6779);
        context.run();
    }
    catch(const std::exception& error)
    {
        std::cerr << error.what() << '\n';
    }
    return 0;
}