#include <stdio.h>
int wang(int x, int y) // 定义了一个整形结果的函数//wang是函数名
{
    int z = x + y; // 简化的是直接 return(x+y);
    return z;      // 返回结果z
}

int haiwei(int d, int z) // 定义乘法函数
{
    int f = d * z;
    return f; // 略
}
int main()
{
    int a = 0;
    int b = 0;
    scanf("%d %d", &a, &b);         // 输入
    int sum = wang(a, b);           // 调用wang函数，把a，b值代入
    int niu = haiwei(a, b);         // 调用haiwei函数，把a，b值代入
    printf(" %d\n %d\n", sum, niu); // 输出
    return 0;
}