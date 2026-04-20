#include <stdio.h>
#include <math.h>
#include <stdlib.h>
/*计算行列式*/
double det(size_t size, double *ranks)
{
    double num = 1.0;
    for (int j = 0; j < size-1; j++)
    {
        int n = j;
        for (int i = n+1; i < size; i++)
        {
            if (fabs(ranks[i*size+j]) > fabs(ranks[n*size+j])) 
            {
                n = i;
            }    
        }

        if (n != j)
        {
            for (int i = j; i < size ; i++)
            {
                double x = ranks[j*size+i];
                ranks[j*size+i] = ranks[n*size+i];
                ranks[n*size+i] = x;
            }            
            num *= -1.0;
        }
        
        double scl = ranks[(j*size)+j];

        if (scl == 0.0)
        {
            return 0.0;
        }
        else 
        {
            num *= scl;
        }

        for (int i = j+1; i < size; i++) 
        {
            ranks[i*size+j] /= scl;
        }

        for(int i = j+1; i < size; i++)
        {
            double scl = ranks[i*size+j];
            for(int k = j+1; k < size; k++)
            {
                ranks[i*size+k] -= ranks[j*size+k]*scl;
            }
        }
    }
    num *= ranks[(size-1)*size+(size-1)];
    return num;
}
int main()
{
    size_t size = 0;
    size_t size1 =0;
    printf("请输入矩阵的行数和列数:");
    if(scanf("%zu %zu", &size, &size1) != 2)
    {
        printf("输入错误\n");
        return 0;
    }
    double* ranks = (double *)malloc(sizeof(double)*size*size1);
    if(ranks == NULL)
    {
        printf("内存分配失败\n");
        return 0;
    }
    //for循环2选一
    for (int i=0;i<size*size1;i++)
    {
        ranks[i] = rand()%5-2;
    }
    // for(size_t i = 0; i < size; i++)
    // {
    //     for(size_t j = 0; j < size1; j++)
    //     {
    //         printf("请输入第%zu行第%zu列的元素:", i+1, j+1);
    //         if(scanf("%lf", &ranks[i*size1+j]) != 1)
    //         {
    //             printf("输入错误\n");
    //             return 0;
    //         }
    //     }
    // }
    printf("矩阵的行列式为:%lf\n",det(size,ranks));
    free(ranks);
    return 0;
}