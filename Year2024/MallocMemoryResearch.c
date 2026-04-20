#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
//研究malloc函数申请内存的过程
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    long long p = 0;
    void *ptr;
    while (1)
    {
        ptr = malloc(1);
        p++;
        (void)ptr; // 故意不使用返回值，仅测试内存分配上限
        printf("已申请%lld字节的空间\n", p);
    }
    return 0;
}