#include <string.h>
#include <stdio.h>
int main()
{
    char arr[]="hello";//string求出的结果需要定义一个存放的值//
    int len = strlen("hel");//strlen函数是求字符串的长度的函数//
    int wang = strlen(arr);
    printf("%d\n", len);
    printf("%d\n", wang); 
    return 0;
}