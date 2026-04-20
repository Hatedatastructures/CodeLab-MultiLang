#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

//计算C语言运行效率以及获取当前时间
int main()
{
    srand(time(0));
    uint64_t i = clock();
    for (uint64_t j = 0; j < 100000000; j++)
    {
        ;
    }
    
    uint64_t k = clock();
    printf("C语言运行时间为：%llums\n", k - i);
    time_t t = time(0);
    printf("当前时间为：%lld", t);
    char* time_str = ctime(&t);
    printf("当前时间为：%s", time_str);
    struct tm* local_time = localtime(&t);
    printf("当前时间为：%d年%d月%d日%d时%d分%d秒", 
        local_time->tm_year + 1900, 
        local_time->tm_mon + 1, 
        local_time->tm_mday, 
        local_time->tm_hour, 
        local_time->tm_min, 
        local_time->tm_sec);
    return 0;
}