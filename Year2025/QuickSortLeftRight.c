#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
//快速排序算法左右指针法
//时间复杂度O(n*    logn)
/*
思想：
先找一个基准值，遍历数组把比基准值小的放到左边，比基准值大的放到右边，
这样就找到了基准值的位置，然后递归的把基准值左边的数组和右边的数组进行同样的操作
*/
void swap(int*a , int*b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
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
int single(int*a , int left, int right)
{
    int mid = Three_middle(a , left , right);
    swap(&a[mid] , &a[right]);//将中间值放到最右边,把最坏情况变成最好情况，使得第一次排序不在无效，减少递归层数

    int key = a[left];
    int keylists = left;
    while(left < right)
    {
        while(left < right && a[right] >= key)
        {
            right--;
        }
        while(left < right && a[left] <= key)
        {
            left++;
        }
        //这两循环走完就是left找到了第一个比key大的数，right找到了第一个比key小的数，正好交换一下
        //或者者left和right相遇了，也就代表找到了key的位置
        swap(&a[left] , &a[right]);
        //这里交换完后，就除了基准数位置不对，其他的数分成立两个区间，一个区间比基准数小，一个区间比基准数大
    }
    swap(&a[keylists] , &a[left]);//key这个数找到了自己合适的位置
    return left;//返回已经找好位置数的下标
}
//乱序最快，有序最慢
void Quick_recursion_sort(int*a , int left , int right)
{
    if((right - left > 10))//如果数组长度大于10，则进行快速排序，否则进行插入排序
    {
        int key = single(a , left , right);
        Quick_recursion_sort(a , left , key - 1);//根据返回的下标进行递归
        Quick_recursion_sort(a , key + 1 , right);
    }
    else
    {
        insert_sort(a + left , right - left + 1);
    }
    //递归层数太深容易栈溢出
}
int main()
{
    time(NULL);
    srand(time(NULL));
    int N = 10000;
    int* num = (int*)malloc(sizeof(int) * N);
    int* arr = (int*)malloc(sizeof(int) * N);
    for (int i = N ; i >= 0 ; i--)
    {
        arr[i] = rand();
    }
    for(int i = N ; i >= 0 ; i--)
    {
        num[i] = i;
    }
    
    time_t last = clock();
    Quick_recursion_sort(arr , 0 , N - 1);
    time_t now = clock();
    for(int i = 0 ; i < N ; i++)
    {
        printf("%d ",arr[i]);
    }
    time_t last1 = clock();
    Quick_recursion_sort(num , 0 , N - 1);
    time_t now1 = clock();
    printf("乱序时间 = %zu\n",now - last);
    printf("降序有序时间 = %zu\n",now1 - last1);
    free(arr);
    free(num);
    return 0;
}   