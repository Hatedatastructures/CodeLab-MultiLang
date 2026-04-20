#include <stdio.h>
int main()
{
    int haiwei;
    printf("玩原神!\n");
    printf("不玩原神?\n");
    printf("选0或1\n");
    scanf("%d", &haiwei);
    if (haiwei == 0) // 如果  //可换成haiwei==1,结果为1是正确结果
    {
        printf("玩原神玩的\n");
    }
    else // 还
    {
        printf("正常人\n");
    }

    return 0;
}
