#include <stdio.h>
#include <string.h>

void AccountRegistration ()
{
   
    char name[20],a[5],ch;
    printf("请输入注册账户名称：\n");
    scanf("%19s", name);//控制输入字数
    printf("您输入的账户名称为：%s\n", name);
    printf("确定?yes/no\n");
    scanf("%s",&a);

    while ((ch=getchar())!='\n')//清理缓冲区\n
    {
        ;
    }
    

    if (strcmp(a,"yes")==0)//判断yes进入下一个函数，  strcmp是判断字符串是否相等的函数
    {
        printf("继续");
    }
    else //语句结束
    {
      printf("用户取消了注册\n"); 
      return ;
    }

}

void login()//成功
{
    printf("注册成功\n");
    //登入后功能实现
}

void wang()//失败
{
    char key[5];
    printf("密码输入错误\n");
    printf("注册还是退出?yes/no\n");
    scanf("%s", &key);
    if (strcmp(key, "yes") == 0)
    {
       printf("请重新输入\n");
       AccountRegistration();
    }
    else
    {
       printf("您已取消注册\n");
       return ;
    } 
}

int main()
{
    AccountRegistration();    
    char password1[25], password2[25];
    printf("请输入密码：\n");
    scanf("%24s",password1);
    printf("请确认密码：\n");
    scanf("%24s", password2);
    if (strcmp(password1, password2) == 0)
    {
        printf("两次输入的密码相同\n");
        login();

    }
    else 
    {
        printf("两次输入的密码不相同\n");
        wang();
    }
    return 0;
}