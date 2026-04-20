#include <stdio.h>
//获取键盘输入用法
int main ()
{
    int wang = 0;//用wang储存键盘输入的数据
    while ((wang = getchar()) !=EOF)//注意是吧getchar获取的值赋值给wang，不是getchar（wang）###括号内是获取键盘的值不能填数据
    {
        putchar(wang);//输出键盘获取的数据
    }
    return 0;

}