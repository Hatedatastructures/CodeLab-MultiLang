#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 使用指针数组，但是先让每个指针指向动态分配的内存空间
    int **pow = (int **)malloc(1000000000 * sizeof(int *));
    if (pow == NULL) 
    {
        // 如果内存分配失败，进行相应处理
        perror("malloc failed");
        return 1;
    }
    for (int i = 0; i < 1000000000; ++i) 
    {
        pow[i] = (int *)malloc(sizeof(int));
        if (pow[i] == NULL) 
        {
            // 如果内存分配失败，进行相应处理，这里简单打印错误并退出循环（可根据实际优化处理逻辑）
            perror("malloc for element failed");
            break;
        }
        *pow[i] = i;
    }
    system("pause");
    return 0;
}