#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
//归并排序非递归实现
/*
思路：
1. 将数组分成若干个步长为1的子数组，然后两两合并
2. 合并时，将两个子数组合并成一个有序数组
3. 重复步骤2，直到整个数组有序
*/
void merge_sort(int* arr, int* temp, int left, int mid, int right)
{
    assert(arr);
    assert(temp);
    int i = left;
    int j = mid + 1;
    int k = left;

    // 比较左右子数组元素，将较小的放入临时数组
    while (i <= mid && j <= right) 
    {
        if (arr[i] <= arr[j]) 
        {
            temp[k++] = arr[i++];
        } 
        else 
        {
            temp[k++] = arr[j++];
        }
    }

    // 处理左子数组剩余元素
    while (i <= mid) 
    {
        temp[k++] = arr[i++];
    }

    // 处理右子数组剩余元素
    while (j <= right) 
    {
        temp[k++] = arr[j++];
    }

    // 将临时数组元素复制回原数组
    for (i = left; i <= right; i++) 
    {
        arr[i] = temp[i];
    }
}
void merge_NO_recursion_sort(int*arr, int n)
{
    assert(arr);
    int* temp = (int*)malloc(sizeof(int) * n);
    assert(temp);
    for(int gap = 1; gap <= n; gap = gap * 2)
    //gap为步长，每次步长翻倍
    {
        for(int i = 0; i < n; i = i + 2 * gap)
        //i为左子数组起始位置
        {
            int left = i;
            int mid = left + gap - 1;
            int right = mid + gap;
            if(mid > n - 1)
            {
                //防止mid越界
                mid = n - 1;
            }
            if(right > n - 1)
            {
                //防止right越界
                right = n - 1;
            }
            merge_sort(arr, temp, left, mid, right);
        }
    }
    free(temp);
}
int main()
{
    srand(time(0));
    int N = 10;
    int* arr = (int*)malloc(N * sizeof(int));
    for(int i = N; i >= 0; i--)
    {
        arr[i] = i;
    }
    time_t list =clock();
    //快速排序
    merge_NO_recursion_sort(arr, N);
    time_t end = clock();

    for(int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    
    printf("time:%lld\n", end - list);
    free(arr);
    return 0;
}