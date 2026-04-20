#include <stdio.h>

int main()
{
    int a,b;
    scanf("a=%d b=%d",&a,&b);
    int tmp =a;//借助第三方变量
    a = b;//重新给变量赋值
    b = tmp;
    printf("a=%d b=%d",a,b);
    return 0;

}