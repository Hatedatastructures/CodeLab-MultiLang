#include <stdio.h>
int wang()
{
    int day = 0;
    scanf("%d", &day);
    switch (day)//switch 语句只能判断整形
    {
        case  1 :
        case  2 :
        case  3 :
        case  4 :
        case  5 :
        printf("上课\n");
        break;
        case  6 :
        printf("摆烂\n");
        break;
        case  7 :
        printf("摆烂\n");
        break;
        default:
        printf("选择错误\n");
        break;
    }
    return  0;

}
int main()
{
    int day = 0;
    scanf("%d", &day);
    switch (day)//switch 语句只能判断整形
    {
        case  1 :
        printf("上课\n");
        break;
        case  2 :
        printf("上课\n");
        break;
        case  3 :
        printf("上课\n");
        break;
        case  4 :
        printf("上课\n");
        break;
        case  5 :
        printf("上课\n");
        break;
        case  6 :
        printf("摆烂\n");
        break;
        case  7 :
        printf("摆烂\n");
        break;
        default:
        printf("选择错误\n");
        break;
    }
    return  0;
}


