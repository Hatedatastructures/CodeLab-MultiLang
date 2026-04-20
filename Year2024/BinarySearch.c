#include <stdio.h>
//2分查找效率高但是需要排序
int main()
{
    int arr[]={1,2,3,4,5,6,7,8,9,10};//1的下标是0，10的下标是9
    int sum=0;
    int n = 0;
    scanf("%d",&n);
    sum=sizeof(arr)/sizeof(arr[0]);//求出数组元素个数
    int left=0;
    int right=sum-1;//求出最后一位元素的下标
    
    while (left<=right)//left小于等于right说明两个下标之间还有元素
    {
     int mid=(left+right)/2;//mid定义的是中间数，定义mid如果在外面则会死循环
     if (arr[mid]<n)//mid小于n说明离要找元素在右边，所以mid+1
     {
        left=mid+1;
     }
     else if (arr[mid]>n)//mid大于n说明元素在左边，mid-1
     {
        right=mid-1;
     }
     else//最后一种情况下标刚好指向要找的元素，直接输出
     {
        printf("找到了，下标是：%d\n",mid);
        break;//找到了就直接跳出循环，防止死循环
     }
    }
    if (left>right)//没有元素符合arr【】这个数组
    {
        printf("没找到\n");
    }
    
    return 0;
}