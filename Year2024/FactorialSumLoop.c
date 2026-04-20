#include <stdio.h>

long jc(int K) 
{
    long l =0;
    if (K == 1|| K==0) 
    {
        return 1;
    }
    else 
    {
        l =K * jc(K - 1);
        printf("当前阶乘结果为：->%ld\n",l);
        return l;
    }
}

double fm()
{
    double S = 1.0; 
    double man = 1; 
    for (int i = 4; i <= 19; i += 3) 
    {
        man += jc(i); 
        printf("当前阶乘数：%d,当前结果：%lf",i,man);
        S += 1.0 / man;
        printf("计算的分数：%.20lf\n",S);
    }
    return S;
}
int main() 
{
    double result=fm();
    printf("s的值为：%.20lf\n",result);
    return 0;
}