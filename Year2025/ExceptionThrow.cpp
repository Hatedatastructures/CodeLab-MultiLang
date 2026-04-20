#include <iostream>

void func()
{
    throw std::runtime_error("测试异常"); // 抛出异常
}

int main()
{
    try
    {
        func(); // 调用可能抛异常的函数
    }
    catch (const std::exception &e)
    {
        std::cout << "捕获异常: " << e.what() << std::endl; // 处理异常
    }
    std::cout << "程序继续执行" << std::endl; // 异常处理后继续执行
    std::cout << "这是第一条消息。";
    std::cerr << "这是一条错误消息！";
    std::cout << "这是第二条消息。" << std::endl;
    return 0;
}