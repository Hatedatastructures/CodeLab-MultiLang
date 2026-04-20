//用三目运算符求最大值
#include <stdio.h>
int main()
{
    int a = 90;
    int b = 37;
    int g =(a>b? a:b); //判断如果a>b为真则a是对的b是错的，反之如果a>b为假则a是错的b是对的
    printf("两者最大值为%d\n", g);//简单理解为if else语句，为假则反之
    return 0;
}