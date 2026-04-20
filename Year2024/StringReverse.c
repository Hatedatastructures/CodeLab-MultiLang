#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//字符串逆序输出

void meau(char arr[], int len)
{
    int left = 0;
    int right = len - 1;
    while (left < right)
    {
        char tmp = arr[left];
        //容易混淆
        arr[left] = arr[right];
        arr[right] = tmp;
        left++;
        right--;
    }
    left = 0;
    for (int i = 0; i < len; i++)
        //找完有空格的单词逆序
    {
        if ((arr[i]) == ' ')
        {
            right = i - 1;
            while (left <= right)
            {
                char tmp = arr[left];
                arr[left] = arr[right];
                arr[right] = tmp;
                right--;
                left++;
            }
            left = i + 1;
        }
    }
    //left是最后一个
    right = len - 1;
    //处理最后一个单词
    while (left < right)
    {
        int tmp = arr[left];
        arr[left] = arr[right];
        arr[right] = tmp;
        left++;
        right--;
    }

}
int main()
{
    char str[] = "welcome to beijing!";
    int len = strlen(str);
    meau(str, len);
    printf("%s", str);
    return 0;
}