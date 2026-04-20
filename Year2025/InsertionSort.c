#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//插入排序
void insert_sort(int *arr, int len)
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
int main()
{
    
    int arr[]= {1, 5, 3, 2, 4, 6, 9, 8, 19, 10};
    int sum = sizeof(arr) / sizeof(arr[0]);
    insert_sort(arr, sum);
    for(int i = 0; i < sum; i++)
    {
        printf("%d ", arr[i]);
    }
    return 0;
}
