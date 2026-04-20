#include <iostream>
#include <string>
using namespace std;
namespace test_1
{
    //这是继承
    class Name
    {
        public:
        std::string _name;//名字
        virtual void printf()
        {
            std::cout << _name << std::endl;
        }
         ~Name()
        {
            std::cout << "Name析构函数" << std::endl;
        }
    };

    class Student :public Name
    {
        public:
        size_t size; //学号
        virtual void printf()
        {
            std::cout << _name << " " << size << std::endl;
        }
         ~Student()
        {
            std::cout << "Student析构函数" << std::endl;
        }
    };
}
namespace test_2
{
    //这是继承
    class Name
    {
        public:
        std::string _name;//名字
        virtual void printf()
        {
            std::cout << _name << std::endl;
        }
        virtual ~Name()
        {
            std::cout << "Name析构函数" << std::endl;
        }
    };

    class Student :public Name
    {
        public:
        size_t size; //学号
        virtual void printf()
        {
            std::cout << _name << " " << size << std::endl;
        }
        virtual ~Student()
        {
            std::cout << "Student析构函数" << std::endl;
        }
    };
}
void testVirtual(test_1::Name& testVirtual)
{
    //虚函数调用一般是通过引用或者是指针来调用
    testVirtual.printf();
}
void testVirtual(test_2::Name* testVirtual)
{
    //虚函数调用一般是通过引用或者是指针来调用
    testVirtual->printf();
}
void testTest1()
{
    test_1::Student s;
    test_1::Name n;
    s._name = "张三";
    s.size = 1001;
    testVirtual(s);
    testVirtual(n);
    n._name = "李四";
    testVirtual(n);
}
void testTest1S()
{
    test_2::Student* s = new test_2::Student;
    test_2::Name* n = new test_2::Name;
    s->_name = "王五";
    s->size = 1999;
    testVirtual(s);
    testVirtual(n);
    n->_name = "于六";
    testVirtual(n);
    delete s;
    delete n;
}
// void testTest2()
// {
//     test_1::Student* s = new test_1::Student;
//     test_1::Name* n = new test_1::Name;
//     delete s;
//     delete n;
// }

/*             上面的类没把析构函数写成虚函数导致报错                                                       */
/*            对于多态如果想用自定义类型指针需要把派生类和*基类析构函数写成虚函数来保证指针的正常使用           */
/*            虚函数底层是有虚基表实现，而且虚函数和他存储在常量区(代码段)中，所以虚函数不能在类外定义          */
void testTest2()
{
    test_2::Student* s = new test_2::Student;
    test_2::Name* n = new test_2::Name;
    delete s;
    delete n;
}
int main() // 主函数，程序的入口点
{
    testTest1(); // 调用函数 testTest1，执行其功能
    std::cout << std::endl << std::endl << std::endl;
    testTest1S();
    std::cout << std::endl << std::endl << std::endl;
    testTest2();
    return 0; // 返回0，表示程序正常结束
    return 0;
}