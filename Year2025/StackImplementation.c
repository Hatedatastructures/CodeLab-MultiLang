#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
//栈实现
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
int main()
{
    stack stack;

    //初始化
    initialize(&stack);

    //入栈
    push(&stack, 10);
    push(&stack, 20);
    push(&stack, 30);

    printf("栈顶元素: %d\n", peek(&stack));
    printf("栈的大小: %d\n", size(&stack));

    printf("出栈元素: %d\n", pop(&stack));
    printf("栈顶元素: %d\n", peek(&stack));
    printf("栈的大小: %d\n", size(&stack));
    return 0;
}