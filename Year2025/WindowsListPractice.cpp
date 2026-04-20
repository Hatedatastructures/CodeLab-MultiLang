#include <iostream>
#include <cstdint>
#include <string>
#include <cstring>
#include <windows.h>
#include <list>
#include <assert.h>

#define NAME_MAX 25
#define SEX_MAX 10
#define ADDRESS_MAX 100

namespace Information_Restrictions
{
    uint32_t num_max = 1000000000;//人数限制
    
    std::string name_ch = "China";//判断姓名中文字符串

    std::string name_en = "English";//判断姓名英文字符串

    uint8_t age_max = 150;//年龄限制

    std::string SEX_CHINA = "China";//判断性别中文字符串

    std::string SEX_ENGLISH = "English";//判断性别英文字符串

    uint8_t tel_max = 11;//电话号码长度限制

    uint8_t id_max = 18;//身份证号长度限制

    float Main_Subject = 150.0; //主科总分

    float Sub_Subject = 70.0; //副科总分

    float subs_subject = 50.0; //副科总分
}


namespace Student_system
{
    namespace Function_structs
    {
        typedef struct input
        {
            std::string  Promptwords;
            void* address;
            void* Limit_parameters;
            BOOL(*Enteralimit)(const void*, const void*);
        }inputs;

        typedef struct compare
        {
            void* address;
            void* limit;
            BOOL(*Enteralimit)(const void*, const void*);
        }com;
    }

    typedef struct Function_offset
    {
        const char* Placeholding;
        const void* Compare_pointers_A;
        const void* Compare_pointers_B;
    }foffset_list;

