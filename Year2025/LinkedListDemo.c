/*链表*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdint.h>
#include <inttypes.h>
#define NUMEBER_BUFFERS 1024
#define NAME_MAX 25
#define SEX_MAX 10
#define ADDRESS_MAX 100
#define NAME_MAX_S "%24s"
#define SEX_MAX_S "%9s"
#define ADDRESS_MAX_S "%99s"
#define FILE_READS  "C:\\Users\\C1373\\Desktop\\students_data.txt"
#define FILE_WRITES "C:\\Users\\C1373\\Desktop\\students_data.txt"
typedef struct subjects
{
    uint8_t Chinese;                                   /*语文*/
    uint8_t Math;                                      /*数学*/
    uint8_t English;                                   /*英语*/
    uint8_t politics;                                  /*政治*/
    uint8_t history;                                   /*历史*/
    uint8_t geography;                                 /*地理*/
    uint8_t biology;                                   /*生物*/
    uint8_t chemistry;                                 /*化学*/
    uint8_t physics;                                   /*物理*/
}subjects;
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
typedef struct studentsnode
{
    students* data;
    struct studentsnode* next;
}node;

typedef struct compare
{
    void* address;
    BOOL(*Enteralimit)(const void*);
}com;

typedef struct input
{
    char* Promptwords;
    char* Placeholders;
    void* address;
    BOOL(*Enteralimit)(const void*);
}inputs;

