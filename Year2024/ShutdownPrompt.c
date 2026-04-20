#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
int main()
{
    printf("Whether to turn it off\n");
    printf("1. Yes\n");
    printf("2. No\n");
    char mingling[10] ;
    scanf("%9s",mingling);
    if (strcmp(mingling,"yes") == 0)//strcmp头文件是<string.h>
    {
        system("shutdown -s -t 1");
    }
    else
    {
        printf("The user canceled the shutdown");
    }
    return 0; 
}