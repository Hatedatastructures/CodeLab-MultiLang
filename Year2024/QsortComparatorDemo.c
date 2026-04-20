#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

//qsort函数快排思想
//可作用于结构体，字符，整形。
int com_put(const void*a,const void*b)
{
    return (*(int *)a)-(*(int *)b);
    //降序的话把a和b的值反减
}

struct stu 
{
    char name[20];
    int  age;
};

int com_stu_name(const void*a,const void*b)
{
    //strcmp的值是大于小于或等于0
    return strcmp(((struct stu*)a)->name,((struct stu*)b)->name);
}
int com_stu_age(const void*a,const void*b)
{
    //问为什么没有解引用？
    //答因为->就是解引用访问下标元素
    return (((struct stu*)a)->age-((struct stu*)b)->age);
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    int arr[20]={1,24,24,53,36,4,56,5,7,23,4,23,4,23,4,23,4,32,67,29};
    int sz =sizeof(arr)/sizeof(arr[0]);
    int num =sizeof(arr[0]);
    qsort(arr,sz,num,com_put);
    //传参类型是排序位置的起始位置，要排序的个数，和要排序的元素类型大小，最后是比较函数的指针地址
    for(int j=0;j<sz;j++)
    {
        printf("%d ",arr[j]);
    }
    printf("\n");
    //结构体排序
    struct stu s[5]={{"于海威",18},{"姚星宇",16},{"黎春阳",20},{"牛明赫",17},{"王振",18}};
    int sub =sizeof(s)/sizeof(s[0]);
    int sun =sizeof(s[0]);
    qsort(s,sub,sun,com_stu_name);
    //按姓名
    for(int l=0;l<sub;l++)
    {
        printf("%s ",s[l].name);
    }
    printf("\n");
    qsort(s,sub,sun,com_stu_age);
    for(int l=0;l<sub;l++)
    {
        printf("%s %d ",s[l].name,s[l].age);
    }
    return 0;
}