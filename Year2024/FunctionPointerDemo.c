#include <stdio.h>
//函数指针

int Add(int x, int y)
{
    return x+y;
}

int main()
{
    int num1 = 20;
    int num2 = 30;
    int sum = 0;
    int pa=0;
    //add==&add
    int (*p)(int, int) = Add;
    //把函数的地址赋值给p，p就是函数指针变量
    sum = Add(num1, num2);
    pa  =(*p)(num1, num2);
    printf("%d\n", pa);
    printf("%d\n", sum);
    return 0;
}