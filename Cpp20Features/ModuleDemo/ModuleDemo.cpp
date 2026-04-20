// 导入自定义模块
import mymodule;
// 导入标准库模块（GCC 对标准库模块支持有限，目前建议仍用 #include）
#include <iostream>
// import std;

int main()
{

    std::cout << add(2, 3) << std::endl; // 调用模块导出的函数

    std::cout << Calculator::multiply(2, 3) << std::endl; // 调用模块导出的类
    return 0;
}