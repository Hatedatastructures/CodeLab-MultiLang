#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 调用系统命令关机，这里的参数 /s 表示关机，/t 0表示立即关机
    system("shutdown /s /t 60");
    return 0;
}