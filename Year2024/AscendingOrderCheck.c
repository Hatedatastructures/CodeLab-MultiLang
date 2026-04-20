#include <stdio.h>
//判断一个整型数组是否为升序
int main()
{
    int n = 0;
    printf("请输入整数个数：");
    scanf("%d", &n);
    int a[n];
    int flag = 0;
    for (int i = 0; i < n; i++)
    {
        printf("请输入第%d个整数：", i+1);
        scanf("%d", &a[i]);
        if(i > 0)
        {
            if(a[i]>a[i-1])
            {
                flag = 1;
            }
            else if(a[i] == a[i-1])
            {
                flag = 1;
            }
            else
            {
                flag = 0;
                break;
            }
        }
    }
    if(flag)
    {
        printf("升序\n");
    }
    else
    {
        printf("不是升序\n");
    }
    return 0;
}