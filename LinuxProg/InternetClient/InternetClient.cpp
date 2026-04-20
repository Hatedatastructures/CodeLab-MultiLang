#include "InternetClient.hpp"
int main()
{
    try
    { //-lws2_32
        boost::asio::io_context _service;
        client test(_service,"124.71.136.228", 6779);
        _service.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}