#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtemplate-body"
#include <iostream>
#include <string>
#include <cstdint>
#include <windows.h>
#include <assert.h>
namespace student_Validation_Limitations
{
    uint8_t age_limitation = 150;

    uint8_t tel_long = 11;

    uint8_t address_long = 18;

    template <class grades_type>
    class grades
    {
        public:
        grades_type grades_main = (grades_type)150;

        grades_type grades_min_s = (grades_type)50;

        grades_type grades_main_s = (grades_type)70;
    };
}
template <class grades_type>
class Information_management
{
    private:
    class student_list
    {
        public:
        class student
        {
            public:
                class subject 
                {
                    public:
                    grades_type Chinese;                                    /*语文*/
                    grades_type Math;                                       /*数学*/
                    grades_type English;                                    /*英语*/
                    grades_type politics;                                   /*政治*/
                    grades_type history;                                    /*历史*/
                    grades_type geography;                                  /*地理*/
                    grades_type biology;                                    /*生物*/
                    grades_type chemistry;                                  /*化学*/
                    grades_type physics;                                    /*物理*/
                };
                uint32_t num;                                               /*学号*/
                std::string name;                                           /*姓名*/
                uint8_t age;                                                /*年龄*/
                std::string sex;                                            /*性别*/
                uint64_t tel;                                               /*电话*/
                std::string address;                                        /*地址*/
                uint64_t identity_card;                                     /*证号*/
                subject grades;                                             /*成绩*/
                void student_printf() const
                {
                    std::cout << "学号：" << num               << " ";
                    std::cout << "姓名：" << name              << " ";
                    std::cout << "年龄：" << age               << " ";
                    std::cout << "性别：" << sex               << " ";
                    std::cout << "电话：" << tel               << " ";
                    std::cout << "地址：" << address           << " ";
                    std::cout << "证号：" << identity_card     << " ";
                    std::cout << "语文：" << grades.Chinese    << " ";
                    std::cout << "数学：" << grades.Math       << " ";
                    std::cout << "英语：" << grades.English    << " ";
                    std::cout << "政治：" << grades.politics   << " ";
                    std::cout << "历史：" << grades.history    << " ";
                    std::cout << "地理：" << grades.geography  << " ";
                    std::cout << "生物：" << grades.biology    << " ";
                    std::cout << "化学：" << grades.chemistry  << " ";
                    std::cout << "物理：" << grades.physics    << std::endl;
                }
                student()
                {
                    ;
                }
                student(const student& Student_volume)
                {
                    num = Student_volume.num;
                    name= Student_volume.name;
                    age = Student_volume.age;
                    sex = Student_volume.sex;
                    tel = Student_volume.tel;
                    address = Student_volume.address;
                    identity_card = Student_volume.identity_card;
                    grades = Student_volume.grades;
                }
        };
        student* data;
        student_list* prev;
        student_list* next;

        class student_Grade_checks
        {
            public:
            typedef struct input
            {
                std::string informationcheck_name;
                void* address;
                void* Limit_parameters;
                BOOL(*Enteralimit)(const void*, const void*);
            }inputs;

