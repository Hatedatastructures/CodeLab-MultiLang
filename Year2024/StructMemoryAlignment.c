#include <stdio.h>
//内存对齐，结构体对齐
//求结构体大小
//#pragma pack(1)
//更改对齐数
typedef struct s
{
    char k;
    int d;
    char l;
}a;//4+4+1=9，但是内存对齐，4+4+4=12

typedef struct s1
{
    char j;
    char c2;
    int i;
    a;
}b;//12+8=20

typedef struct s2
{
    char u;
    char p;
    b;
}c;//1+1=2,因为要补齐，所以是4,4+20=24

typedef struct s3
{
    char u;
    //补到4的倍数
    b;
    //24
    char p;
    //24+4=28
}d;

typedef struct s4
{
    b;//20
    char u;
    double o;
    //这里是 8 字节对齐，因为有 double 类型成员
}e;
typedef struct s5
{
    char u;
    double o;
    double arr[100];
    b;//20
}f;
typedef struct s6
{
    f g[100];
    int po;
}g;
int main()
{
    //重点是看结构体中，成员变量的类型，根据类型的大小去找最大对齐数
    printf("%d\n", sizeof(a));
    printf("%d\n", sizeof(b));
    printf("%d\n", sizeof(c));
    printf("%d\n", sizeof(d));
    printf("%d\n", sizeof(e));
    printf("%d\n", sizeof(f));
    printf("%d\n", sizeof(g));
    return 0;
}