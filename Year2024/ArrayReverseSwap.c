#include<stdio.h>

void dx(int* p,int m)
{
    int n =( m/ 2),temp=0;
    for (int i = 0; i < n; i++)
    {
        printf("正在交换第%d个和第%d个元素\n",i,m-1-i);
        temp = *(p + i);
        *(p + i) = *(p + m - 1 - i);
        *(p + m - 1 - i) = temp;
    }
}

int main()
{
    void dx(int* p,int m);
    int a[10] = {},i;
    printf("请输入十个数据:\n");
    for (i = 0; i < 10; i++)
    {
        printf("a[%d]=",i);
        scanf("%d",&a[i]);
    }
    printf("\n");
    int* p = a,n=10;
    dx(p,n);
    printf("逆向排序后的数据为:\n");
    for (; p < a+10; p++)
        printf("%d ", *p);
    return 0;
}