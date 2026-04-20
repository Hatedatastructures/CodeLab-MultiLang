#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
//冒泡排序算法
void bubble_sort(int arr[], int sz)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < sz - 1; i++)
    {
        for (j = 0; j < sz - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
        
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}
int main()
{
    int N = 1000000;
    int* arr = (int*)malloc(N * sizeof(int));
    for(int i = N; i >= 0; i--)
    {
        arr[i] = i;
    }
    time_t list =clock();
    //冒泡排序
    bubble_sort(arr, N);
    time_t end = clock();

    for(int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    
    printf("time:%lld\n", end - list);
    free(arr);
    return 0;
}
