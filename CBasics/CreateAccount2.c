#include <stdio.h>
#include <string.h>

void wang()
{
    printf("请输入账户名\n");
    char account[20];
    scanf("%19s", account); // 防止缓冲区溢出
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        ; // 清理缓冲区残留
    }
    // 如果需要，可以在这里处理或打印账户名
    printf("您输入的账户名为：%s\n", account);
}

void zhen()
{
    char password[20], password2[20], confirm[5];

    printf("请输入密码：\n");
    scanf("%19s", password); // 防止缓冲区溢出
    printf("请确认密码：\n");
    scanf("%19s", password2);

    if (strcmp(password, password2) == 0)
    {
        printf("两次输入的密码一致\n");
        printf("确认注册账户?yes/no: ");
        scanf("%4s", confirm); // 限制输入长度

        if (strcmp(confirm, "yes") == 0)
        {
            printf("注册成功\n");
        }
        else if (strcmp(confirm, "no") == 0)
        {
            printf("注册失败，是否重新注册？(yes/no): ");
            char k[5];
            scanf("%4s", k);
            if (strcmp(k, "yes") == 0)
            {
                printf("继续注册...\n");
                zhen(); // 递归调用，注意递归深度
            }
            else
            {
                printf("注册结束\n");
                return;
            }
        }
        else
        {
            printf("无效输入，请重新确认\n");
            zhen(); // 递归调用，注意递归深度
        }
    }
    else
    {
        printf("两次输入的密码不一致，请重新输入\n");
        zhen(); // 递归调用，注意递归深度
    }
}

int main()
{
    wang();
    zhen();
    return 0;
}