#include <iostream>
#include <string>
namespace test_1
{
    //这是继承
    class Name
    {
        public:
        std::string _name;//名字
        void printf()
        {
            std::cout << _name << std::endl;
        }
    };

    class Student :public Name
    {
        public:
        size_t size; //学号
    };
}
namespace test_2
{
    //组合，这是最简单的框架，以后写肯定要写构造函数和拷贝之类的
    class Name
    {
        public:
        std::string _name;//名字
        void printf()
        {
            std::cout << _name << std::endl;
        }
    };
    class Student 
    {
        public:
        size_t size; //学号
        Name _students_name;
        // Student()
        // {
        //     _students_name._name = "测试2";
        //     _students_name.printf();
        // }
        //声明在外面使用，要想直接使用需要在student类中声明构造来赋值
    };
}
int main()
{
    test_1::Student s1;
    s1._name = "测试";
    s1.printf();
    test_2::Student s2;
    s2._students_name._name = "测试2";
    s2._students_name.printf();
    return 0;
}