#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void print()
{
    printf("*************************************\n");
    printf("****  Enter 1: Guess the number  ****\n");
    printf("****        Input 2: Shutdown    ****\n");
    printf("*************************************\n");

}
void game()
{
    int i =0;
    int sum = rand()%100+1;//余数1~100，不加1，则0~99
    //rand max值为32767
    printf("Please enter a number between 1 and 100");
    while (1)//一直循环直到猜中才会跳出
    {
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
            int a;  
            char command[64]; // 用于存储system命令的字符串  
  
            printf("How long does it take to shut down? (Enter seconds)\n");  
            scanf("%d", &a);  
            printf("Power off after %d seconds\n", a);  
  
            // 使用sprintf构建system命令  
            sprintf(command, "shutdown -s -t %d", a);  
  
            // 执行system命令  
            system(command);  
  
            // 注意：system函数执行后，程序可能会立即返回，因为shutdown命令是在后台执行的。  
            // 如果需要等待关机完成，则应该考虑其他方法，但通常这不是必要的。 
            break;
        }
    
    default:
    {
        printf("Typing error\n");
        break;
    }
    }
    } while (num);
    return 0;
}