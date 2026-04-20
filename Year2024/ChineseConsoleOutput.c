#include <stdio.h>
#include <windows.h>  // 引入Windows相关的头文件，用于设置控制台编码等操作

int main()
{
    // 设置控制台输出代码页为936（对应GBK编码，用于支持中文显示）
//    SetConsoleOutputCP(936);

    char chinese_str[] = "你好，世界";
    printf("%s\n", chinese_str);

    return 0;
}