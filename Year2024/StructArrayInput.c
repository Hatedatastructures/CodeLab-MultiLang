#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_MAX 10 /* 数组长度 */
#define SUM_MAX 10 /* 结构体数量 */

typedef struct st 
{
    int a;
    int b;
    char c;
    char d[]; /* 柔性数组成员 */
} stu;

int main() {
    stu *s[SUM_MAX]; // 使用指针数组存储每个结构体的地址

    // 为每个结构体实例分配内存
    for (int i = 0; i < SUM_MAX; i++) 
    {
        s[i] = (stu *)malloc(sizeof(stu) + STR_MAX * sizeof(char)); // 为结构体和柔性数组分配内存
        if (s[i] == NULL) 
        {
            printf("内存分配失败\n");
            // 释放已分配的内存
            for (int j = 0; j < i; j++) 
            {
                free(s[j]);
            }
            return 1;
        }

        // 初始化结构体成员
        s[i]->a = i;
        s[i]->b = i + 1;
        s[i]->c = 'a' + i;
        for (int j = 0; j < STR_MAX; j++) 
        {
            s[i]->d[j] = 'a' + j; // 初始化柔性数组
        }
    }

    // 输出结构体内容
    for (int i = 0; i < SUM_MAX; i++) 
    {
        printf("第%2d项：%d %d %c\n", i + 1, s[i]->a, s[i]->b, s[i]->c);
    }

    // 输出柔性数组内容
    for (int i = 0; i < SUM_MAX; i++) 
    {
        for (int j = 0; j < STR_MAX; j++) 
        {
            printf("%c", s[i]->d[j]);
        }
        printf("\n");
    }

    // 释放内存
    for (int i = 0; i < SUM_MAX; i++) 
    {
        free(s[i]);
    }

    return 0;
}
