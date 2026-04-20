#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
//快速排序非递归实现
/*
思路：
1.首先定义一个栈，用来存储每次划分的中间值索引
2.将数组的左右区间的值先入栈，然后出栈，出栈后进行排序，排序完毕后，将排序后两边的区间分别入栈，直到栈为空
3.循环执行以下操作，直到栈为空，栈为空就代表所有元素都划分完毕
*/
#define MAXSIZE 100

typedef struct stack
{
    int data[MAXSIZE];
    int top;
}stack;

void initialize(stack *s)//初始化
{
    s->top = -1;
}

BOOL isEmpty(stack *s)//判断栈是否为空
{
    return s->top == -1;
    //如果栈顶指针为-1，则返回true，否则返回false
}

BOOL isFull(stack *s)//判断栈是否已满
{
    return s->top == MAXSIZE - 1;
    //如果栈顶指针等于最大容量减1，则返回true，否则返回false
}

void push(stack *s, int x)//入栈
{
    assert(s);
    if(isFull(s))
    {
        printf("栈已满，无法入栈\n");
        return;
    }
    s->data[++(s->top)] = x;
    //只能加加后赋值，否则会出错，因为top值开始时为-1
}

int pop(stack *s)//出栈
{
    assert(s);
    if(isEmpty(s))
    {
        printf("栈为空，无法出栈\n");
        return -1;
    }
    return s->data[(s->top)--];
    //先取值后减减
}

int peek(stack *s)//查看栈顶元素
{
    assert(s);
    if(isEmpty(s))
    {
        printf("栈为空，无法查看栈顶元素\n");
        return -1;
    }
    return s->data[s->top];
}

int size(stack *s)//查看栈中元素个数
{
    assert(s);
    return s->top + 1;
}
void swap(int* a, int* b) 
{
    int t = *a;
    *a = *b;
    *b = t;
}
int Three_middle(int*a , int left , int right)
{
    //在三个数中找到中间值，以此来优化快速排序
    int mid = (left + right) / 2;//取中间值
    if(a[left] < a[right])
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
    else if(a[left] > a[right])
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
int  traverse(int* arr, int left, int right)
{
    assert(arr);
    int mid = Three_middle(arr, left, right);
    swap(&arr[left], &arr[mid]);

    int key = arr[left];
    int key_index = left;
    while(left < right)
    {
        while(left < right && arr[right] >= key)
        {
            right--;
        }
        while(left < right && arr[left] <= key)
        {
            left++;
        }
        if(left < right)
        {
            swap(&arr[left], &arr[right]);
        }
    }
    swap(&arr[key_index], &arr[left]);
    return left;
}

void Quick_NO_recursion_sort(int* arr, int left, int right)
{
    assert(arr);
    stack s;
    initialize(&s);
    //入
    push(&s, left);
    push(&s, right);
    while(!isEmpty(&s))
    {
        //出
        int right_index = pop(&s);
        int left_index = pop(&s);
        

        int key_index = traverse(arr, left_index, right_index);
        //这是已经排序好的，所以不用再入栈了

        if(key_index - 1 > left_index)
        {
            //分割成两个区间，分别入栈，左区间
            push(&s, left_index);
            push(&s, key_index - 1);
        }

        if(key_index + 1 < right_index)
        {
            //分割成两个区间，分别入栈，右区间
            push(&s, key_index + 1);
            push(&s, right_index);
            //出的时候先出右区间，再出左区间，反着来
        }
    }
}

int main()
{
    int N = 100000000;
    int* arr = (int*)malloc(N * sizeof(int));
    for(int i = N; i >= 0; i--)
    {
        arr[i] = i;
    }
    time_t list =clock();

    //快速排序
    Quick_NO_recursion_sort(arr, 0, N - 1);


    time_t end = clock();

    // for(int i = 0; i < N; i++)
    // {
    //     printf("%d ", arr[i]);
    // }
    
    printf("time:%lld\n", end - list);
    free(arr);
    return 0;
}