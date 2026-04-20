#include <stdio.h>
#include <stdlib.h>
int main() 
{
    size_t size;
    if (scanf("%zu", &size)!= 1) 
    {
        printf("输入错误\n");
        return 1;
    }
    if(size == 0)
    {
        printf("0\n");
        return 0;
    }
    size_t *num = (size_t *)malloc(size * sizeof(size_t));
    if (num == NULL) 
    {
        printf("内存分配失败\n");
        return 1;
    }
    for (size_t i = 0; i < size; i++) 
    {
        if (scanf("%zu", &num[i])!= 1) 
        {
            printf("输入错误\n");
            free(num);
            return 1;
        }
    }
    size_t ans = 0;
    for (size_t i = 0; i < size; i++) 
    {
        size_t sum = 0;
        for (size_t j = i; j < size; j++) 
        {
            sum = sum + num[j];
            ans = ans ^ sum;
        }
    }
    printf("%zu\n", ans);
    free(num);
    return 0;
}