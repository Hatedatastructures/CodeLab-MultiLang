#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char arr[20] = {0};
    char brr[] = "hello";
    strcpy(arr,brr);
    printf("%s\n",arr);
    system("pause");
    return 0;
}