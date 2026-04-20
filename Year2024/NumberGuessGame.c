#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void print()
{
    printf("*************************************\n");
    printf("****  Enter 1: Guess the number  ****\n");
    printf("****        Input 2: Exit        ****\n");
    printf("*************************************\n");

}
void game()
{
    int i =0;
    int sum = rand()%100+1;//余数1~100，不加1，则0~99
    //rand max值为32767
    while (1)//一直循环直到猜中才会跳出
    {
        //int sum = rand()%100+1;难度更高，每猜一个都会刷新
        printf("Please guess the number\n");
        scanf("%d",&i);//判断
        if (sum<i)
        {
            printf("Big guess\n");
        }
        else if (sum>i)
        {
            printf("Small guess\n");
        }  
        else
        {
            printf("You guessed it\n");
            break;//跳出循环
        }
    }  
}
int main()
{
    int num;
    srand((unsigned)time(NULL));//根据时间戳生成随机数
    do
    {
        print();
    
    printf("Please enter->\n");
    scanf("%d",&num);
    switch (num)
    {
    case 1:
        {
           game(); //调用函数
           break;
        }
    
    case 2:
        {
            printf("Exit\n");
            break;
        }
    
    default:
        printf("Input error, please re-enter\n");

        break;
    }
    } while (num);
    return 0;
}