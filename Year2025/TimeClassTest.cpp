#include <iostream>

class test_time
{
public:
    explicit test_time(int h = 0, int m = 0, int s = 0)
    {
        //全缺省构造函数，explicit防止隐式转换
        hour = h;
        minute = m;
        second = s;
    }
    test_time()
    {
        //默认构造函数
        hour = 0;
        minute = 0;
        second = 0;
    }
    ~test_time()
    {
        std::cout << "析构函数被调用" << std::endl;
    }
    void show_printf() const
    {
        //这里加了const，表示这个函数不会修改对象的数据成员
        std::cout << hour << ":" << minute << ":" << second << std::endl;
    }
    const test_time* operator&() const
    {
        //返回常量引用，防止对象被意外修改
        return this;
    }
private:
    int hour;
    int minute;
    int second;
};
class time_
{
public:
    time_(int h = 0, int m = 0, int s = 0)
    :hour(h),minute(m),second(s)
    {
        //列表形式初始化，列表的初始话顺序与成员变量的声明顺序无关和构造函数的参数列表顺序有关
        std::cout << "构造函数被调用" << std::endl;
    }
    time_(const time_& t)
    {
        //拷贝构造函数
        hour = t.hour;
        minute = t.minute;
        second = t.second;
    }
    ~time_()
    {
        std::cout << "析构函数被调用" << std::endl;
    }
    void show_printf() const
    {
        //凡是加了const，都不能修改对象的数据成员
        std::cout << hour << ":" << minute << ":" << second << std::endl;
    }
    const time_* operator&() const
    {
        //返回常量引用，防止对象被意外修改
        return this;
    }
private:

    int hour;
    int minute;
    int second;
    
};
int main()
{
    //六个默认函数：构造函数、析构函数、拷贝构造函数、赋值运算符重载、取地址运算符重载、常量成员函数
    //构造函数有全缺省构造函数、默认构造函数、列表初始化构造函数
    test_time t1(10, 20, 30);
    t1.show_printf();

    time_ t2(10, 20, 30);
    t2.show_printf();
    
    std::cout << &t2 << std::endl;
    std::cout << &t1 << std::endl;
    return 0;
}