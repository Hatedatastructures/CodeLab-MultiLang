#include "ProgressBarHelper.h"

void progress_bar_printf()
{
    char buffer[102];  //防止溢出
    memset(buffer, ' ', 100);
    buffer[100] = '\0';
    for (int i = 0; i <= 100; i++)
    {
        if (i > 0) 
        {
            buffer[i-1] = '#';  
            // 将前一个空格替换为#,只需要打印这个数组就行
        }
        printf("\033[0m\033[1;31m[%-100s] [%-3d%%]\033[0m\r", buffer, i);
        fflush(stdout);
        Sleep(10);
    }
    printf("\n");
}
void printf_red(const char *s)
{
    printf("\033[0m\033[1;31m%s\033[0m", s);
}

void printf_green(const char *s)
{
    printf("\033[0m\033[1;32m%s\033[0m", s);
}

void printf_yellow(const char *s)
{
    printf("\033[0m\033[1;33m%s\033[0m", s);
}

void printf_blue(const char *s)
{
    printf("\033[0m\033[1;34m%s\033[0m", s);
}

void printf_pink(const char *s)
{
    printf("\033[0m\033[1;35m%s\033[0m", s);
}

void printf_cyan(const char *s)
{
    printf("\033[0m\033[1;36m%s\033[0m", s);
}