#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

std::mutex consoleMutex;

void createLogWindow(const std::string& title, const std::string& logContent) 
{
    // 创建新的控制台窗口
    AllocConsole();
    SetConsoleTitleA(title.c_str());
    
    // 重定向标准输出
    freopen("CONOUT$", "w", stdout);
    
    // 输出日志内容
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::cout << logContent << std::endl;
    
    // 这里可以添加更多日志输出逻辑
}

int main() 
{
    // 创建多个日志窗口
    std::thread logWindow1(createLogWindow, "日志窗口1", "这是窗口1的日志内容...");
    std::thread logWindow2(createLogWindow, "日志窗口2", "这是窗口2的日志内容...");
    std::thread logWindow3(createLogWindow, "错误日志", "错误信息: 文件未找到!");
    
    logWindow1.join();
    logWindow2.join();
    logWindow3.join();
    system("pause");
    return 0;
}