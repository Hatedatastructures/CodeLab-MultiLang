#include <stdio.h>
#include <string.h>
int main()
{
    char arr1[]={0};
    char arr2[]="hello world";//拷贝源在后面，目的地在前面
    strcpy(arr1,arr2);//拷贝函数
    printf("%s\n",arr1);
    return 0;   
}