#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
typedef struct stu
{
    char a[10];
    /*12*/
    char b[10];
    /*12*/
    int k[10];
    /*40*/
    long long l[10];
    /*80*/
}stu;

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    int g =200;
    int*d =&g;
    int n =sizeof(stu);
    stu*p=(stu*)malloc(sizeof(stu)*(*d));
    /*不初始化*/
    for(int i=0;i<(*d);i++)
    {
        scanf("%s",p[i].a);
        scanf("%s",p[i].b);
        scanf("%d",&p[i].k[0]);
        scanf("%lld",&p[i].l[1]);
    }
    /*开始初始化*/
    for(int i=0;i<(*d);i++)
    {
        printf("%s %s %d %lld\n",p[i].a,p[i].b,p[i].k[0],p[i].l[1]);
    }
    stu*m=(stu*)calloc((*d),sizeof(stu));
    /*默认已经初始化*/
    for(int i=0;i<(*d);i++)
    {
        printf("%s %s %d %lld\n",m[i].a,m[i].b,m[i].k[0],m[i].l[1]);
    }
    stu*l=(stu*)realloc(p,sizeof(stu)*(*d)*2);
    /*相当于追加空间*/
    if(l!=NULL)
    {
        p=l;
        printf("分配成功！\n");
    }
    else
    {
        printf("%s",strerror(errno));
        printf("分配失败！\n");
    }
    int ar=sizeof(stu)*(*d);
    int arr=sizeof(stu)*(*d)*2;
    printf("%d %d\n",ar,arr);
    free(p);
    free(m);
    p=NULL;
    m=NULL;
    return 0;
}