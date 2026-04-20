#include<stdio.h>
#define MAX 4
#define MIN 4

int  main()
{
    int b[MAX][MIN] = {};
    int a[MAX][MIN] = {};
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MIN; j++)
        {
            printf("请输入第%d行第%d列的数：", i + 1, j + 1);
            scanf("%d", &a[i][j]);
        }
    }
    /*对角线*/
    for(int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MIN; j++)
        {
           if(i == j)
           {
               b[i][j] = a[i][j];
           }
           else
           {
               b[i][j] = a[j][i];
           }
        }
    }
    printf("原矩阵为：\n");
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MIN; j++)
        {
            printf("%2d ", a[i][j]);
        }
        printf("\n");
    }
    printf("转置后的:\n");
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MIN; j++)
        {
            printf("%2d ", b[i][j]);
        }
        printf("\n");
    }
    return 0;
}
