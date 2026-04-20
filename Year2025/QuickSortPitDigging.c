#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//快速排序算法递归挖坑法
/*
思想：
先找一个基准数，保存基准数，把基准数的位置拿掉当一个坑
再遍历数组，如果它比基准数小，那就把它放到数组左边填这个坑
如果比基准数大则把他放到数组右边填这个坑，这个循环走完，基准数的位置就确定了
*/
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int Three_middle(int*a , int left , int right)
{
    //在三个数中找到中间值，以此来优化快速排序
    int mid = (left + right) / 2;//取中间值
    if(left < right)
    {
        // 如果左边界值大于中间值，返回左边界索引
        if(a[left] > a[mid])
        {
            return left;
        }
        if(a[mid] > a[right])
        {
            return right;
        }
        return mid;
    }
    else if(left > right)
    {
        if(a[left] < a[mid])
        {
            return left;
        }
        if(a[mid] < a[right])
        {
            return right;
        }
        return mid;
    }
    return left;//如果left == right,则返回left或者right都可以
}
void insert_sort(int *arr, int len)//插入排序
{
    int temp = 0;
    for(int i = 1; i < len; i++)
    {
       temp = arr[i];
       for(int j = i - 1; j >= 0; j--)
       {
           if(temp < arr[j])
           {
               arr[j + 1] = arr[j];
           }
           else
           {
               arr[j + 1] = temp;
               break;
           }
           //先从第二个比较，如果小于第一个，则将第一个赋值给第二个，再比较第一个和第三个，如果小于第三个，则将第三个赋值给第二个，以此类推
           //如果大于，则将temp赋值给j+1位置，然后跳出循环
       }
    }
}
int single(int*a , int left, int right)
{
    int mid = Three_middle(a , left , right);
    swap(&a[mid] , &a[right]);//将中间值放到最右边,把最坏情况变成最好情况，使得第一次排序不在无效，减少递归层数
    //从左往右
    int key = a[left];//key这个数的位置当作坑
    int keylists = left;
    while(left < right)
    {
        while(left < right && a[right] >= key)
        {
            right--;
            //结束条件a[right] 小于 key
        }
        a[keylists] = a[right];//将右边小于key的数放到左边，填坑
        while(left < right && a[left] <= key)
        {
            left++;
            //结束条件a[left] 大于 key
        }
        a[keylists] = a[left];//将左边大于key的数放到右边，填坑
        //和左右指针法一样，只不过把交换的数变成了填坑，左右指针是两个交换，填坑法是一个交换，属于是分开了
    }
    //这个最外层循环走完也就代表除了keylists这个数，两边不是比key大的就是比key小的
    swap(&a[keylists] , &a[left]);//key这个数找到了自己合适的位置
    return left;//返回已经找好位置数的下标
}
//乱序最快，有序最慢
void Quick_recursion_sort(int*a , int left , int right)
{
    if((right - left) > 10)////如果数组长度大于10，则使用快速排序，否则使用插入排序
    {
        int key = single(a , left , right);
        Quick_recursion_sort(a , left , key - 1);//根据返回的下标进行递归
        Quick_recursion_sort(a , key + 1 , right);
    }
    else
    {
        insert_sort(a + left, right - left + 1);
    }
    //递归层数太深容易栈溢出
}
int main()
{
    int N = 10000;
    int* arr = (int*)malloc(N * sizeof(int));
    for(int i = N; i >= 0; i--)
    {
        arr[i] = i;
    }
    
    time_t list =clock();
    //快速排序
    Quick_recursion_sort(arr, 0, N - 1);
    time_t end = clock();
    for(int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("time:%lld\n", end - list);
    free(arr);
    return 0;
}
