#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

// 选择排序
/*
思路：
先找到数组中最小和最大的元素，将最小元素放到数组的最前面，将最大元素放到数组的最后面，然后对剩下的元素进行同样的操作，直到整个数组排序完成。
*/
void swsp (int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void choose_sort(int *arr, int len)
{
    assert(arr);
    int i = 0;
    int min = i;
    int max = i;
    for(; i < len - 1 && i >= 0; i++)
    {
        for(int j = i + 1; j < len && j >= 0; j++)
        {
            //在数组中先找大数的下标位置，再找小数的下标位置
            if(arr[j] < arr[min])
            {
                min = j;//找小数的下标位置
            }
            if(arr[j] > arr[max])
            {
                max = j;//找大数的下标位置
            }
        }
        swsp(&arr[i], &arr[min]);//交换小数
        if(min == i)
        {
            max = min;//判断是否交换了小数，如果交换了，则大数下标位置不变，否则大数下标位置为当前下标，防止大数被交换到前面
        }
        swsp(&arr[len - 1], &arr[max]);
        min++;
        max--;
    }
}

int main()
{
    
    srand(time(0));
    int num = 1000000;
    int *arr = (int *)malloc(sizeof(int) * num);
    for(int i = 0; i < num; i++)
    {
        arr[i] = rand();
    }
    int time_t = clock();
    choose_sort(arr, num);
    int time_t1 = clock();
    // for(int i = 0; i < num; i++)
    // {
    //     printf("%d ", arr[i]);
    // }
    printf("\n");
    printf("%d\n", time_t1 - time_t);//计算排序消耗的时间
    return 0;
}