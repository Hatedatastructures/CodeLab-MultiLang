#include <iostream>
class A
{
    public:
    size_t num = 2;

};
class B :public A
{
    size_t num = 9;
    public:
    void printf()
    {
        std::cout << num <<std::endl;
        std::cout << A::num << std::endl;
        // std::cout << ::A::sum << std::endl;
    }
};



/*                对象演示              */
class Information
{
    public:
    std::string name;
    size_t tel;
    size_t ID;
};
// 定义一个名为Student的类，该类继承自Information类
class Student : public Information
{
    // 私有成员变量，只能在类的内部访问
    private:
        // std::string类型的成员变量，用于存储学生的班级信息
    std::string Class;
        // size_t类型的成员变量，用于存储学生的成绩，size_t通常用于表示大小或计数
    size_t grades;
};

class teacher : public Information
//
{
    private:
    size_t Job_number;
    bool director;
    std::string category;
};

class Dormitory_management:public Information
{
    private:
    size_t Job_number;
};


int main()
{
    B test;
    test.printf();
    Dormitory_management st;
    //st.ID;
    //如果继承方式是public则父类是公开状态，以此类推
    return 0;
}