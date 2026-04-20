#include <stdio.h>
struct wang
{
    char chuanganqi [10];
    char pingmu [6];
    char xinghao [8];
    char name [4];
    int age;
    char telphone [12];
    int jiage ;
};
void mac (struct wang *zhizhe)
{
    printf("传感器：%s\n",zhizhe->chuanganqi);
    printf("屏幕品牌：%s\n",zhizhe->pingmu);
    printf("型号：%s\n",zhizhe->xinghao);
    printf("名称：%s\n",zhizhe->name);
    printf("年龄：%d\n",zhizhe->age);
    printf("电话：%s\n",zhizhe->telphone);
    printf("价格：%d\n",zhizhe->jiage);
    printf("\n");  // 增加换行符以美观地分隔每组数据
}

void test (struct wang wangren)
{
    printf("请输入传感器名字\n");
    scanf("%9s",wangren.chuanganqi);
    printf("请输入屏幕品牌\n");
    scanf("%6s",wangren.pingmu);
    printf("请输入型号\n");
    scanf("%8s",wangren.xinghao);
    printf("请输入姓名\n");
    scanf("%4s",wangren.name);
    printf("请输入年龄\n");
    scanf("%d",&wangren.age);
    printf("请输入电话\n");
    scanf("%12s",wangren.telphone);
    printf("请输入价格\n");
    scanf("%d",&wangren.jiage);
}
int main()
{
    struct wang wangren;
    test(wangren);
    mac(&wangren);
    return 0;
}    
