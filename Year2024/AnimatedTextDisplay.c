#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
int main()
{
    char arr1[] = "welcome to china!!!";
    char arr2[] = "###################";
    int left = 0;
    int right = sizeof(arr1) / sizeof(arr2[0]) - 2; // 减2是因为sizeof多读取了一个"\0"
    // int right=strlen(arr1)-1;
    while (left <= right)
    {
        arr2[left] = arr1[left]; // 两个数组共用一个下标
        arr2[right] = arr1[right];
        printf("%s\n", arr2);
        Sleep(1000); // 大写的s，延时函数1000毫秒
        left++;
        right--;
        system("cls"); // windows系统函数，清空窗口
    }
    return 0;
}