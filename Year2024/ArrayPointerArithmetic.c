#include <stdio.h>
int main()
{
    int arr[5]={1,2,3,4,5};
    int arr1[2][3]={0};
    int *pr =(int*)(&arr+1);
    //pr垃圾值
    printf("%d \n",sizeof(arr1+1));
    printf("%d \n",sizeof(arr1));
    printf("%d \n",sizeof(*arr1));
    printf("%d \n",sizeof(*(arr1[0]+1)));
    printf("%d \n",sizeof(&arr1));
    printf("%d \n",sizeof(arr1[0]));
    printf("%d \n",sizeof(arr1[0]+1));
    printf("%d \n",sizeof(&arr1[0]+1));
    printf("%d %d",*(&arr+1),*(pr-1));
    return 0;
}