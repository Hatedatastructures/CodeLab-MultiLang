#include "CoroutineLog/CoroutineLog.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

namespace asio = boost::asio;

asio::awaitable<void> run_perf(CoroutineLog::coroutine_log& logger)
{
    const std::string path = "perf_test.log";
    const std::size_t batch_count = 1000;       // 批次数量
    const std::size_t lines_per_batch = 500;   // 每批行数
    const std::size_t line_len = 64;           // 每行长度（近似）

    std::vector<std::string> lines;
    lines.reserve(lines_per_batch);
    for (std::size_t i = 0; i < lines_per_batch; ++i)
    {
        std::string s;
        s.reserve(line_len);
        s = "perf line ";
        s += std::to_string(i);
        s.append(" ");
        s.append(48, 'x');
        s.append("\n");
        lines.emplace_back(std::move(s));
    }

    auto t0 = std::chrono::steady_clock::now();
    std::size_t total_bytes = 0;
    for (std::size_t b = 0; b < batch_count; ++b)
    {
        total_bytes += co_await logger.file_write(path, lines);
    }
    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::cout << "perf batches=" << batch_count
              << " lines_per_batch=" << lines_per_batch
              << " total_bytes=" << total_bytes
              << " time_ms=" << ms
              << std::endl;
    co_return;
}

int main()
{
    std::shared_ptr<asio::io_context> io = std::make_shared<asio::io_context>();
    CoroutineLog::coroutine_log logger(io->get_executor());


    asio::co_spawn(*io, [&]() -> asio::awaitable<void>
    {
        co_await run_perf(logger);
        co_return; 
    }, asio::detached);

    // std::vector<std::jthread> threads;
    // for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    // {
    //     threads.emplace_back([&]() { io->run(); });
    // }


    std::jthread t1([&]() { io->run(); });
    return 0;
}
// #include "CoroutineLog/CoroutineLog.hpp"
// #include <string>
// int main()
// {
//     asio::io_context io;
//     CoroutineLog::coroutine_log logger(io.get_executor());
//     boost::asio::co_spawn(io, [&]() -> asio::awaitable<void>
//     {
//         for (int i = 0; i < 100; ++i)
//         {
//             co_await logger.file_write("test.log", "hello world " + std::to_string(i) + "\n");
//         }
//         co_return; 
//     }, asio::detached);
//     io.run();
//     return 0;
// }