#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#define OFFSETOF(type,my_name) ((size_t)&((type*)0)->my_name)//从0地址开始计算直到找到成员变量
/*宏定义模拟实现*/
typedef struct student
{
    char name[20];
    int age;
    char sex;
    float score;
}stu;

int main()
{
    stu s1 = {"zhangsan", 18, 'm', 90.5};
    printf("%d\n",offsetof(stu,name));
    /*返回偏移量*/
    printf("%d\n",offsetof(stu,age));
    printf("%d\n",offsetof(stu,sex));
    printf("%d\n",offsetof(stu,score));
    printf("%d\n",OFFSETOF(stu,name));
    printf("%d\n",OFFSETOF(stu,age));
    printf("%d\n",OFFSETOF(stu,sex));
    printf("%d\n",OFFSETOF(stu,score));
    return 0;
}   