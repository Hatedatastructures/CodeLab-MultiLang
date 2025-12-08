#include "CoroutineLog/CoroutineLog.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

namespace asio = boost::asio;

// asio::awaitable<void> run_perf(performance_log::coroutine_log& logger)
// {
//     const std::string path = "perf_test.log";
//     const std::size_t batch_count = 1000;       // 批次数量
//     const std::size_t lines_per_batch = 500;   // 每批行数
//     const std::size_t line_len = 64;           // 每行长度（近似）

//     std::vector<std::string> lines;
//     lines.reserve(lines_per_batch);
//     for (std::size_t i = 0; i < lines_per_batch; ++i)
//     {
//         std::string s;
//         s.reserve(line_len);
//         s = "perf line ";
//         s += std::to_string(i);
//         s.append(" ");
//         s.append(48, 'x');
//         s.append("\n");
//         lines.emplace_back(std::move(s));
//     }

//     auto t0 = std::chrono::steady_clock::now();
//     std::size_t total_bytes = 0;
//     for (std::size_t b = 0; b < batch_count; ++b)
//     {
//         total_bytes += co_await logger.file_write(path, lines);
//     }
//     auto t1 = std::chrono::steady_clock::now();
//     auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

//     std::cout << "perf batches=" << batch_count
//               << " lines_per_batch=" << lines_per_batch
//               << " total_bytes=" << total_bytes
//               << " time_ms=" << ms
//               << std::endl;
//     co_return;
// }

// int main()
// {
//     std::shared_ptr<asio::io_context> io = std::make_shared<asio::io_context>();
//     performance_log::coroutine_log logger(io->get_executor());


//     asio::co_spawn(*io, [&]() -> asio::awaitable<void>
//     {
//         co_await run_perf(logger);
//         co_return; 
//     }, asio::detached);

//     // std::vector<std::jthread> threads;
//     // for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
//     // {
//     //     threads.emplace_back([&]() { io->run(); });
//     // }


//     std::jthread t1([&]() { io->run(); });
//     return 0;
// }
#include "CoroutineLog/CoroutineLog.hpp"
#include <string>
int main()
{
    asio::io_context io;
    performance_log::coroutine_log logger(io.get_executor());
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    boost::asio::co_spawn(io, [&]() -> asio::awaitable<void>
    {
        // std::vector<std::string> logstrs;
        std::size_t total_bytes = 0;
        for (int i = 0; i < 10000; ++i)
        {
            total_bytes += co_await logger.file_write("test.log", performance_log::level::info,performance_log::timestamp() + "hello world " + std::to_string(i) + "\n");
            co_await logger.console_write(performance_log::level::info, "hello world " + std::to_string(i) + "\n");
            // logstrs.emplace_back("hello world" + std::to_string(i) + "\n");
        }
        std::cout << "total_bytes=" << total_bytes << std::endl;
        // co_await logger.file_write("test.log", logstrs);
        co_return;
    }, asio::detached);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "time_ms=" << ms << std::endl;
    std::jthread t([&]() { io.run(); });

    // asio::io_context io;
    // namespace log = performance_log;
    // log::initialize(io.get_executor());
    // asio::co_spawn(io, [&]() -> asio::awaitable<void>
    // {
    //     // co_await log::FileLogMassage(log::level::info, "{}\n", "静态初始化完成");
    //     co_await log::FileLogMassage("test.log", "[{}][{}]\n", std::chrono::system_clock::now(), "静态初始化完成");
    //     co_await log::FileLogMassage("test.log", "[{}][{}]\n", std::chrono::system_clock::now(), "进入main函数");
    //     co_return;
    // }, asio::detached);
    // std::jthread t([&]() { io.run(); });
    // io.stop();
    // log::shutdown();
    // std::this_thread::sleep_for(std::chrono::seconds(20));

    // std::cout << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now()) << std::endl;
    return 0;
}