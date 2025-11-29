#include <iostream>
#include <filesystem>
#include <format>
namespace fs = std::filesystem;

int main()
{
    fs::path running_path = fs::current_path();  // 获取当前运行的绝对路径
    std::cout << std::format("当前目录:{}",running_path.string()) << std::endl;
    std::cout << std::format("父目录:{}",running_path.parent_path().string()) << std::endl;
    return 0;
}
