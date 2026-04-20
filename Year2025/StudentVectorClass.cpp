#include <iostream>
#include <vector>
#include <cstdint>
#define NAME_MAX 25
#define SEX_MAX 10
#define ADDRESS_MAX 100
namespace students
{
    typedef struct student
    {
        uint32_t num;                                         /*学号*/
        char name[NAME_MAX];                                  /*姓名*/
        uint8_t age;                                          /*年龄*/
        char sex[SEX_MAX];                                    /*性别*/
        uint64_t tel;                                         /*电话*/
        char address[ADDRESS_MAX];                            /*地址*/
        uint64_t identity_card;                               /*证号*/                            
    } student;
}
int add (int a)
{
    int ab= a + 1;
    return ab;
}
int main()
{
    // std::vector<students::student> students;
    // students::student temp;
    
    // students.push_back(temp);//尾插
    // std::cout << "size:" << students.size() << std::endl;

    int a = 10;
    std::cout << "a:" << a << std::endl;
    return 0;
}