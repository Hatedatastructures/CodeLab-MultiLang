#include "HttpServerApp.hpp"

#include <iostream>
#include <thread>
#include <filesystem>

int main()
{
    boost::asio::io_context io_context;
    server server(io_context, 8080);
    server.set_web_root((std::filesystem::path(__FILE__).parent_path() / "webroot").string());
    server.start();
    auto io_function = [&io_context]()
    {
        io_context.run();
        std::cout << "io_context.run() finished" << std::endl;
    };
    std::jthread thread(io_function);

    return 0;
}
