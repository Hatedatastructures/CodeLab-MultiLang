#include <stdio.h>
int main()
{
    char*arr="于海威";
    char sdr[]="于海威";
    printf("长度是%zu \n",sizeof(*arr));
    printf("长度是%zu \n",sizeof(arr[1]));
    //该输出类似于
    printf("长度是%zu \n",sizeof(*(arr+1)));

    printf("长度是%zu \n",sizeof(&arr));
    //相当于2级指针
    printf("长度是%zu \n",sizeof(sdr));
    return 0;
}