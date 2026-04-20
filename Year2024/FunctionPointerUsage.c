#include <stdio.h>

// 函数Add，用于计算两个整数相加并返回结果
int Add(int x, int y) 
{
    return x + y;
}

// game函数，接收一个函数指针func（指向接收两个int参数并返回int的函数）和一个整数l
// 在函数内部利用传入的函数指针func进行相应的计算，并输出和返回相关结果
int game(int (*func)(int, int), int l) 
{
    int a = 726548;
    int b = 376424;
    int c = func(a, b);
    int f = func(l, c);
    printf("%d \n", c);
    printf("%d \n", f);
    return func(f,l);
}

// 修改后的text函数，接收一个函数指针func（指向接收两个int参数并返回int的函数）和两个整数参数
int text(int (*func)(int, int), int x, int y) 
{
    int k = func(x, y);
    return k;
}

int main() 
{
    // 调用game函数，传入Add函数指针和整数0（可根据实际情况调整该整数）
    int f = game(Add, 3424);  
    // 调用text函数，传入Add函数指针和两个整数参数
    int d = text(Add, 325434, f);  
    printf("f=%d\n", f);
    printf("d=%d\n", d);
    return 0;
}