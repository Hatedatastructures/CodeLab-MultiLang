#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
//malloc函数的正确用法
int main()
{
    int *p = (int *)malloc(sizeof(int) * 10);
    if (p == NULL)
    {
        printf("%s",strerror(errno));
        //strerror函数头文件是<string.h>
        //strerror函数返回一个字符串，该字符串描述了errno所表示的最后一个错误
        //会报错
        printf("申请内存失败\n");
        exit(1);
    }
    for (int i = 0; i < 10; i++)
    {
        p[i] = i;
    }
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", p[i]);
    }
    free(p);
    p = NULL;
    //正确赋值以保证指针指向有效内存
    return 0;
}