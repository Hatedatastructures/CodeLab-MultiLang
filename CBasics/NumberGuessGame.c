#include <stdio.h>
#include <windows.h>
#include <time.h>

void meau()
{
    printf("-------猜数字--------\n");
    printf("-----1.开始游戏------\n");
    printf("-----0.退出游戏------\n");
}

void game()
{
    // 随机数种子
    int num = rand() % 100 + 1;
    int guess = 0;
    int count = 0;
    printf("请输入一个1-100之间的整数：->\n");
    do
    {
        printf("请输入数字：->");
        scanf("%d", &guess);
        if (guess > num)
        {
            printf("猜大了\n");
        }
        else if (guess < num)
        {
            printf("猜小了\n");
        }
        else
        {
            printf("恭喜你猜对了\n");
            printf("\a");
            break;
        }
        count++;
    } while (count);
    printf("你一共猜了%d次\n", count);
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    // 字符编码
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
    // 设置颜色,加粗
    int inport = 0;
    srand((unsigned)time(NULL));
    do
    {
        meau();
        if (scanf("%d", &inport) != EOF)
        {
            if (inport == 1)
            {
                printf("游戏开始：->\n");
                game();
            }
            else if (inport == 0)
            {
                printf("按任意键退出游戏\n");
            }
            else
            {
                printf("输入错误请重新输入！\n");
            }
        }
        else
        {
            printf("读取异常！，请重新输入\n");
        }
    } while (inport);
    return 0;
}