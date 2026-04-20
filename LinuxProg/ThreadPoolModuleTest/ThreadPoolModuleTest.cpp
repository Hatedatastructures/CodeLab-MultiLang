#include "ThreadPool.hpp"

int main()
{
    auto pool = con::make_lightweight_pool(12);
    pool->start();
    auto func = [](const std::string& first,const std::string& second)
    {
        std::cout << first << " " << second << std::endl;
        return first + second;
    };
    auto value = pool->submit(func, "hello", "world");
    std::cout << value.get() << std::endl;
    auto func2 = [](int a, int b)
    {
        std::cout << a * b << std::endl;
    };
    auto value2 = pool->submit(func2, 1, 2);
    // std::cout << value2.get() << std::endl;
    return 0;
}