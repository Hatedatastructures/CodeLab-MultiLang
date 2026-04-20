#include <stdio.h>
#include <windows.h>
#define N 10
#define M 10
//杨辉三角
int main()
{
    int a[N][M] = {0};
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j <= i; j++)
        {
            if(j == 0 || j == i)
            {
                a[i][j] = 1;
            }
            else
            {
                a[i][j] = a[i-1][j-1] + a[i-1][j];
            }
        }
    }
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j <= i; j++)
        {
            printf("%3d ", a[i][j]);
        }
        printf("\n");
    }
    system("pause");
    return 0;
}