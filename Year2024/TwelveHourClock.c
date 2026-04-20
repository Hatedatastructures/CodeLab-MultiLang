#include <stdio.h>
#include <windows.h>
//打印12小时制时间
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    int hour , minute;
    int str =0;
    printf("输入24小时制时间:->\n");
    str = scanf("%d:%d",&hour,&minute);
    if(str==2)
    {
        if(hour<=24 && hour>=0 && minute>=0 && minute<=60)
        {
            if (hour<12)
            {
                printf("AM %d:%d\n",hour,minute);
            }
            else
            {
                printf("PM %d:%d\n",hour-12,minute);
            } 
        }
        else
        {
            printf("输入非法！\n");
        }
    }
    else
    {
        printf("读取错误！\n");
    }
    return 0;
}