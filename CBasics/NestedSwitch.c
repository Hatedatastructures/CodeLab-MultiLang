// switch循环嵌套
#include <stdio.h>
int main()
{
    int a = 3;
    int b = 5;
    switch (a) // 判断a为什么值选择进入那个
    {
    case 1:
        a++; // 因为a的值为3所以不会进入1和2
    case 2:
        b++;
    case 3:
        a++; // 进入a自加1
    case 4:
        switch (b) // switch可以嵌套循环
        {
        case 5:
            a++; // b为5进入循环
        case 6:
            b++; // 因为没有berak所以继续
        }
    case 7:
        a++;
        ;
        break; // 中止循环

    default: // 其他数值话直接就进入这个
        break;
    }
    return 0;
}