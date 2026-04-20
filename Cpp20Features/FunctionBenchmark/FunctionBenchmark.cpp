#include <string>
#include <iostream>
#include <format>
#include <functional>
#include <chrono>
#include <thread>

using func = std::function<std::string(std::uint64_t, std::uint64_t)>;

func test_func1()
{
    return [](std::uint64_t a, std::uint64_t b)
    { return std::to_string(a + b); };
}

// int main()
// {
//     std::cout << std::format("{}", test_func1()(1, 2)) << std::endl;
//     return 0;
// }

// int main()
// {
//     // auto start = std::chrono::high_resolution_clock::now();
//     // std::cout << std::format("{}", test_func1()(1, 2)) << std::endl;
//     // auto end = std::chrono::high_resolution_clock::now();
//     // std::chrono::duration<double> duration = end - start;
//     // std::cout << std::format("Time taken: {} seconds", duration.count()) << std::endl;
//     std::cout << "Start\n";
//     std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> duration = end - start;
//     std::cout << std::format("Time taken: {} seconds", duration.count()) << std::endl;
//     std::cout << "2 seconds passed\n";
//     return 0;
// }

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Time taken: " << ns.count() << " nanoseconds\n";
    return 0;
}