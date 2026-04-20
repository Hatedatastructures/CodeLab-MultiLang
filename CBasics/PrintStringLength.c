#include <string.h> 
#include <stdio.h>
int main()
{
    char arr[]={'h', 'e', 'l', 'l','o','\0'};
    int len = strlen(arr);
    printf("%d\n", len);
    return 0;
}