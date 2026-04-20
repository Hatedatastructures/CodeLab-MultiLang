#include <stdio.h>
//求两个数的较大值
//写一个函数求两个整数的较大值

//如：

//输入：10 20

//输出较大值：20
int main()
{
    int a=0;
    int b=0;
    scanf("%d %d",&a,&b);
    if(a>b)
    {
        printf("%d\n",a);
    }
    else
    {
        printf("%d\n",b);
    }
    return 0;
}