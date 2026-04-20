#include <stdio.h>
#include <stdlib.h>

//希尔排序
//思路：将数组分成若干个间隔，对每个间隔内的元素进行插入排序，然后逐渐减小间隔，直到间隔为1，此时数组已经有序
//时间复杂度：O(n^1.3~n^2)
void shell_sort(int* arr ,int n)
{
    //for
    if(arr == NULL || n<=0)
    {
        return;
    }
    // 初始化间隔为数组长度的一半，每次循环将间隔减半
    for(int gap = n / 2; gap > 0; gap =(gap/2))
    {
        // 从间隔位置开始遍历数组
        for(int i = gap; i < n; i=i+gap)//从间隔开始小于数组长度
        {
            // j用于记录当前元素的位置，tmp用于暂存当前元素的值
            int j = i;
            int tmp = arr[i];
            // 内层循环，将当前元素与间隔前的元素比较，如果间隔前的元素大于当前元素，则将间隔前的元素继续向下比较
            for(; j>=gap && arr[j-gap]>tmp; j-=gap)
            {
                arr[j] = arr[j-gap];
            }
            // 进不去上面的循环说明当前元素比间隔前的元素小，直接将当前元素放到间隔后的位置
            arr[j] = tmp;
        }
    }
}
void sell_sorts(int* arr ,int n)
{
    //while
    if(arr == NULL || n<=0)
    {
        return;
    }
    
    int gap = n;
    while(gap > 1)
    {
        gap = gap/3 + 1;
        int i = gap;//i是中间数的间隔
        while( i < n )
        {
            int j = i;//j是通过间隔找到的中间数
            int tmp = arr[i];
            while(j-gap>=0 && arr[j-gap]>tmp)
            {
                arr[j] = arr[j-gap];
                j = j - gap;//假如还小就向下继续找
            }
            arr[j] = tmp; //上面的循环如果没进去就是因为arr[j-gap]<=tmp，所以直接把tmp放到j的位置
            i = i+gap;//跨过中间数，继续找下一个中间数
        }
    }
}
int main()
{
    int arr[] = {1,5,3,7,9,2,4,6,8,0,19,11,13,15,17,16,14,12,10};
    int arr1[] = {1,5,3,7,9,2,4,6,8,0,19,11,13,15,17,16,14,12,10};
    shell_sort(arr,sizeof(arr)/sizeof(arr[0]));
    for(int i = 0; i<(int)(sizeof(arr)/sizeof(arr[0])); i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
    sell_sorts(arr1,sizeof(arr1)/sizeof(arr1[0]));
    for(int i = 0; i<(int)(sizeof(arr1)/sizeof(arr1[0])); i++)
    {
        printf("%d ",arr1[i]);
    }
    return 0;
}