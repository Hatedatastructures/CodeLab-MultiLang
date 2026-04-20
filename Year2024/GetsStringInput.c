#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    char str[100];
    printf("请输入一个字符串：");
    //该函数读取到换行符才会停下来,不会检查输入的字符是否大于该数组的元素，有溢出风险
    char *result = fgets(str, sizeof(str), stdin);
    // 移除 fgets 可能读入的尾部换行符
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
    //gets函数的返回值是这个字符数组的首元素地址
    printf("%s ",str);
    printf("\n");
    //puts会自动换行
    puts(str);
    if (result == str) 
    {
       printf("Input was successful.\n");
       puts(result);
       printf("%s ",result);
       //%s期望的的是一个char*类型的指针，传指针变量
       //通过字符串打印向后读取数组
    }
    return EXIT_SUCCESS;
    //返回EXIT_SUCCESS，表示程序成功执行
}