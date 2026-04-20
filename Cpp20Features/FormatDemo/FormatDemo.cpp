#include <iostream>
#include <format>
#include "Asio/Model/Concurrent/Container.hpp"
#include "Asio/Model/Container/Container.hpp"
#include <map>
#include <unordered_map>
#include <vector>

// 辅助函数：打印 vector
template <typename T>
void print_vec(const std::vector<T> &vec)
{
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0)
            std::cout << ", ";
        std::cout << vec[i];
    }
    std::cout << "]" << std::endl;
}

// 辅助函数：打印 map
template <typename K, typename V>
void print_map(const std::map<K, V> &m)
{
    std::cout << "{";
    bool first = true;
    for (const auto &[k, v] : m)
    {
        if (!first)
            std::cout << ", ";
        std::cout << k << ": " << v;
        first = false;
    }
    std::cout << "}" << std::endl;
}

// 辅助函数：打印 unordered_map
template <typename K, typename V>
void print_umap(const std::unordered_map<K, V> &m)
{
    std::cout << "{";
    bool first = true;
    for (const auto &[k, v] : m)
    {
        if (!first)
            std::cout << ", ";
        std::cout << k << ": " << v;
        first = false;
    }
    std::cout << "}" << std::endl;
}

int main()
{
    {
        wan::scl::string str = {"hello"};
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::map<int, std::string> map = {{1, "one"}, {2, "two"}, {3, "three"}};
        std::unordered_map<std::string, int> umap = {{"one", 1}, {"two", 2}, {"three", 3}};
        std::string s = "hello";

        // 手动打印容器（std::format 和 operator<< 均不支持标准容器）
        print_vec(vec);
        print_map(map);
        print_umap(umap);
        std::cout << str << std::endl;
        std::cout << std::format("{}", s) << std::endl;

        auto it = umap.begin();
        std::advance(it, 1);
        std::cout << std::format("{}", it->second) << std::endl;
        it = umap.begin();
        std::advance(it, 2);
        std::cout << std::format("{}", it->second) << std::endl;
    }
    return 0;
}
