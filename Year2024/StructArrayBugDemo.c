#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_MAX 10
/*数组长度*/
#define SUM_MAX 10
/*结构体数量*/
/*bug*/
typedef struct st
{
    int a;
    int b;
    char c;
    char d[];
}stu;
int main()
{
    stu*s=(stu*)malloc((sizeof(stu)+STR_MAX)*SUM_MAX);
    if(s==NULL)
    {
        printf("内存分配失败");
        return 1;
    }
    for(int i=0;i<SUM_MAX;i++)
    {
        (s+i)->a=i;
        (s+i)->b=i+1;
        (s+i)->c='a'+i;
        s->d[i]='a'+i;
    }
    for(int i=0;i<SUM_MAX;i++)
    {
        printf("第%2d项：%d %d %c",i+1,(s+i)->a,(s+i)->b,(s+i)->c);
        printf("\n");
    }
    for(int i=0;i<SUM_MAX;i++)
    {
        for(int j=0;j<STR_MAX;j++)
        {
            printf("%c",(s+i)->d[i]);
        }
        printf("\n");
    }
    free(s);
    s=NULL;
    return 0;
}