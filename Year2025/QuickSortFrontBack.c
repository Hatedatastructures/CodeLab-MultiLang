#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//快速排序算法前后指针法
/*
思路：
先遍历数组，前指针从前往后找找到比key大的数，前指针继续向后移动
如过前指针找到比key小的数，后指针就加加，把大的数和小的数交换
这样就把大的数都放在了后面，小的数都放在前面，然后循环停下来的时候，
就是找到key的位置了
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
int single(int *a, int left, int right)
{
    int mid = Three_middle(a , left , right);//取中间值
    swap(&a[left] , &a[mid]);//将中间值和第一个值交换

    int key = a[left];
    int cur = left;
    int prev = left - 1;
    
    while(cur < right)
    {
        if(a[cur]< key && ++prev != cur )
        {
            //此时prev和cur一个是指向比key大的数，一个是指向比key小的数，交换
            //把比key小的数放在前面，比key大的数放在后面
            prev++;
            swap(&a[prev] , &a[cur]);
            //cur找到比key小的数就停下来，把prev++，然后再交换cur和prev的值
        }
        cur++;//cur大于于key就继续向后移动
    }
    swap(&a[++prev] , &a[left]);
    return prev;
}
void Quick_recursion_sort(int*a , int left , int right)
{
    if((right - left) > 10)//当数组大于10个元素时，使用递归，继续分解
    {
        int key = single(a , left , right);
        Quick_recursion_sort(a , left , key - 1);//根据返回的下标进行递归
        Quick_recursion_sort(a , key + 1 , right);
    }
    else//当数组元素小于10个时，使用插入排序，插入排序小数组效率高
    {
        insert_sort(a + left , right - left + 1);
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
