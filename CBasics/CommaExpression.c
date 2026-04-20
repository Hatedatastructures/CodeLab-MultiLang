//逗号表达式，特点从左往右依次计算
#include <stdio.h>
int main()
{
    int a=66;
    int b=76;
    int c=23;
    int e=0;//e=1748,a=40204,b=40227//
    long h =(e=b*c,a=c*e,b=a+c);//重新初始化e,a,b的值
    printf("%ld\n",h);
    return 0;
}