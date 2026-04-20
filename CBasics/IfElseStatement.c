#include <stdio.h>
int main()
{
    int a = 0;
    int b = 0;
    scanf("%d", &a);
    if (a > 0)
        b = 1;
    else if (a == 0)
        b = 0;
    else
        b = -1;
    printf("%d\n", b);
    return 0;
}