BOOL numjudgment(const void* num)
{
    uint32_t n = *(uint32_t*)num;
    if (n == 0) 
    {
        printf("学号不能为0！\n");
        return FALSE;
    }
    return TRUE;
}
BOOL namejudgment(const void* name)
{
    char *p = (char*)name;
    unsigned char c = (unsigned char)p[0];
    for(uint8_t i = 0; i < strlen(p); i++)
    {
        if ((c & 0xF0) == 0xE0) 
        {
            if (strlen(p) >= 3) 
            {
                return 1;
            }
        }
    }
    return TRUE;
}
BOOL agejudgment(const void* age)
{
    if((*(uint8_t*)age) == 0 || (*(uint8_t*)age >= 150))
    {
        printf("错误!,年龄不能大于等于150!\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
BOOL sexjudgment(const void* sex)
{
    if(strcmp(sex, "男") == 0 || strcmp(sex, "女") == 0 || strcmp(sex, "man") == 0 || strcmp(sex, "woman") == 0)
    {
        return TRUE;
    }
    else
    {
        printf("错误!,性别只能为男或女\n");
        return FALSE;
    }
    
}
BOOL teljudgment(const void* tel)
{
    long long num = *(long long*)tel;
    uint8_t len = 0;
    while(num > 0)
    {
        num /= 10;
        len++;
    }
    if(len != 11)
    {
        printf("错误!,电话号码必须为11位,当前%u位\n", len);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL IDnumberjudgment(const void* identity_card)
{
    long long num = *(long long*)identity_card;
    uint8_t len = 0;
    while(num > 0)
    {
        num /= 10;
        len++;
    }
    if(len != 18)
    {
        printf("错误!,身份证号必须为18位,当前%u位\n", len);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
BOOL MainSubjectjudgment(const void* MainSubject)
{
    if((*(uint8_t*)MainSubject) > 150)
    {
        printf("错误!,成绩非法或不合理,成绩不能大于150或小于0\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
BOOL Sub_sectionjudgment(const void* Sub_section)
{
    if((*(uint8_t*)Sub_section) > 70)
    {
        printf("错误!,成绩不能大于70\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
BOOL Sub_sectionsjudgment(const void* Sub_sections)
{
    if((*(uint8_t*)Sub_sections) > 50)
    {
        printf("错误!,成绩不能大于50\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
BOOL stuinputs(char*Promptwords, char*Placeholders,void*address)
{
    while(1)
    {
        printf("%s",Promptwords);
        int result =scanf(Placeholders,address);
        if(result == 1 )
        {
            while (getchar() != '\n');
            return TRUE;
        }
        else
        {
            printf("输入错误!\n");
            while (getchar() != '\n');
            continue;
        }
    }
}                            
void stuinput(students* input)                            /*输入*/
{
    inputs inputs[] =
    {
        {"请输入学号：",    "%u",           &input->num,             numjudgment},
        {"请输入姓名：",    NAME_MAX_S,     input->name,             namejudgment},
        {"请输入年龄：",    "%d",           &input->age,             agejudgment},
        {"请输入性别：",    SEX_MAX_S,      input->sex,              sexjudgment},
        {"请输入电话：",    "%zu",          &input->tel,             teljudgment},
        {"请输入地址：",    ADDRESS_MAX_S,  input->address,          NULL},
        {"请输入证号：",    "%lld",         &input->identity_card,   IDnumberjudgment},
        {"请输入语文成绩：", "%hhu",        &input->grades.Chinese,  MainSubjectjudgment},
        {"请输入数学成绩：", "%hhu",        &input->grades.Math,     MainSubjectjudgment},
        {"请输入英语成绩：", "%hhu",        &input->grades.English,  MainSubjectjudgment},
        {"请输入政治成绩：", "%hhu",        &input->grades.politics, Sub_sectionjudgment},
        {"请输入历史成绩：", "%hhu",        &input->grades.history,  Sub_sectionsjudgment},
        {"请输入地理成绩：", "%hhu",        &input->grades.geography,Sub_sectionsjudgment},
        {"请输入生物成绩：", "%hhu",        &input->grades.biology,  Sub_sectionsjudgment},
        {"请输入化学成绩：", "%hhu",        &input->grades.chemistry,Sub_sectionsjudgment},
        {"请输入物理成绩：", "%hhu",        &input->grades.physics,  Sub_sectionjudgment},
    };
    for(uint8_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
    {
        while (1)
        {
            if(stuinputs(inputs[i].Promptwords, inputs[i].Placeholders, (void*)inputs[i].address) != FALSE)
            {
                if(inputs[i].Enteralimit != NULL)
                {
                    if(inputs[i].Enteralimit(inputs[i].address) == FALSE)
                    {
                        printf("输入无效！\n");
                        continue;
                    }
                }
                else
                {
                    break;
                }
            }
            break;
        } 
    }
}
void Empty_printf(char* str)
{
    
    printf("%s""链表为空\n",str);
    system("pause");
    exit(-1);
}

node* newnode()                                    /*创建新节点*/
{
    node* q = NULL;
    q = (node*)malloc(sizeof(node));
    if(q == NULL)
    {
        printf("内存分配失败！\n");
        system("pause");
        exit(-1);
    }
    q->data = (students*)malloc(sizeof(students));
    if(q->data == NULL)
    {
        free(q);
        q = NULL;
        printf("内存分配失败！\n");
        system("pause");
        exit(-1);
    }
    memset(q->data, 0, sizeof(students));
    memset(&q->data->grades, 0, sizeof(subjects));
    q->next = NULL;
    return q;
}

void initialize(node** head)                                  /*初始化*/
{
    node* p = NULL;
    p = (node*)malloc(sizeof(node));
    if(p == NULL)
    {
        free(p);
        p = NULL;
        Empty_printf("初始化时");
    }
    else
    {
        p->data = (students*)malloc(sizeof(students));
        if(p->data == NULL)
        {
            free(p->data);
            p->data = NULL;
            Empty_printf("初始化时");
        }
    }
    memset(p->data, 0, sizeof(students));
    memset(&p->data->grades, 0, sizeof(subjects));
    p->next = NULL;
    *head = p;
    printf("初始化成功\n");
}
void Tail_deletion(node**head)                                /*尾删*/
{
    if(*head == NULL || head == NULL)
    {
        free(*head);
        *head = NULL;
       Empty_printf("尾部删除时");
    }
    if((*head)->next == NULL)
    {
        free((*head)->data);
        free(*head);
        *head = NULL;
        printf("尾部删除成功！\n");
    }
    else
    {
        node* p = *head;
        node* q = NULL;
        while(p->next != NULL)
        {
            q = p;
            p = p->next;
        }
        q->next = NULL;
        free(p->data);
        free(p);
        printf("尾部删除成功！\n");
    };
}
void Tail_plugging(node** head)                                /*尾插*/
{
    if(head == NULL)
    {
        free(*head);
        *head = NULL;
        Empty_printf("尾部插入时");
    }

    if(*head == NULL) 
    {
       node* cour = newnode();
       stuinput(cour->data);
       *head = cour;
    }
    else
    {
        node* cour = *head;
        while(cour->next != NULL)
        {
            cour = cour->next;
        }
        cour->next = newnode();
        stuinput(cour->next->data);
    }
    printf("尾部插入成功！\n");
}

void Head_insertion(node** head)                             /*头插*/
{
    if(head == NULL)
    {
        Empty_printf("头部插入时");
    }
    if(*head == NULL)
    {
        node* cour = newnode();
        stuinput(cour->data);
        *head = cour;
    }
    else
    {
        node* cour = newnode();
        stuinput(cour->data);
        cour->next = *head;
        *head = cour;
    }
    printf("头部插入成功！\n");
}
void Head_deletion(node** head)                              /*头删*/
{
    if(*head == NULL)
    {
        free(*head);
        *head = NULL;
        Empty_printf("头部删除时");
    }
    else
    {
        node* cour = *head;
        *head = (*head)->next;
        free(cour->data);
        free(cour);
    }
    printf("头部删除成功！\n");
}
void Insert_in_the_middle(node**head,uint32_t index)          /*中间插入*/
{
    if(head == NULL ) 
    {
        free(*head);
        *head = NULL;
        Empty_printf("中间插入时");
    }
    if(index == 0 || *head == NULL)
    {
        Head_insertion(head);
        return;
    }
    if((*head)->next == NULL)
    {
        Tail_plugging(head);
        return;
    }
    uint32_t num = 0;
    node *cour = *head;
    while(cour!=NULL)
    {
        if(num == index && cour->next == NULL)
        {
            Tail_plugging(head);
            break;
        }
        else if(num == index)
        {
            node* new_node = newnode();
            stuinput(new_node->data);
            new_node->next = cour->next;
            cour->next = new_node;
            printf("中间插入成功！\n");
            break;
        }
        else
        {
            cour = cour->next;
            num++;
        }
    }
    if(num < index)
    {
        printf("插入失败！当前%u条数据\n",num);
    }
}
void Intermediate_deletion(node** head,uint32_t index)     /*中间删除*/
{
    if(*head == NULL)
    {
        free(*head);
        *head = NULL;
        Empty_printf("中间删除时");
    }
    node* prev = NULL;
    node* current = *head;
    uint32_t count = 0;

    // 查找要删除的节点及其前驱
    while (current != NULL && count < index)
    {
        prev = current;
        current = current->next;
        count++;
    }

    if (current == NULL)
    {
        printf("索引超出范围！\n");
        return;
    }

    if (prev == NULL)
    {
        // 删除头节点
        *head = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    free(current->data);
    free(current);
    printf("中间删除成功！");
}
void students_print( node* head)                       /*打印*/
{
    if(head == NULL)
    {
        free(head);
        head = NULL;
        Empty_printf("打印时");
    }
    else
    {
        uint32_t numbre = 0;
        while(head != NULL)
        {
            if(printf("学号：%" PRIu32 " 姓名:%s 年龄:%hhu 性别:%s 电话：%zu 地址：%s 证件号：%zu 语文：%hhu 数学：%hhu 英语：%hhu 政治:%hhu 历史:%hhu 地理: %hhu 生物：%hhu 化学：%hhu 物理：%hhu\n",
            head->data->num, head->data->name, head->data->age, head->data->sex, head->data->tel, 
            head->data->address, head->data->identity_card, head->data->grades.Chinese, 
            head->data->grades.Math, head->data->grades.English, head->data->grades.politics, 
            head->data->grades.history, head->data->grades.geography, head->data->grades.biology, 
            head->data->grades.chemistry, head->data->grades.physics) > 0)
            {
                numbre++;
            }
            head = head->next;
        }
        printf("打印完成，本次打印共打印%u个数据\n",numbre);
    }
}
void file_reads( node** head )
{
    if( head == NULL )
    {
        free(*head);
        *head = NULL;
        Empty_printf("读取文件时");
    }
    FILE* fp =fopen(FILE_READS,"r");
    if( fp == NULL )
    {
        printf("无法打开文件，请确认文件地址:%s是否存在！\n",FILE_READS);
        system("pause");
        exit(-1);
    }
    students new ;
    uint8_t nums = 1;
    uint8_t l = 0;
    int8_t result = 0;
    uint8_t j = 0;
    while(result != EOF)
    {
        memset(&new, 0 ,sizeof(students));
        result = fscanf(fp, "%u %s %hhu %s %llu %s %llu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
        &new.num, new.name, &new.age, new.sex, &new.tel, new.address, &new.identity_card,
        &new.grades.Chinese, &new.grades.Math, &new.grades.English, &new.grades.politics,
        &new.grades.history, &new.grades.geography, &new.grades.biology, &new.grades.chemistry, &new.grades.physics);
        if(result == 16)
        {
            if((*head) == NULL)
            {
                *head = newnode();
                memcpy((*head)->data,&new,sizeof(students));
                l++;
            }
            else
            {
               node* temporary = *head;
                while(temporary->next != NULL)
                {
                    temporary = temporary->next;
                }
                temporary->next = newnode();
                memcpy(temporary->next->data,&new,sizeof(students));
                l++;
            }
        }
        else
        {
            if(result == EOF)
            {
                printf("读取完成，共读取%u个数据\n",l);
            }
            else
            {
                printf("读取第%u行出错\n",l);
            }
            continue;
        }
        com coms[] =
        {   
           {&new.num,              numjudgment},
           {new.name,              namejudgment},
           {&new.age,              agejudgment},
           {new.sex,               sexjudgment},
           {&new.tel,              teljudgment},
           {new.address,           NULL},
           {&new.identity_card,     IDnumberjudgment},
           {&new.grades.Chinese,    MainSubjectjudgment},
           {&new.grades.Math,       MainSubjectjudgment},
           {&new.grades.English,    MainSubjectjudgment},
           {&new.grades.politics,   Sub_sectionjudgment},
           {&new.grades.history,    Sub_sectionsjudgment},
           {&new.grades.geography,  Sub_sectionsjudgment},
           {&new.grades.biology,    Sub_sectionsjudgment},
           {&new.grades.chemistry,  Sub_sectionsjudgment},
           {&new.grades.physics,    Sub_sectionjudgment}
        };
        for(uint32_t i = 0; i < sizeof(coms) / sizeof(coms[0]); i++)
        {
            if(coms[i].Enteralimit != NULL)
            {
                if(coms[i].Enteralimit(coms[i].address) == FALSE)
                {
                    printf("读取第%u行时，第%u个数据非法！\n",l,nums);
                }
            }
            else
            {
                continue;
            }
            nums++;
        }
    }
    node* temporary = *head;
    while (temporary != NULL)
    {
        temporary = temporary->next;
        j++;
    }
    
    printf("读取完成，共读取%u个数据,现在链表共%u个数据\n",l,j);
    fclose(fp);
    fp = NULL;
}
void file_write(node* head)
{
    if( head == NULL )
    {
        free(head);
        head = NULL;
        Empty_printf("写入时\n");
    }
    FILE* fp = fopen(FILE_WRITES,"w");
    if(fp == NULL)
    {
        printf("写入时打开文件失败！\n");
        system("pause");
        exit(-1);
    }
    if( sizeof(students)*2 < NUMEBER_BUFFERS )
    {
        uint8_t count = 0;
        while ( head != NULL)
        {
            char buffer[NUMEBER_BUFFERS];
            int size = snprintf(buffer, sizeof(buffer),"%d %s %hhu %s %zu %s %zu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu\n",
            head->data->num,head->data->name,head->data->age,head->data->sex,head->data->tel,head->data->address,head->data->identity_card,
            head->data->grades.Chinese,head->data->grades.Math,head->data->grades.English,head->data->grades.politics,
            head->data->grades.history,head->data->grades.geography,head->data->grades.biology,head->data->grades.chemistry,
            head->data->grades.physics);
            //根据宏来改动
            if(size < 0)
            {
                printf("缓冲区存储第%u行时，存储失败！\n",count);
                continue;
            }
            else
            {
                fputs(buffer,fp);
                count++;
            }
            head = head->next;
        }
        printf("写入完成！当前写入成功人数为%u\n",count);
        fclose(fp);
        fp = NULL;
    }
    else
    {
        printf("缓存区过小，请调整\n");
    }
}
uint8_t list_length(const node* head)
{
    uint8_t count = 0;
    while(head != NULL) 
    {
        count++;
        head = head->next;
    }
    return count;
}
void release(node** head)
{
    while(*head != NULL)
    {
        node* temp = *head;
        *head = (*head)->next;
        if(temp->data != NULL)
        {
            free(temp->data);
            temp->data = NULL;
        }
        free(temp);
        temp = NULL;
    }
    printf("链表释放完成\n");
}
DWORD WINAPI text(LPVOID lpParam)
{
    printf("测试传入数为：%d\n",*(uint32_t*)lpParam);
    {
        node* head = NULL;
        // initialize(&head);
        // Tail_plugging(&head);
        // students_print(head);
        // release(&head);
        printf("读取\n");
        file_reads(&head);
        Sleep(1000);
        printf("写入\n");
        file_write(head);
        release(&head);
    }
    return 0;
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);/*vs 2022可以注释掉目的：控制台用UTF-8编码*/
    // DWORD threadId = 0;
    uint32_t threadI = 12;
    LPVOID threadId = &threadI;
    LPDWORD man = NULL;
    HANDLE manage = CreateThread(NULL,0,text,threadId,0,man);
    if(manage == NULL)
    {
        printf("创建线程失败！\n");
        system("pause");
        exit(-1);
    }
    WaitForSingleObject(manage,INFINITE);
    CloseHandle(manage);/*释放线程*/
    return 0;
}
