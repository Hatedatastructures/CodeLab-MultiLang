#include <stdio.h>
//联合体.共用体
//可以判断大小端
union test1
{
    int a;
    char b;
};
typedef struct test
{
    int a;
    char b;
}s;

int main()
{
    union test1 t;
    t.a = 1;
    printf("%d\n",t.a);
    printf("%c\n",t.b);
    printf("%d\n",sizeof(t));
    s s1;
    printf("%d\n",sizeof(s1));
    return 0;
}