            static uint8_t enteralimitStudentLongCompare(const uint64_t* data)
            {
                uint8_t long_compare = 0;
                uint64_t datas = *data;
                while ( datas > 0 )
                {
                    long_compare++;
                    datas /= 10;
                }
                return long_compare;
                
            }
            static BOOL enteralimitStudentAge(const void* data, const void* Limit_parameters)
            {
                if (Limit_parameters)
                {
                    if ((*(uint8_t*)data) <= *(uint8_t*)Limit_parameters)
                    {
                        return TRUE;
                    }
                    else
                    {
                        std::cout << "错误!,年龄不能大于" << *(uint8_t*)Limit_parameters << std::endl;
                        return FALSE;
                    }
                }
                else
                {
                    if (*(uint8_t*)data == 0)
                    {
                        std::cout << "错误!,年龄不能为0" << std::endl;
                        return FALSE;
                    }
                    else
                    {
                        return TRUE;
                    }
                }
            }
            static BOOL enteralimitStudentTel(const void* data, const void* Limit_parameters)
            {
                uint64_t tel = *(uint64_t*)data;
                if(Limit_parameters)
                {
                    if(student_Grade_checks::enteralimitStudentLongCompare(&tel) != *(uint8_t*)Limit_parameters)
                    {
                        std::cout << "错误!,电话号码长度应为" << *(uint8_t*)Limit_parameters << std::endl;
                        std::cout << "当前电话号码长度为"  << student_Grade_checks::enteralimitStudentLongCompare(&tel) << std::endl;
                        return FALSE;
                    }
                    return TRUE;
                }
                else
                {
                    if(tel > 0)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }
            }
            static BOOL enteralimitStudentId(const void* data, const void* Limit_parameters)
            {
                uint64_t id = *(uint64_t*)data;
                if(Limit_parameters)
                {
                    if(student_Grade_checks::enteralimitStudentLongCompare(&id) != *(uint8_t*)Limit_parameters)
                    {
                        std::cout << "错误!,身份证号码长度应为" << *(uint8_t*)Limit_parameters << std::endl;
                        std::cout << "当前身份证号码长度为"  << student_Grade_checks::enteralimitStudentLongCompare(&id) << std::endl;
                        return FALSE;
                    }
                    return TRUE;
                }
                else
                {
                    if(id > 0)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }
            }
            static BOOL enteralimitStudentMainSubject(const void* data, const void* Limit_parameters)
            {
                if(*(grades_type*)data > *(grades_type*)0 && *(grades_type*)data <= *(grades_type*)Limit_parameters)
                {
                    return TRUE;
                }
                else
                {
                    std::cout << "错误!,成绩应在0~" << *(grades_type*)Limit_parameters << "之间" << std::endl;
                    return FALSE;
                }
            }
        };
        // student_Grade_checks Check_the_examples;
        ~student_list()
        {
            delete data;
            prev = nullptr;
            next = nullptr;
        }
        student_list(const student_list& student_list_volume)
        {
            data = new student(*(student_list_volume.data));
            prev = student_list_volume.prev;
            next = student_list_volume.next;
        }
        student_list()
        {
            data = new student;
            prev = nullptr;
            next = nullptr;
        }
        student_list& operator=(const student_list& student_list_volume)
        {
            if(this == &student_list_volume)
            {
                return *this;
            }
            delete data;
            data = new student(*(student_list_volume.data));
            prev = student_list_volume.prev;
            next = student_list_volume.next;
            return *this;
        }
        student_list* studentListNewNode()
        {
            student_list* new_node = new student_list;
            new_node->data = new student;
            new_node->prev = nullptr;
            new_node->next = nullptr;
            return new_node;
        }
        template <typename T>
        BOOL studentListStuinputS(std::string Promptwords, T& Input_variable)
        {
            while (true)
            {
                std::cout << Promptwords << std::endl;
                std::cin >> Input_variable;
                if (std::cin.eof())
                {
                    std::cout << "输入已结束。" << std::endl;
                    return FALSE;
                }
                std::cin.clear();
                return TRUE;
            }
        }
        BOOL studentListStuinputS(std::string Promptwords, void* Input_variable)
        {
            {
                std::cout << Promptwords << std::endl;
                std::cin >> Input_variable;
                if (std::cin.eof())
                {
                    std::cout << "输入已结束。" << std::endl;
                    return FALSE;
                }
                std::cin.clear();
                return TRUE;
            }
        }
        void studentListStuinput(student* Input_variables)
        {
            //输入检查
            student_Validation_Limitations::grades <grades_type>  grades_liat;
            typename Information_management::student_list::student_Grade_checks::inputs verify[] =
            {
                {"学号", &(Input_variables->num),              nullptr                                                 ,nullptr},
                {"姓名", &(Input_variables->name),             nullptr                                                 ,nullptr},
                {"年龄", &(Input_variables->age),              &student_Validation_Limitations::age_limitation         ,student_list::student_Grade_checks::enteralimitStudentAge},
                {"性别", &(Input_variables->sex),              nullptr                                                 ,nullptr},
                {"电话", &(Input_variables->tel),              &student_Validation_Limitations::tel_long               ,student_list::student_Grade_checks::enteralimitStudentTel},
                {"地址", &(Input_variables->address),          nullptr                                                 ,nullptr},
                {"证号", &(Input_variables->identity_card),    &student_Validation_Limitations::address_long           ,student_list::student_Grade_checks::enteralimitStudentId},
                {"语文", &(Input_variables->grades.Chinese),   &(grades_liat.grades_main)                              ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"数学", &(Input_variables->grades.Math),      &(grades_liat.grades_main)                              ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"英语", &(Input_variables->grades.English),   &(grades_liat.grades_main)                              ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"政治", &(Input_variables->grades.politics),  &(grades_liat.grades_main_s)                            ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"历史", &(Input_variables->grades.history),   &(grades_liat.grades_main_s)                            ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"地理", &(Input_variables->grades.geography), &(grades_liat.grades_min_s)                             ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"生物", &(Input_variables->grades.biology),   &(grades_liat.grades_min_s)                             ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"化学", &(Input_variables->grades.chemistry), &(grades_liat.grades_min_s)                             ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
                {"物理", &(Input_variables->grades.physics),   &(grades_liat.grades_main_s)                            ,student_list::student_Grade_checks::enteralimitStudentMainSubject},
            };
            for (uint8_t i = 0; i < sizeof(verify) / sizeof(verify[0]); i++)
            {
                while (true)
                {
                    if(studentListStuinputS(verify[i].informationcheck_name, verify[i].address))
                    {
                        if(verify[i].Enteralimit != nullptr)
                        {
                            if(verify[i].Enteralimit(verify[i].address, verify[i]. Limit_parameters) == false)
                            {
                                std::cout << "输入失败" << std::endl;
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
    public:
    student_list* head;
    Information_management()
    {
        head = nullptr;
    }
    
    // Information_management(const Information_management& Information_management_volume)
    // {
    //     head = nullptr;
    //     student_list* current = Information_management_volume.head->studentListNewNode();
    //     current = Information_management_volume.head;
    //     student_list* prevNode = nullptr;
    //     while (current != nullptr)
    //     {
    //         student_list* newNode = Information_management_volume.head->studentListNewNode();
    //         //创建新节点
    //         newNode->data = current->data;
    //         newNode->prev = prevNode;
    //         newNode->next = nullptr;
    //         if(prevNode != nullptr)
    //         {
    //             prevNode->next = newNode;
    //             //如果上一节点不为空，则将新节点连接到上一节点的下节点
    //         }
    //         else
    //         {
    //             head = newNode;
    //             //将新链表连接到this本地头节点
    //         }
    //         prevNode = newNode;
    //         current = current->next;
    //     }
    // }
    Information_management(const Information_management& Information_management_volume)
    {
        head = nullptr;
        student_list* current = Information_management_volume.head;
        student_list* prevNode = nullptr;
        while (current != nullptr)
        {
            student_list* newNode = new student_list(*current); // 直接通过拷贝构造函数创建新节点
            newNode->prev = prevNode;
            newNode->next = nullptr;
            if (prevNode == nullptr)
            {
                head = newNode;
            }
            else
            {
                prevNode->next = newNode;
            }
            prevNode = newNode;
            current = current->next;
        }
    }
    ~Information_management()
    {
        student_list* current = head;
        while (current != nullptr)
        {
            student_list* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
    }
    Information_management& operator=(const Information_management& Information_management_volume)
    {
        if (this == &Information_management_volume)
        {
            return *this;
        }
        //拷贝整个链表
        student_list* studentListDeepCopy = Information_management_volume.head->studentListNewNode();
        studentListDeepCopy = Information_management_volume.head;
        student_list* prevNode = nullptr;
        while (studentListDeepCopy != nullptr)
        {
            student_list* newNode = Information_management_volume.head->studentListNewNode();
            //创建新节点
            newNode->data = studentListDeepCopy->data;
            newNode->prev = prevNode;
            newNode->next = nullptr;   /*拷贝节点数据*/

            if(prevNode != nullptr)
            {
                prevNode->next = newNode;
                //如果上一节点不为空，则将新节点连接到上一节点的下一节点
            }
            else
            {
                head = newNode;
                //将新链表连接到this本地头节点
            }
            prevNode = newNode;
            studentListDeepCopy = studentListDeepCopy->next;
        }
        return *this;
    }
    void informationManagementPrintList(const Information_management* cour )
    {
        Information_management* current = cour->head;
        while (current != nullptr)
        {
            cour->informationManagementPrint(current->head);
            current = current->next;
        }
        std::cout <<"打印完成"<< std::endl;
    }
    void informationManagementPrint(const student_list* cour )  
    {
        cour->data->student_printf(); 
    }

    void informationManagementLinkedListWchecks(student_list*& push_back_Node )
    {
        if(push_back_Node == nullptr)
        {
           std::cout << "链表为空" << std::endl;
           std::cout << "请创建链表" << std::endl;
        }
    }
    void informationManagementHeadPushBack()
    {
        student_list* newNode = new student_list();
        newNode->studentListStuinput(newNode->data);
        if (!head)
        {
            head = newNode;
        }
        else
        {
            student_list* current = head;
            while (current->next)
            {
                 current = current->next;
            }
            current->next = newNode;
            newNode->prev = current;
        }
    }
   
};
   

int main()
{
    Information_management < int > Information_management_volume;
    Information_management_volume.informationManagementHeadPushBack();
    Information_management_volume.informationManagementPrint(Information_management_volume.head);
    delete Information_management_volume.head;
    return 0;
}
#pragma GCC diagnostic pop
