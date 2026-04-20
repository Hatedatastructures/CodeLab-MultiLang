#include <stdio.h>
#define PRINT(N) printf(#N" = %d\n", N)
#define PRINT1(N) printf("#N""=%d\n", N)
#define PRINT2(N,GESI) printf(#N" = "GESI"\n", (N))
int main()
{
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    float f = 1.0;
    int x = 10;
    printf("%lf\n", x);
    PRINT(a);
    PRINT(b);
    PRINT(c);
    PRINT(d);
    PRINT(e);
    /*
    #的用法
    */
    PRINT1(a + b);
    PRINT1(a + b + c);
    PRINT1(a + b + c + d);
    PRINT1(a + b + c + d + e);
    //
    PRINT2(a + b + c + d + e, "%d");
    PRINT2(a + b + c + d + e, "%ld");
    PRINT2(f+a, "%lf");
    printf("%lf\n", a);
    return 0;
}