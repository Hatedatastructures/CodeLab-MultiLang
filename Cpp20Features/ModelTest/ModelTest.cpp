#include "container/container.hpp"
#include "concurrent/container.hpp"
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "network/network.hpp"

using namespace wan;
int main()
{
    wan::scl::string string_val("hello,world");
    std::cout << string_val << std::endl;
    mco::concurrent_map<mco::concurrent_string,mco::concurrent_string> map;
    map.insert({"hello","world"});
    std::cout << map.at(mco::concurrent_string("hello")).c_str() << std::endl;
    boost::asio::io_context io_context;
    wan::business::transponder<boost::beast::http::string_body> transponder(io_context);
    wan::scl::string host("localhost");
    transponder.add_upstream("www.X.com","127.0.0.1",443,true);
    std::shared_ptr<wan::pool::thread_pool> thread_pool = wan::pool::make_performance_pool(20);
    transponder.set_async_executor(thread_pool);
    transponder.set_ssl_insecure_skip_verify(true);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    io_context.run();
    return 0;
}