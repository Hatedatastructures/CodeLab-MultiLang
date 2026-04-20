#include <stdio.h>
#include <windows.h>
int main()
{
    int number = 60;
    while (number)
    {
        printf("%02d\r", number); // 控制输出长度
        fflush(stdout);
        Sleep(1000);
        number--;
    }
    return 0;
}