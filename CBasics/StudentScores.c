//输入学号和各科成绩打印
#include <stdio.h>

int main()
{
    long xueihao=0;
    float a=0.0;
    float b=0.0;
    float c=0.0;
    scanf("%ld;%f,%f,%f",&xueihao,&a,&b,&c);
    printf("The each subject score of No.%ld is %.2f %.2f %.2f",xueihao,a,b,c);
    return 0;
}