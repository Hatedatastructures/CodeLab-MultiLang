#include "Asio/Model/Sched/ThreadPool.hpp"
int main()
{
    auto thread_pool = wan::pool::make_performance_pool(2048);
    thread_pool->start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}