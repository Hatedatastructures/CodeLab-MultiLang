//结构体
#include <stdio.h>
struct yuhaiwei //结构体定义，yuhaiwei是结构体定义的类型
{
    char name[20];//定义的name都相当于是结构体的成员
    int age;//单个数字不用分配空间
    char sex[12];
    char telephone[18];
};//结构体定义完要加分号表示结束

void wang(struct yuhaiwei *a)//a是值
//定义wang函数，参数指向yuhaiwei的指针，yuhaiwei是类型不能指向zheng
{
    printf("%s %d %s %s\n",(*a).name,(*a).age,(*a).sex,(*a).telephone);
    //(*a)指针地址指向结构体成员，且*a必须加括号,访问内存地址直接获得结果
    printf("%s %d %s %s\n",a->name,a->age,a->sex,a->telephone);
    //->结构体指针变量,指向成员名
}

int main()
{
    struct yuhaiwei zhang = {"wangzhen",16,"nan","13738349304"};//定义zhang储存
    //zhang是结构体对象.是成员名
    printf("%s %d %s %s\n",zhang.name, zhang.age, zhang.sex,zhang.telephone);//打印，输出后面必须是定义的zhang加.后面在跟上结构体内定义的类型
    wang(&zhang);
    //调用函数wang，并传入zhang数据的地址
    return 0;
}