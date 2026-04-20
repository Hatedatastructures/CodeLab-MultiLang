#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
//归并排序算法递归实现
/*
思路：
1.将数组分成两个区间
2.对两个区间进行排序
3.将两个排序好的区间合并成一个有序的数组
*/
void merge_sort(int* arr, int* temp, int left, int right)
{
    if(left >= right)
    {
        //如果left大于等于right，说明只有一个元素，不需要排序
        return;
    }
    int mid = left + (right - left) / 2;
    //将数组分成两个区间

    //进行递归
    merge_sort(arr, temp, left, mid);
    merge_sort(arr, temp, mid + 1, right);

    int begin1 = left, end1 = mid;//第一个区间
    int begin2 = mid + 1, end2 = right;//第二个区间
    int index = left;//temp数组下标
    while(begin1 <= end1 && begin2 <= end2)
    {
        if(arr[begin1] <= arr[begin2])
        {
            temp[index++] = arr[begin1++];
        }
        else
        {
            temp[index++] = arr[begin2++];
        }
        //比较两个区间的元素大小，将小的元素放到temp数组中
    }
    //循环结束条件就是两个区间中有一个区间没有剩余元素了
    while(begin1 <= end1)
    {
        //如果第一个区间还有剩余元素，将其放到temp数组中
        temp[index++] = arr[begin1++];
    }
    while(begin2 <= end2)
    {
        
        //如果第二个区间还有剩余元素，将其放到temp数组中
        temp[index++] = arr[begin2++];
    }
}
void merge_recursion_sort(int* arr, int n)
{
    assert(arr);
    int* temp = (int*)malloc(n * sizeof(int));
    merge_sort(arr, temp, 0, n - 1);
    free(temp);
    temp = NULL;
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
    merge_recursion_sort(arr, N);
    time_t end = clock();
    for(int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("time:%lld\n", end - list);
    free(arr);
    return 0;
}
