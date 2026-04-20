#include <stdio.h>
#define ADD(x, y) ((x) + ((y))) // 定义宏
#define MAX 90                  // 定义不用分号结尾和赋值
int yaoxingyu = 9;
int main()
{
    int a = 89;
    int b = 45;
    int sum = a + b;
    int wang = ADD(a, b); // 用法和函数类似,因为是数值所以要用int接住定义的返回值
    printf("%d\n", sum);
    printf("%d\n", wang);
    printf("%ld\n", MAX);
    return 0;
}
