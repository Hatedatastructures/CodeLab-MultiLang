#include <stdio.h>
int wang(int x, int y) // 定义函数
{
    return (x > y ? x : y);
    // 如果 x 大于 y，则条件为真（true），此时整个表达式的结果为 x。
    // 如果 x 不大于 y（即小于或等于），则条件为假（false），此时整个表达式的结果为 y
}
int main()
{
    int a = 0;
    int b = 0;
    scanf("%d %d", &a, &b);
    int sum = wang(a, b); // 调用函数前首先要定义一个储存结果的值
    printf("%d", sum);
    return 0;
}