//指针
#include <stdio.h>
int main()
{
    int num = 10;//num赋值
    int* p = &num;//创建p变量用来储存num的地址
    *p=20;//*p是解地址操作符,就是通过p的地址找到p所指向的对象，*p就是p锁指向的对象
    printf("num=%d\n", num);
    return 0;

}

