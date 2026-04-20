#include <iostream>
#include <string>
using namespace std;
class Person
{
    public :
    Person () 
    {
        ++ _count;
    }
    protected :
        string _name ; // 姓名
    public :
        static int _count; // 统计人的个数。
};

int Person::_count = 0;

class Student : public Person
{
    protected :
        int _stuNum ; // 学号
    public:
        void fucn()
        {
            std::cout << ::Person::_count << std::endl;
        }
    private:
        size_t szie;
};
class Graduate : public Student
{
protected:
    string _seminarCourse;
};
int main()
{
    //通过访问限定符来保持
    Student s1;
    Student s2;
    Student s3;
    Student s4;
    Graduate g2;
    g2.fucn();
    cout << g2._count << endl;
    cout << Person::_count << endl;
    return 0;
}