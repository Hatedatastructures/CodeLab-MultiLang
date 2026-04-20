#include <stdio.h>
#include <windows.h>
int main()
{
    int  yuhaiwei=0;//初始化
    printf("wan yuan shen \n");//先输出玩原神
    while (yuhaiwei<=9999999)//循环小于9999999次输出牢大
    {
       printf("牢大:%d\n", yuhaiwei);//自增运算
       yuhaiwei++;
    }
    if(yuhaiwei>=9999999)//判断大于9999999次时输出
    {
        printf("玩原神玩的\n");
    }
    return 0;//返回
}