    typedef enum stu_Sode_list
    {
        EXITS,
        HEAD_INSERTION,
        TAIL_INSERTION,
        HEAD_DELETION,
        TAIL_DELETION,
        MIDDLE_INSERTION,
        MIDDLE_DELETION,
        MIDDLE_REVISE,
        POSITION_FIND,
        SORTING,
        PRINT_LIST,
        RELEASE_LIST,
        FILE_READ,
        FILE_WRITE
    } MenuOption;
}
class Student_examine
{
    public:
        static BOOL num_checks(const void* num,const void* limit = NULL)//人数检查
        {
            assert(num);
            if(*(uint32_t*)limit < *(uint32_t*)num)
            {
                std::cout << "人数超出范围" << std::endl;
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        static BOOL name_checks(const void* name,const void* limit = NULL)//姓名检查
        {
            assert(name);
            const char* name_ptr = static_cast<const char*>(name);
            if(limit == NULL)
            {
                return TRUE;
            }
           if(std::strcmp((const char*)limit,"China") ==0 )
            {
                size_t len = strlen(name_ptr);
                if (len >= 2 && len < NAME_MAX)
                {
                    return TRUE;
                }
                std::cout << "姓名长度不合法" << std::endl;
            }
            else if (std::strcmp((const char*)limit,"English") == 0 )
            {
                if(std::strlen(name_ptr) >= 2 && std::strlen(name_ptr) < NAME_MAX)
                {
                    return TRUE;
                }
                std::cout << "姓名长度不合法" << std::endl;
            }
            return FALSE;
        }
        static BOOL age_checks(const void* age,const void* limit = NULL)//年龄检查
        {
            assert(age);
            if(*(uint8_t*)limit < *(uint8_t*)age)
            {
                std::cout << "年龄超出范围" << std::endl;
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        static BOOL sex_checks(const void* sex,const void* limit = NULL)//性别检查
        {
            assert(sex);
            if(limit == NULL)
            {
                return TRUE;
            }
            if(std::strcmp((const char*)limit,"China") ==0 )
            {
                if((std::strcmp((char*)sex,"男") == 0)||(std::strcmp((char*)sex, "女") == 0))
                {
                    return TRUE;
                }
            }
            else if (std::strcmp((const char*)limit,"English") == 0 )
            {
                if((std::strcmp((char*)sex,"male") == 0)||(std::strcmp((char*)sex, "female") == 0))
                {
                    return TRUE;
                }
            }
            std::cout << "性别错误" << std::endl;
            return FALSE;
        }
        static BOOL tel_checks(const void* tel,const void* limit = NULL)//电话检查
        {
            assert(tel);
            uint64_t num = *(uint64_t*)tel;
            uint8_t len = 0;
            while(num)
            {
                num /= 10; len++;
            }
            if(len != *(uint8_t*)limit)
            {
                std::cout << "电话号码错误" << std::endl;
                return FALSE;
            }
            return TRUE;
        }
        static BOOL id_checks(const void* id,const void* limit = NULL)//身份证检查
        {
            assert(id);
            uint64_t num = *(uint64_t*)id;
            uint8_t len = 0;
            while(num)
            {
                num /= 10; len++;
            }
            if(len != *(uint8_t*)limit)
            {
                std::cout << "身份证号码错误" << std::endl;
                return FALSE;
            }
            return TRUE;
        }
        static BOOL greed_check(const void* grade, const void* limit = NULL)//成绩检查
        {
            assert(grade);
            if ((*(float*)grade) <= *(float*)limit || *(float*)grade >= 0)
            {
                return TRUE;
            }
            else
            {
                std::cout << "成绩错误" << std::endl;
                return FALSE;
            }
        }
};

class student
{
    private:
        typedef struct subjects
        {
            float Chinese;                                        /*语文*/
            float Math;                                           /*数学*/
            float English;                                        /*英语*/
            float politics;                                       /*政治*/
            float history;                                        /*历史*/
            float geography;                                      /*地理*/
            float biology;                                        /*生物*/
            float chemistry;                                      /*化学*/
            float physics;                                        /*物理*/
        }subjects;
    public:
        typedef struct students
        {
            uint32_t num;                                         /*学号*/
            char name[NAME_MAX];                                  /*姓名*/
            uint8_t age;                                          /*年龄*/
            char sex[SEX_MAX];                                    /*性别*/
            uint64_t tel;                                         /*电话*/
            char address[ADDRESS_MAX];                            /*地址*/
            uint64_t identity_card;                               /*证号*/
            subjects grades;                                      /*成绩*/
        } students;

        static students malloc_my ()
        {
            students* stu_s = (students*) malloc (sizeof(students));
            memset(stu_s, 0, sizeof(stu_s->grades));
            students my_data_student_val = *stu_s;
            return my_data_student_val;
        }
        student(students stu_s = student::malloc_my() ) 
        {
            //构造函数
            stu_s = malloc_my();
        }
        // 定义一个函数New_student，用于创建一个新的学生列表
        std::list<students>*New_student()
        {
            //新节点返回指针
            std::list<students>*list = new std::list<students>;
            return list;
        }
        ~student()
        {
            free(this);
        }
        BOOL students_inputs(std::string  Promptwords, void* address)
        {
            uint8_t number_max = 0 ;
            while(true)
            {
                std::cout << Promptwords << std::endl;
                if(std::cin >> address)
                {
                    break;
                }
                std::cin.sync();
                std::cout << "输入错误，请重新输入" << std::endl;
                if(number_max >= 3)
                {
                    std::cout << "输入错误次数过多，请重新输入" << std::endl;
                    return FALSE;
                }
            }
            return TRUE;
        }
        void studentInput(students& stu)
        {
            Student_system::Function_structs::inputs input_s [] =
            {
                {"请输入学号：",     &stu.num,              &Information_Restrictions::num_max,     Student_examine::num_checks    },
                {"请输入姓名：",     stu.name,              nullptr,                                Student_examine::name_checks   },
                {"请输入年龄：",     &stu.age,              &Information_Restrictions::age_max,     Student_examine::age_checks    },
                {"请输入性别：",     stu.sex,               &Information_Restrictions::SEX_CHINA,   Student_examine::sex_checks    },
                {"请输入电话：",     &stu.tel,              &Information_Restrictions::tel_max,     Student_examine::tel_checks    },
                {"请输入地址：",     stu.address,           nullptr,                                nullptr                        },
                {"请输入身份证号：", &stu.identity_card,    &Information_Restrictions::id_max,      Student_examine::id_checks     },
                {"请输入语文成绩：", &stu.grades.Chinese,   &Information_Restrictions::Main_Subject,Student_examine::greed_check   },
                {"请输入数学成绩：", &stu.grades.Math,      &Information_Restrictions::Main_Subject,Student_examine::greed_check   },
                {"请输入英语成绩：", &stu.grades.English,   &Information_Restrictions::Main_Subject,Student_examine::greed_check   },
                {"请输入政治成绩：", &stu.grades.politics,  &Information_Restrictions::Sub_Subject, Student_examine::greed_check   },
                {"请输入历史成绩：", &stu.grades.history,   &Information_Restrictions::subs_subject,Student_examine::greed_check   },
                {"请输入地理成绩：", &stu.grades.geography, &Information_Restrictions::subs_subject,Student_examine::greed_check   },
                {"请输入生物成绩：", &stu.grades.biology,   &Information_Restrictions::subs_subject,Student_examine::greed_check   },
                {"请输入化学成绩：", &stu.grades.chemistry, &Information_Restrictions::subs_subject,Student_examine::greed_check   },
                {"请输入物理成绩：", &stu.grades.physics,   &Information_Restrictions::Sub_Subject, Student_examine::greed_check   },
            };
            for(Student_system::Function_structs::inputs i : input_s)
            {
               while(true)
               {
                    if (students_inputs(i.Promptwords,(void*)i.address) != FALSE)
                    {
                        if (i.Enteralimit != nullptr)
                        {
                            if (i.Enteralimit(i.address, i.Limit_parameters) == FALSE)
                            {
                                std::cout << "输入错误，请重新输入" << std::endl;
                                continue;
                            }
                        }
                        break;
                    }
                    break;
               }
            }
        }

};
int main()
{
    student students;
    std::list<student::students>*list = students.New_student();
    //constexpr int list_size = 10; //表示在编译时就确定了list的大小，在运行时无法改变，从而提高效率
    return 0;
}