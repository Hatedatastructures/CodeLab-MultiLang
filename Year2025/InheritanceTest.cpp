#include <iostream>
#include <string>
using namespace std;
namespace test
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
    class Information
    {
        public:
        int age; //年龄
        virtual void printf()
        {
            std::cout << age << std::endl;
        }
        virtual ~Information()
        {
            std::cout << "Information析构函数" << std::endl;
        }
    };
    class Student :public Name,public Information
    {
        public:
        size_t size; //学号
        virtual void printf()
        {
            std::cout << _name << " " <<age << " " << size << std::endl;
        }
        virtual ~Student()
        {
            std::cout << "Student析构函数" << std::endl;
        }
    };
    void printf(Name* val)
    {
        val->printf();
    }
    void printf(Information* val)
    {
        val->printf();
    }
    void printf(Student* val)
    {
        val->printf();
    }
}
void test_()
{
    test::Student* student = new test::Student();
    test::Name* name = new test::Name();
    test::Information* information = new test::Information();

    student->age = 18;
    student->_name = "张三";
    student->size = 20250001;
    test::printf(student);
    std::cout << std::endl;

    name->_name = "张三";
    test::printf(name);
    std::cout << std::endl;

    information->age = 18;
    test::printf(information);

    student->printf();
    std::cout << std::endl;
    name->printf();
    std::cout << std::endl;
    information->printf();
    std::cout << std::endl;

    delete student;
    std::cout << std::endl;
    delete name;
    std::cout << std::endl;
    delete information;
    std::cout << std::endl;
}
int main()
{
    test_();
    return 0;
}