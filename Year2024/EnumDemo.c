#include <stdio.h>
//枚举
enum day
{
    MON = 1,
    TUE,
    WED,
    THU,
    FRI,
    SAT,
    SUN
    /*默认从1开始递增*/
};
int main()
{
    enum day d = TUE;
    printf("%d\n", d);
    return 0;
}