
#include <stdio.h>
int main()
{
    char mima[30]={0};
    printf("请输入您的密码:\n");
    scanf("%s",mima);


    int ch;//清理缓存区
    while (ch = getchar() != '\n')//字符型只能用单引号
    {
       ;//空语句，清理缓存区，防止后面代码读取到“/n”导致代码错误
    }



    printf("请确认你的密码Y/NO\n");
    char v;
    scanf(" %c",&v);//跳过空白字符 
    if ('Y'==v)
    {
        printf("密码确认通过\n");
    }
    else if ('N'==v)
    {
        printf("密码确认不通过\n");
    }
    else 
    {
        printf("输入错误\n");
    }
    return 0;
}