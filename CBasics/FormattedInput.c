#include <stdio.h>
int main()
{
    int a, b, c;
    double d;
    scanf("%04d %02d %02d %.3f", &a, &b, &c, &d);   // 通过判断输入字符长度
    printf("nian=%04d yuer=%02d re=%02d", a, b, c); // 从而输出

    return 0;
}