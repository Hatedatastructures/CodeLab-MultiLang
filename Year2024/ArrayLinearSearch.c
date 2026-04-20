#include <stdio.h>
//找数组下标
//数组少挨个找效率高
//数组多2分查找快
int main()
{
    int arr[]={1,2,3,4,5,6,7,8,9,10};
    int sum = 0;
    int a = 0;
    int b = 0;
    scanf("%d",&sum);
    a=sizeof(arr)/sizeof(arr[0]);
    for (b=0;sum<a;b++)
    {
        if (arr[b]==sum)
        {
            printf("找到了，下标是：%d\n",b);
            break;
        }
    }
    if (sum>10)
    {
        printf("没找到\n");
    }
    return 0;
    

}