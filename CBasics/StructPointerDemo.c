#include <stdio.h>  
  
struct wang 
{  
    char chuanganqi[10];  
    char pingmu[6];  
    char xinghao[8];  
    char name[4];  
    int age;  
    char telphone[12];  
    int jiage; // 注意这里只有一个价格字段  
};  
  
void mac(struct wang *zhizhe) {  
    printf("传感器：%s\n", zhizhe->chuanganqi);  
    printf("平幕品牌：%s\n", zhizhe->pingmu); // 注意这里我修改了“品牌”为“平幕品牌”，以匹配您的提示  
    printf("型号：%s\n", zhizhe->xinghao);  
    printf("名称：%s\n", zhizhe->name);  
    printf("年龄：%d\n", zhizhe->age);  
    printf("电话：%s\n", zhizhe->telphone);  
    printf("价格：%d\n", zhizhe->jiage);  
    printf("\n"); // 增加换行符以美观地分隔每组数据  
}  
  
void test(struct wang *wangren) 
{ // 修改参数类型为 struct wang*  
    printf("请输入传感器名字\n");  
    scanf("%9s", wangren->chuanganqi); // 去掉 &，并限制输入长度  
    printf("请输入屏幕品牌\n");  
    scanf("%5s", wangren->pingmu); // 去掉 &，并限制输入长度  
    printf("请输入型号\n");  
    scanf("%7s", wangren->xinghao); // 去掉 &，并限制输入长度  
    printf("请输入姓名\n");  
    scanf("%3s", wangren->name); // 去掉 &，并限制输入长度  
    printf("请输入年龄\n");  
    scanf("%d", &wangren->age); // 对于 int 类型，需要使用 &  
    printf("请输入电话\n");  
    scanf("%11s", wangren->telphone); // 去掉 &，并限制输入长度（为 '\0' 留出空间）  
    printf("请输入价格\n");  
    scanf("%d", &wangren->jiage); // 对于 int 类型，需要使用 &  
}  
  
int main() 
{  
    struct wang wangren;  
    test(&wangren); // 传递结构体指针  
    mac(&wangren);  
    return 0;  
}