#include <stdio.h>
long jc(long k)
{
    if(k==0)
    {
    return 1;
    }
    return k*jc(k-1);
}
double fm()
{
    int i;
    double s=1.0;
    long sum=1;
    for(i=1;i<=6;i++)
    {
        sum+=jc(3*i+1);
        s+=1.0/sum;
    }
    return s;
}
int main()
{
    double result=fm();
    printf("s的值为：%lf\n",result);
    return 0;
}