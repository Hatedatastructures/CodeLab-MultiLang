#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>
//堆排序算法
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void AJsdown(int* arr, int n, int parent)//向下调整算法
{
    assert(arr);
    int child = 2 * parent + 1;
    while(child < n)
    {
        //防止child越界访问
        if(child + 1 < n && arr[child] < arr[child + 1])
        {
            child++;//父亲节点和最大的孩子节点比较，如果左孩子比右孩子大，则child指向右孩子
        }
        if(arr[child] > arr[parent])
        {
            //如果孩子节点比父亲节点大，则交换
            swap(&arr[child], &arr[parent]);
            parent = child;//继续向下找，child指向父亲节点
            child = 2 * parent + 1;
        }
        else
        {
            break;
        }
    }
}
// void AJsup(int* arr, int n, int child)//向上调整算法
// {
//     assert(arr); 
//     int parent = (child - 1) / 2;//算出父亲节点位置
//     while(child > 0)
//     {
//         if(arr[child] > arr[parent])
//         {
//             swap(&arr[child], &arr[parent]);
//             child = parent;
//             parent = (child - 1) / 2;
//         }
//         else
//         {
//             break;
//         }
//     }
// }
void heapSort(int* arr, int n)
{
    //堆排序思路就是先建堆，然后交换堆顶元素和最后一个元素，再向下调整
    assert(arr);
    for(int i = (n - 1 - 1) / 2;  i >= 0; i--)//找到最后一个非叶子节点
    {
        //建大堆
        AJsdown(arr, n , i);//向下调整算法
    }
    for(int i = n - 1; i > 0; i--)
    {
        swap(&arr[0], &arr[i]);//将堆顶元素和最后一个元素交换
        AJsdown(arr, i, 0);//向下调整算法
    }
}
int main()
{
    
    srand(time(0));
    int n = 10000;
    int* arr = (int*)malloc(sizeof(int) * n);
    for(int i = 0; i < n; i++)
    {
        arr[i] = rand();
    }
    uint64_t t1 = clock();
    heapSort(arr, n);
    uint64_t t2 = clock();
    for(int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("时间效率 = %lld毫秒\n", t2 - t1);
    return 0;
}