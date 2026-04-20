#include <stdio.h>
//位段
//优点节省空间，但是跨平台移植性差
struct ss
{
    char c1 : 1;//这里数字是多少比特位
    char c2 : 3;
    char c3 : 5;
    char c5 : 7;
    int i : 24;
    //最大数为2^24-1
    int j : 12;
};
//位段储存是连续的
//开辟空间时，如果是int则以4字节为最小单位，如果是char则以1字节为最小单位
//如果位段没有写完，则补0，继续开辟下一个空间存数据
int main()
{
    struct ss s;
    s.c1 = 1;//赋值
    s.i = 66;
    //赋值时不能赋值大于该位段所能表示的最大值
    printf("%d\n", sizeof(s));
    return 0;
}