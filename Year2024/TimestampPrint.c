#include <stdio.h>
#include <time.h>
int main() 
{
    time_t current_time;
    current_time = time(NULL);
    if (current_time == -1) 
    {
        printf("获取时间失败\n");
    } else {
        printf("从1970年1月1日00:00:00 UTC到现在经过的秒数：%d\n", (int)current_time);
        fprintf(stdout,"从1970年1月1日00:00:00 UTC到现在经过的秒数：%ld\n", (long)current_time);
    }
    return 0;
}