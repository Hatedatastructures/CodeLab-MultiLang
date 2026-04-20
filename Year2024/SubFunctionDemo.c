#include <stdio.h>
#include <windows.h>

int sub (int x,int y)
{
    return x+y;
}

int sum (int x,int y)
{
    return x-y;
}

int num (int x,int y)
{
    return x*y;
}

int nub (int x,int y)
{
    return x/y;
}

void meau()
{
    printf("-----0.退出1.加法----\n");
    printf("-----2.减法3.乘法----\n");
    printf("-------4.除法--------\n");
}

void wing(int*a)
//回调函数
{
    int (*pa[5])(int x,int y)={0,sub,sum,num,nub};
    printf("请输入两个数：->\n");
    int c=0;
    int l=0;
    scanf("%d %d",&c,&l);
    int ret =pa[(*a)](c,l);
    //函数指针传参
    printf("%d\n ",ret);
}
int main()
{
    //SetConsoleOutputCP(936);
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
    int a=0;
    printf("-------计算器--------\n");
    {
        meau();
        scanf("%d",&a);
        wing(&a);
    } while (a);
    return 0;
}