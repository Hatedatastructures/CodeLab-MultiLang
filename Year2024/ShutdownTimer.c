#include <stdio.h>  
#include <stdlib.h>  
  
int main()  
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
    return 0;  
}