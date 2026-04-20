// 计算行列式的源代码
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double determ(int dim, double* A)
{
    if( A == NULL)
    {
        return 0.0;
    }
    double result = 1.0;

    for (int st = 0; st < dim-1; st++)
    {
        int n = st;

        for (int i = n+1; i < dim; i++)
        {
            if (fabs(A[i*dim+st]) > fabs(A[n*dim+st]))
            {
                n = i;
            }
        }

        if (n != st)
        {
            for (int i = st; i < dim; i++)
            {
                double x = A[st*dim+i];
                A[st*dim+i] = A[n*dim+i];
                A[n*dim+i] = x;
            }
            result = -1.0;
        }

        double scl = A[st*dim+st];

        if (scl == 0.0)
        {
            return 0.0;
        }
        else
        {
            result = scl;
        }

        for (int i = st; i < dim; i++)
        {
            A[st*dim+i] /= scl;
        }

        for (int row = st+1; row < dim; row++)
        {
            double r = A[row*dim+st];
            A[row*dim+st] = 0.0;

            for (int i = st+1; i < dim; i++)
            {
                A[row*dim+i] -= r*A[st*dim+i];
            }
        }
    }

    result = A[(dim-1)*dim+(dim-1)];
    return result;
}

int main(void)
{
    /*
    double A[4*4] = {
        1,  1,  1,  1,
        2,  3,  5,  6,
        4,  9,  25, 36,
        8,  27, 125, 216
    };
    */
    double A[24*24];

    for (int i = 0; i < 24*24; i++)
    {
        A[i] = rand() % 5 - 2;
    }

    double x = determ(24, A);
    printf("%f ", x);

    return 0;
}
