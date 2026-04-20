#include <stdio.h>
#include <string.h>

int gcd(int a, int b) 
{
    while (b != 0) 
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
int lcm(int a, int b) 
{
    return (a / gcd(a, b)) * b;
}

int teger(const char* str ,int pa,int *p) 
{
    int cou =0;
    for(int i =0 ; i<pa ;i++)
    {
        if(str[i]>=48 && str[i]<=57)
        {
            p[cou]=str[i]-'0';
            cou++;
        }
    }
    return cou;
}

void meau()
{
    printf("1.最大公约数\n");
    printf("2.最小公倍数\n");
    printf("3.实现数字字符串转换成对应整数\n");
}
int main()
{
    int  choice =0;
    //choice可以利用起来访问函数指针
    meau();
    int (*arr[2])(int x,int y)={gcd,lcm};
    //创建函数指针数组
    if(scanf("%d",&choice)!=EOF)
    //判断输入读取情况
    {
        int a,b;
        if(choice!=3)
        {
            printf("请输入两个数->\n");
            if(scanf("%d %d",&a,&b)==2)
            {
                int num = arr[choice-1](a,b);
                printf("结果是%d",num);
            }
            else
            {
                printf("整形输入错误！\n");
            }
        }
        else
        {
            char s [100];
            int p[100]={0};
            printf("请输入字符串->\n");
            if(scanf("%s",s)!=EOF)
            {
                int lf =strlen(s);
                printf("打印：\n");
                int sub =teger(s,lf,p);
                for(int k = 0;k<sub;k++)
                {
                    printf("%d",p[k]);
                }
                printf("\n");
                //美观
            }
            else
            {
                printf("字符串输入错误！\n");
            }
        }
    }
    else
    {
        printf("输入错误！\n");
    }
    return 0;
}