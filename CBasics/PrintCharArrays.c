#include <stdio.h>
int main()
{
    char array1[]="hello";//字符串不用\0转义，并且打印不出\0//
    char array2[] = {'h', 'e', 'l', 'l', 'o', '\0'};//单个字符需要在最后打上\0进行转义//
    printf("%s\n", array1);//双引号字符串，单引号字符，单个字符可以储存数据//
    printf("%s\n", array2);
    return 0;
}
