#include <coroutine>
#include <generator>
#include <iostream>
#include <iterator>
#include <format>
#include <vector>
#include <string>
#include <flat_map>


std::generator<std::string> generate_strings()
{
    co_yield "Hello";
    co_yield "World";
    co_yield "!";
}
int main()
{
    auto strings = generate_strings();
    std::cout << std::format("next({})",generate_strings()) << std::endl;
    for(const auto& value : generate_strings())
    {
        std::cout << value << std::endl;
    }
    if consteval  {
        std::cout << "consteval" << std::endl;
    }
    std::unreachable();
}
