#include <iostream>
#include <string>
//菱形继承
namespace test_1
{
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

    class Teacher :public Name
    {
        public:
        size_t _ID; //职工号
    };

    class Assistant :public Student,public Teacher
    {
        public:
        std::string _majorCourse; //主修课程
    };
    void test ()
    {
        Assistant Test1;
        Test1._ID = 25;
        // Test1._name = " 语法为"; 找不到name 因为有两个
        Test1.Student::_name = "语法为";
        Test1.Teacher::_name = "要没";
        Test1.Student::printf();
        //访问student类里的成员两者互不影响
        Test1.Teacher::printf();
        //访问Teacher类里的成员
    }
}
namespace test_2
{
    class Name
    {
        public:
        std::string _name;//名字
        void printf()
        {
            std::cout << _name << std::endl;
        }
    };

    //virtual在这里的意思是虚继承Name
    class Student :virtual public Name
    {
        public:
        size_t size; //学号
    };

    class Teacher :virtual public Name
    {
        public:
        size_t _ID; //职工号
    };

    class Assistant :public Student,public Teacher
    {
        public:
        std::string _majorCourse; //主修课程
    };
    void test()
    {
        //底层是虚基表，调试时会看到多了几个东西，虚基表里存的是从它所在的类到最下面_name存的偏移量
        //它可以通过偏移量找到_name的的地址
        Assistant Test;
        Test._name = "语法为";
        Test.printf();
        Test.Student::_name = "要没";
        Test.Student::printf();
        Test.Teacher::_name = "妖媚";
        Test.Teacher::printf();
    }
}
//菱形继承会导致数据二义性和数据冗余，官方解决方法就是虚继承
int main()
{
    test_1::test();
    std::cout << std::endl << std::endl;
    test_2::test();
    return 0;
}