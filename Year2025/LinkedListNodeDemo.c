#include <stdio.h>
#include <stdlib.h>
typedef struct studentsnode
{
    int* data;
    struct studentsnode* next;
}sode;
int main()
{
    signed int n;
    _Complex double a;
    long  double b;
    double c;
    printf("%zd\n",sizeof(n));
    printf("%zd\n",sizeof(a));
    printf("%zd\n",sizeof(b));
    printf("%zd\n",sizeof(c));
    printf("%zd\n",sizeof(sode));
    return 0;
}