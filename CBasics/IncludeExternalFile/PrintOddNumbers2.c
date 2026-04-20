#include <stdio.h>
int ccc = 1;
int wang(int y)
{
    if (y % 2 == 1)
    {
        return y;
    }
    else
        return 0;
}
int main2() // int main函数换成了自定义hai函数
{
    int a = 0;
    while (a <= 1000)
    {
        if (wang(a)) // 判断wang函数返回值是否为真，进而打印
        {
            printf("%d ", a);
        }
        a++;
    }
    return 0;
}
