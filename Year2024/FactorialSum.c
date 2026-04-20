#include <stdio.h>
#include <windows.h>
//计算位阶
int main()
{
    int a =1;
    int sum =1;
    int c =0;
    int n =0;
    printf("位阶计算\n");
    scanf("%d",&c);
    for (a=1;a<=c;a++)
    {
        sum =sum*a;
        n=n+sum;
        printf("%d\n",sum);
    }
    return 0;
}

