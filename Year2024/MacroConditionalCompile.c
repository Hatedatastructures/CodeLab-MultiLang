#include <stdio.h>
#define M 3
//#define __DEBUG__
/*该句注释掉会把下面范围内全部屏蔽*/
int main()
{
    int a=78;
    int b=34;
    int c=45;
    #if M > 3
    #ifdef __DEBUG__
    if (a > b)
    {
        if (a > c)
        {
            printf("%d", a);
        }
        else
        {
            printf("%d", c);
        }
    }
    printf("\n");
    #endif
    #endif 
    return 0;
}