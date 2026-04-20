#include <iostream>

using namespace std;

class student
{
public:
    char*name;
    int age;
    int score;
private:
    char*sex;
protected:
    char*address;
};
inline int& add(int& a, int& b)
{
    static int c =0;
    c = a + b;
    return c;
}

int main()
{
    //
    int a = 1, b = 2;
    int ha = 3, hb = 4;
    int c = add(a, b);
    //引用返回
    cout << c << endl;
    int d = add(ha, hb);
    cout << d << endl;
    char*name = nullptr;
    auto p = name;
    student cj;
    auto c1 = cj;
    //打印变量类型
    cout << typeid(p).name() << endl;
    cout << typeid(c).name() << endl;
    //输出的数字是类名字的长度
    cout << typeid(c1).name() << endl;

    //创建和释放指针
    int* p1 = new int;
    delete p1;
    return 0;
}