#include <stdio.h>
#include <unistd.h>
int main()
{
    int number = 60;
    while (number)
    {
        printf("%02d\r", number); // 控制输出长度
        fflush(stdout);
        usleep(50000);
        number--;
    }
    return 0;
}
