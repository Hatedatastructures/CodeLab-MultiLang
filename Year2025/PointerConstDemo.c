#include <stdio.h>

int main()
{
    char arr[] ={'1','2','3','4','5','6','7','8','9'};
    const char *p = arr;
    char* const cou = arr;
    printf("%p\n%p\n",arr,&arr[0]);
    printf("%p\n%p\n",p,&p[0]);
    printf("%p\n%p\n",cou,&cou[0]);
    char * arrs = arr + 1;
    const char * p2 = arr + 2;
    printf("%td\n", p2 - arr);
    return 0;
}