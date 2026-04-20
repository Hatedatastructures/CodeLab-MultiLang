#include <ranges>
#include <iostream>
#include <vector>
#include <format>
#include <map>

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5}; // 基于范围的打印，迭代器思维
    std::map<std::string, std::string> usermap = {{"username", "hello world"}, {"age", "100"}};
    auto even = vec | std::views::filter(
                          [](int x)
                          { return x % 2 == 0; });
    auto user = std::views::filter(usermap,
                                   [](const auto &pair)
                                   { return pair.first == "username"; });
    auto it = user.begin()->second;
    std::cout << std::format("{}", it) << std::endl;
    std::cout << *std::next(even.begin()) << std::endl;
    for (auto x : even)
    {
        std::cout << x << " ";
    }
    return 0;
}