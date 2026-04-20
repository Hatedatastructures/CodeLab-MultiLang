/*双向链表例子*/
/*2025_2_17开始写，2_26粗略修改完bug*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <stdbool.h>

#define NAME_MAX 25
#define SEX_MAX 10
#define ADDRESS_MAX 100

#define NAME_MAX_S "%24s"
#define SEX_MAX_S "%9s"
#define ADDRESS_MAX_S "%99s"
static char* SEX_CHINA = "China";
// static char* SEX_ENGLISH = "English";
// 根据需求切换
static uint32_t MAX_NUM = 120000;
static uint8_t MAX_AGE = 100;
static float MAX_GRADE_MAIN = 150.0f;
static float MAX_GRADE_SECONDARY =70.0f;
typedef struct subjects
{
    float Chinese;                                    /*语文*/
    float Math;                                       /*数学*/
    float English;                                    /*英语*/
    float politics;                                   /*政治*/
    float history;                                    /*历史*/
    float geography;                                  /*地理*/
    float biology;                                    /*生物*/
    float chemistry;                                  /*化学*/
    float physics;                                    /*物理*/
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
typedef struct student_node
{
    students* data;
    struct student_node* prev;
    struct student_node* next;
}Sode;
typedef struct input
{
    char* Promptwords;
    char* Placeholders;
    void* address;
    void* Limit_parameters;
    BOOL(*Enteralimit)(const void*, const void*);
}inputs;
typedef struct str
{
    const char* Placeholding;
    const void* Compare_pointers_A;
    const void* Compare_pointers_B;
}strlist;
typedef struct compare
{
    void* address;
    BOOL(*Enteralimit)(const void*, const void*);
    void* limit;
}com;
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

static BOOL numjudgment(const void* num, const void* limit)
{
    if (limit != NULL)
    {
        if ((*(uint32_t*)num) <= *(uint32_t*)limit || *(uint32_t*)num != 0)
        {
            return TRUE;
        }
        else
        {
            printf("错误!,人数超过限制，限制人数：%u\n", *(uint32_t*)limit);
            return FALSE;
        }
    }
    else
    {
        if (*(uint32_t*)num == 0)
        {
            printf("错误!,人数不能为0\n");
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}
static BOOL namejudgment(const void* name, const void* limit)
{
    char* strname = (char*)name;
    unsigned char strnames = (unsigned char)strname[0];
    if (limit != NULL)
    {
        for (uint8_t i = 0; i < strlen(strname); i++)
        {
            if ((strnames & 0xF0) == 0xE0)
            {
                if (strlen(strname) >= 3)
                {
                    return TRUE;
                }
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < strlen(strname); i++)
        {
            if ((strnames & 0xF0) == 0xE0)
            {
                if (strlen(strname) >= 3)
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
static BOOL agejudgment(const void* age, const void* limit)
{
    if (limit != NULL)
    {
        if ((*(uint8_t*)age) <= *(uint8_t*)limit)
        {
            return TRUE;
        }
        else
        {
            printf("错误!,年龄超过限制，限制年龄：%u\n", *(uint8_t*)limit);
            return FALSE;
        }
    }
    else
    {
        if (*(uint8_t*)age == 0)
        {
            printf("错误!,年龄不能为0\n");
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}
static BOOL sexjudgment(const void* sex, const void* limit)
{
    if (limit != NULL)
    {
        if (strcmp((char*)limit, "China") == 0)
        {
            if ((strcmp((char*)sex, "男") == 0) || (strcmp((char*)sex, "女") == 0))
            {
                return TRUE;
            }
            else
            {
                printf("错误!,性别只能为男或女\n");
                return FALSE;
            }
        }
        else if (strcmp((char*)limit, "English") == 0)
        {
            if ((strcmp((char*)sex, "man") == 0) || (strcmp((char*)sex, "woman") == 0))
            {
                return TRUE;
            }
            else
            {
                printf("错误!,性别只能为man或woman\n");
                return FALSE;
            }
        }
    }
    else
    {
        if (strcmp(sex, "男") == 0 || strcmp(sex, "女") == 0 || strcmp(sex, "man") == 0 || strcmp(sex, "woman") == 0)
        {
            return TRUE;
        }
        else
        {
            printf("错误!,性别只能为男或女\n");
            return FALSE;
        }
    }
    return FALSE;
}
static BOOL teljudgment(const void* tel, const void* limit)
{
    uint64_t num = *(uint64_t*)tel;
    uint8_t len = 0;
    if (limit != NULL)
    {
        while (num > 0)
        {
            num /= 10;
            len++;
        }
        if (len != 11)
        {
            printf("错误!,电话号码必须为11位,当前%u位\n", len);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        while (num > 0)
        {
            num /= 10;
            len++;
        }
        if (len != 11)
        {
            printf("错误!,电话号码必须为11位,当前%u位\n", len);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}

static BOOL IDnumberjudgment(const void* identity_card, const void* limit)
{
    uint64_t num = *(uint64_t*)identity_card;
    uint8_t len = 0;
    while (num > 0)
    {
        num /= 10;
        len++;
    }
    if (limit != NULL)
    {
        if (len != 18)
        {
            printf("错误!,身份证号必须为18位,当前%u位\n", len);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        if (len != 18)
        {
            printf("错误!,身份证号必须为18位,当前%u位\n", len);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}

static BOOL MainSubjectjudgment(const void* grade, const void* limit)
{
    if (limit != NULL)
    {
        if ((*(float*)grade) <= *(float*)limit || *(float*)grade >= 0)
        {
            return TRUE;
        }
        else
        {
            printf("错误!,成绩超过限制，限制成绩：%.2f\n", *(float*)limit);
            return FALSE;
        }
    }
    else
    {
        if (*(float*)grade <= 0)
        {
            printf("错误!,成绩非法0\n");
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}
static BOOL stuinputs(char* Promptwords, char* Placeholders, void* address)
{
    while (1)
    {
        printf("%s", Promptwords);
        int result = scanf(Placeholders, address);
        if (result == 1)
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
static void Pointer_judgment(const void* judgment,const char* name)
{
    if (judgment == NULL)
    {
        if (name != NULL)
        {
            printf("%s 传入无效！\n", name);
        }
        else
        {
            printf(" 空指针错误！\n");
        }
        system("pause");
        exit(-1);
    }
}/*检查空指针*/
void stuinput(students* input)                            /*输入*/
{
    Pointer_judgment(input,"stuinput");
    inputs inputs[] =
    {
        {"请输入学号：",    "%u",          &input->num,       &MAX_NUM              , numjudgment },
        {"请输入姓名：",    NAME_MAX_S,    input->name,        NULL                 , namejudgment},
        {"请输入年龄：",    "%hhu",        &input->age,       &MAX_AGE              , agejudgment},
        {"请输入性别：",    SEX_MAX_S,     input->sex,        SEX_CHINA             , sexjudgment},
        {"请输入电话：",    "%llu",        &input->tel,        NULL                 , teljudgment},
        {"请输入地址：",    ADDRESS_MAX_S, input->address,     NULL                 , NULL},
        {"请输入证号：",    "%llu",  &input->identity_card,    NULL                 , IDnumberjudgment},
        {"请输入语文成绩：", "%f",    &input->grades.Chinese,  &MAX_GRADE_MAIN      , MainSubjectjudgment},
        {"请输入数学成绩：", "%f",    &input->grades.Math,     &MAX_GRADE_MAIN      , MainSubjectjudgment},
        {"请输入英语成绩：", "%f",    &input->grades.English,  &MAX_GRADE_MAIN      , MainSubjectjudgment},
        {"请输入政治成绩：", "%f",    &input->grades.politics, &MAX_GRADE_SECONDARY , MainSubjectjudgment},
        {"请输入历史成绩：", "%f",    &input->grades.history,  &MAX_GRADE_SECONDARY , MainSubjectjudgment},
        {"请输入地理成绩：", "%f",    &input->grades.geography,&MAX_GRADE_SECONDARY , MainSubjectjudgment},
        {"请输入生物成绩：", "%f",    &input->grades.biology,  &MAX_GRADE_SECONDARY , MainSubjectjudgment},
        {"请输入化学成绩：", "%f",    &input->grades.chemistry,&MAX_GRADE_SECONDARY , MainSubjectjudgment},
        {"请输入物理成绩：", "%f",    &input->grades.physics,  &MAX_GRADE_SECONDARY , MainSubjectjudgment},
    };
    for (uint8_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
    {
        while (1)
        {
            if (stuinputs(inputs[i].Promptwords, inputs[i].Placeholders, (void*)inputs[i].address) != FALSE)
            {
                if (inputs[i].Enteralimit != NULL)
                {
                    if (inputs[i].Enteralimit(inputs[i].address, inputs[i].Limit_parameters) == FALSE)
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


Sode* New_node()
{
    Sode* Node = (Sode*)malloc(sizeof(Sode));
    if(Node == NULL)
    {
        printf("内存分配失败！\n");
        exit(-1);
    }
    Node->data = (students*)malloc(sizeof(students));
    if (Node->data == NULL)
    {
        printf("内存分配失败！\n");
        exit(-1);
    }
    memset(Node->data, 0, sizeof(students));
    Node->prev = NULL;
    Node->next = NULL;
    return Node;
}/*新节点*/
void Head_insertion(Sode** head, Sode* Node)/*头插*/
{
    Pointer_judgment(head, "Head_insertion");
    Pointer_judgment(Node, "Head_insertion");
    if (*head == NULL)
    {
        *head = Node;
        Node->prev = NULL;
        Node->next = NULL;
    }
    else
    {
        (*head)->prev = Node;
        Node->next = *head;
        Node->prev = NULL;
        (*head) = Node;
    }
}/*头插*/
void Head_deletion(Sode** head)
{
    Pointer_judgment(head, "Head_deletion");
    if (*head != NULL)
    {
        (*head)->next->prev = NULL;
        Sode* Old_Nodes = (*head)->next;
        (*head)->next = NULL;
        free(*head);
        *head = Old_Nodes;
        printf("删除成功！\n");
    }
    else
    {
        printf("该链表为空,删除无效！\n");
        return;
    }
}/*头删*/
void Tail_insertion(Sode** head, Sode* Node)
{
    Pointer_judgment(head, "Tail_insertion");
    Pointer_judgment(Node, "Tail_insertion");
    if (*head == NULL)
    {
        *head = Node;
        Node->prev = NULL;
        Node->next = NULL;
    }
    else
    {
        Sode* current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = Node;
        Node->prev = current;
        Node->next = NULL;
    }
    printf("尾部插入成功！\n");
}/*尾插*/
void Tail_deletion(Sode** head)
{
    Pointer_judgment(head, "Tail_deletion");
    if (*head != NULL)
    {
        if ((*head)->next == NULL)
        {
            free(*head);
            *head = NULL;
        }
        else
        {
            Sode* Old_Nodes = *head;
            while (Old_Nodes->next->next != NULL)
            {
                Old_Nodes = Old_Nodes->next;
            }
            Old_Nodes->next->prev = NULL;
            free(Old_Nodes->next);
            Old_Nodes->next = NULL;
        }
        printf("删除成功！\n");
    }
    else
    {
        printf("该链表为空,无效删除！\n");
    }
}/*尾删*/

void Insert_middle(Sode** head, Sode* Node, uint32_t num)
{
    Pointer_judgment(head, "nsert_middle");
    Pointer_judgment(Node, "nsert_middle");
    if (*head == NULL || num == 0)
    {
        *head = Node;
        Node->prev = NULL;
        Node->next = NULL;
    }
    else
    {
        Sode* current = *head;
        for (uint32_t i = 0; i < num; i++)
        {
            if (current->next == NULL)
            {
                printf("插入位置无效！\n");
                return;
            }
            current = current->next;
        }
        Node->next = current;
        Node->prev = current->prev;
        current->prev->next = Node;
        current->prev = Node;
    }
    printf("插入成功！\n");
}/*中间插入*/

void Delete_middle(Sode** head, uint32_t num)
{
    Pointer_judgment(head, "Delete_middle");
    if (*head == NULL)
    {
        printf("该链表为空,删除无效！\n");
    }
    else if (num == 0)
    {
        Sode* Old_Nodes = *head;
        *head = (*head)->next;
        (*head)->prev = NULL;
        free(Old_Nodes->data);
        free(Old_Nodes);
        Old_Nodes = NULL;
        printf("删除成功！\n");
    }
    else
    {
        Sode* current = *head;
        for (uint32_t i = 0; i < num; i++)
        {
            if (current->next == NULL)
            {
                printf("删除位置无效！\n");
                return;
            }
            current = current->next;
        }
        current->prev->next = current->next;
        current->next->prev = current->prev;
        free(current->data);
        free(current);
        current = NULL;
        printf("删除成功！\n");
    }
}/*中间删除*/

Sode* Sode_Find(Sode* head, uint32_t num)
{
    Pointer_judgment(head, "Sode_Find");
    Sode* current = head;
    if( num != 0 )
    {
        for (uint32_t i = 0; i < (num - 1) && num != 0; i++)
        {
            if (current->next == NULL)
            {
                printf("查找位置无效！\n");
                return NULL;
            }
        current = current->next;
        }
        printf("查找成功！\n");
    }
    else
    {
        printf("查找位置无效！\n");
    }
    return current;
}/*查找目标位置地址*/

BOOL Sode_printf(const Sode* head)
{
    if(head == NULL)
    {
        return FALSE;
    }
    int32_t result = 0;
    result = printf("学号:%u "
                    "姓名：%s"
                    "年龄：%u"
                    "性别：%s"
                    "电话：%llu"
                    "地址：%s"
                    "证号：%llu"
                    "语文：%.2f"
                    "数学：%.2f"
                    "英语：%.2f" 
                    "政治：%.2f" 
                    "历史：%.2f" 
                    "地理：%.2f" 
                    "生物：%.2f"
                    "化学：%.2f" 
                    "物理：%.2f\n",
                    head->data->num,
                    head->data->name,
                    head->data->age,
                    head->data->sex,
                    head->data->tel,
                    head->data->address,
                    head->data->identity_card,
                    head->data->grades.Chinese,
                    head->data->grades.Math,
                    head->data->grades.English,
                    head->data->grades.politics,
                    head->data->grades.history,
                    head->data->grades.geography,
                    head->data->grades.biology,
                    head->data->grades.chemistry,
                    head->data->grades.physics);
    if(result == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}/*打印*/

BOOL Sode_revise(Sode** head, uint32_t num)
{
    Pointer_judgment(head, "Sode_revise");
    Pointer_judgment(*head, "Sode_revise");
    Sode* current = *head;
    for (uint32_t i = 0; i < num; i++)
    {
        if (current->next == NULL)
        {
            printf("修改位置无效！\n");
            return FALSE;
        }
        current = current->next;
    }
    students* temporary = (students*)malloc(sizeof(students));
    Pointer_judgment(temporary, "修改");
    free(current->data);
    current->data = temporary;
    printf("修改成功！\n");
    return TRUE;
}/*修改指定位置数据*/

int64_t compare (const void* a, const void* b ,const char* c)
{
    strlist list[] = 
        {
            {"学号", &((students*)a)->num,              &((students*)b)->num},
            {"年龄", &((students*)a)->age,              &((students*)b)->age},
            {"性别", &((students*)a)->sex,              &((students*)b)->sex},
            {"证号", &((students*)a)->identity_card,    &((students*)b)->identity_card},
            {"语文", &((students*)a)->grades.Chinese,   &((students*)b)->grades.Chinese},
            {"数学", &((students*)a)->grades.Math,      &((students*)b)->grades.Math},
            {"英语", &((students*)a)->grades.English,   &((students*)b)->grades.English},
            {"政治", &((students*)a)->grades.politics,  &((students*)b)->grades.politics},
            {"历史", &((students*)a)->grades.history,   &((students*)b)->grades.history},
            {"地理", &((students*)a)->grades.geography, &((students*)b)->grades.geography},
            {"生物", &((students*)a)->grades.biology,   &((students*)b)->grades.biology},
            {"化学", &((students*)a)->grades.chemistry, &((students*)b)->grades.chemistry},
            {"物理", &((students*)a)->grades.physics,   &((students*)b)->grades.physics}
        };
    for (uint8_t i = 0; i < sizeof(list) / sizeof(list[0]); i++)
    {
        if (strcmp(list[i].Placeholding, c) == 0)
        {
            return (*(int64_t*)list[i].Compare_pointers_A - *(uint64_t*)list[i].Compare_pointers_B);
        }
    }
    printf("输入排序字符不合法！\n");
    return INT64_MAX;
}
BOOL Insert_sorting(Sode** head, char* data)
{
    Pointer_judgment(head, "Insert_sorting");
    if(*head == NULL)
    {
        return FALSE;
    }
    uint64_t list_count = 0;
    uint64_t list_counts = 0;
    Sode* Ephemeral_nodes = NULL;
    Sode* current = *head;
    uint8_t number = 0;
    Sode* count = *head;
    while(count->next != NULL)
    {
        count = count->next;
        list_count++;
    }
    while(current != NULL)
    {
        Sode* temp = Ephemeral_nodes;
        Sode* slow = NULL;
        if(Ephemeral_nodes == NULL || compare(Ephemeral_nodes->data, current->data, data) >= 0) /*头插*/
        {
            if(Ephemeral_nodes == NULL)
            {
                Ephemeral_nodes = current;
                Ephemeral_nodes->next = NULL;
                Ephemeral_nodes->prev = NULL;
                number = 1;
                list_counts++;
            }
            else
            {
                while(temp != NULL)
                {
                    slow = temp;
                    temp = temp->prev;
                    if(compare(temp->data, current->data, data) >= 0)
                    {
                        temp->prev = current;
                        temp->next = slow;
                        temp = temp->prev;/*防止重复插入*/
                        number = 1;
                        list_counts++;
                        break;
                    }
                }
                temp->prev = NULL;
            }
            if(number != 1)
            {
                printf("插入前部失败！\n");
                return FALSE;
            }
        }
        else
        {
            while(temp != NULL)
            {
                slow = temp ;
                temp = temp->next;
                if(compare(Ephemeral_nodes->next->data, current->data, data) <= 0||compare(Ephemeral_nodes->data, current->data, data)!= INT64_MAX)
                {
                    temp->next = current;
                    temp->prev = slow;
                    temp = temp->next;/*防止重复插入*/
                    list_counts++;
                    number = 1;
                    break;
                }
            }
            temp->next = NULL;
            if(number == 1)
            {
                printf("插入后部失败！\n");
                return FALSE;
            }
        }
        if(current->next != NULL)
        {
           current = current->next;
        }
    }
    if(list_counts != list_count)
    {
        printf("插入排序中bug！\n");
        return FALSE;
    }
    else
    {
        printf("插入排序成功！\n");
        *head = Ephemeral_nodes;
        return TRUE;
    }
    printf("插入排序成功！\n");
}/*插入排序*/

void SodeFile_read(Sode** head, char* file_name)
{
    Pointer_judgment(head, "SodeFile_read");
    FILE* fp = fopen(file_name, "r");
    Pointer_judgment(&fp, "SodeFile_read_fp");
    students* Ephemeral_Nodes = (students*) malloc(sizeof(students));
    if(Ephemeral_Nodes == NULL)
    {
        printf("Ephemeral_Nodes malloc failed!\n");
        return;
    }
    int8_t number = 0;
    uint64_t Number_Nodes = 0;
    while(number != EOF)
    {
        memset(Ephemeral_Nodes, 0, sizeof(students));
        number = fscanf(fp,"%u %s %hhu %s %llu %s %llu %f %f %f %f %f %f %f %f %f",
                &Ephemeral_Nodes->num, Ephemeral_Nodes->name, &Ephemeral_Nodes->age, Ephemeral_Nodes->sex,
                &Ephemeral_Nodes->tel, Ephemeral_Nodes->address, &Ephemeral_Nodes->identity_card,
                &Ephemeral_Nodes->grades.Chinese,&Ephemeral_Nodes->grades.Math,&Ephemeral_Nodes->grades.English,
                &Ephemeral_Nodes->grades.politics,&Ephemeral_Nodes->grades.history,&Ephemeral_Nodes->grades.geography,
                &Ephemeral_Nodes->grades.biology,&Ephemeral_Nodes->grades.chemistry,&Ephemeral_Nodes->grades.physics
                );/*%u是期待的是uint32_t*的数据,其他的同理*/
        if(number == 16)
        {
            if((*head) == NULL)
            {
                *head = New_node();
                memcpy((*head)->data, Ephemeral_Nodes, sizeof(students));
                Number_Nodes++;
            }
            else
            {
                Sode* temp = *head;
                while(temp->next != NULL)
                {
                    temp = temp->next;
                }
                temp->next = New_node();
                memcpy(temp->next->data, Ephemeral_Nodes, sizeof(students));
                Number_Nodes++;
            }
        }
        else
        {
            if(number == EOF || number != 16)
            {
                continue;
            }
        }
        com coms[] =
        {
            {&Ephemeral_Nodes->num,              numjudgment ,        &MAX_NUM},
            {Ephemeral_Nodes->name,              namejudgment,        NULL},
            {&Ephemeral_Nodes->age,              agejudgment,         &MAX_AGE},
            {Ephemeral_Nodes->sex,               sexjudgment,         SEX_CHINA},
            {&Ephemeral_Nodes->tel,              teljudgment,         NULL},
            {Ephemeral_Nodes->address,           NULL,                NULL},
            {&Ephemeral_Nodes->identity_card,    IDnumberjudgment,    NULL},
            {&Ephemeral_Nodes->grades.Chinese,   MainSubjectjudgment, &MAX_GRADE_MAIN},
            {&Ephemeral_Nodes->grades.Math,      MainSubjectjudgment, &MAX_GRADE_MAIN},
            {&Ephemeral_Nodes->grades.English,   MainSubjectjudgment, &MAX_GRADE_MAIN},
            {&Ephemeral_Nodes->grades.politics,  MainSubjectjudgment, &MAX_GRADE_SECONDARY},
            {&Ephemeral_Nodes->grades.history,   MainSubjectjudgment, &MAX_GRADE_SECONDARY},
            {&Ephemeral_Nodes->grades.geography, MainSubjectjudgment, &MAX_GRADE_SECONDARY},
            {&Ephemeral_Nodes->grades.biology,   MainSubjectjudgment, &MAX_GRADE_SECONDARY},
            {&Ephemeral_Nodes->grades.chemistry, MainSubjectjudgment, &MAX_GRADE_SECONDARY},
            {&Ephemeral_Nodes->grades.physics,   MainSubjectjudgment, &MAX_GRADE_SECONDARY},
        };
        for(uint8_t i = 0; i < sizeof(coms)/sizeof(com); i++)/*计算结构体大小*/
        {
            if(coms[i].Enteralimit != NULL)
            {
                if(coms[i].Enteralimit(coms[i].address, coms[i].limit) == FALSE)
                {
                    printf("数据异常%lld行%hhu\n",Number_Nodes, i);/*打印错误*/
                }
            }
            else
            {
                continue;
            }
        }
    }
    printf("文件共读取到 %llu\n", Number_Nodes);
    fclose(fp);
    fp = NULL;
}
void SodeFile_write(Sode** head, char* file_name)
{
    Pointer_judgment(head, "SodeFile_write");
    FILE* fp = fopen(file_name, "w");
    Pointer_judgment(fp, "SodeFile_write_fp");
    Sode* temp_Node = *head;
    uint64_t Number_Nodes = 0;
    while(temp_Node != NULL)
    {
        uint8_t numbersum = fprintf(fp, "%u %s %hhu %s %llu %s %llu %f %f %f %f %f %f %f %f %f\n",
            temp_Node->data->num, temp_Node->data->name, temp_Node->data->age, temp_Node->data->sex,
            temp_Node->data->tel, temp_Node->data->address, temp_Node->data->identity_card,
            temp_Node->data->grades.Chinese, temp_Node->data->grades.Math, temp_Node->data->grades.English,
            temp_Node->data->grades.politics, temp_Node->data->grades.history, temp_Node->data->grades.geography,
            temp_Node->data->grades.biology, temp_Node->data->grades.chemistry, temp_Node->data->grades.physics
        );
        temp_Node = temp_Node->next;
        if(numbersum != 0)
        {
            Number_Nodes++;
        }
        else
        {
            printf("数据写入异常%llu行\n", Number_Nodes);
        }
    }
    printf("文件共写入 %llu行\n", Number_Nodes);
    fclose(fp);
    fp = NULL;
}
void release(Sode** head)
{
    if(head == NULL || *head == NULL)
    {
        return;
    }    
    Sode* temp_Node = *head;
    while(temp_Node != NULL)
    {
        Sode* temp = temp_Node;
        temp_Node = temp_Node->next;
        free(temp->data);
        temp->data = NULL;
        free(temp);
        temp = NULL;
    }
    *head = NULL;
    printf("链表已释放\n");
}

void display_menu() 
{
    printf("\n===== 双向链表管理系统 =====\n");
    printf("[0] 退出系统\t [1] 头部插入\n");
    printf("[2] 尾部插入\t [3] 头部删除\n");
    printf("[4] 尾部删除\t [5] 中间插入\n");
    printf("[6] 中间删除\t [7] 节点修改\n");
    printf("[8] 节点查找\t [9] 链表排序\n");
    printf("[10]打印链表\t [11]释放链表\n");
    printf("[12]读取文件\t [13]保存文件\n");
    printf("============================\n");
    printf("请输入选择(0-13): ");
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    Sode* head = NULL;

    while (true)
    {
        display_menu();
        int choice;
        if (scanf("%d", &choice) != 1) // 检查输入是否为整数
        {  
            printf("输入格式错误！\n");
            while (getchar() != '\n'); 
            continue;
        }
        switch (choice)
        {
            case EXITS:
                printf("退出系统\n");
                release(&head);
                return 0;

            case HEAD_INSERTION:
                {
                    Sode* temp_Node = New_node();
                    stuinput(temp_Node->data);
                    Head_insertion(&head, temp_Node);
                }
                break;

            case TAIL_INSERTION:
                {
                    Sode* temp_Node = New_node();
                    stuinput(temp_Node->data);
                    Tail_insertion(&head, temp_Node);
                }
                break;

            case HEAD_DELETION:
                Head_deletion(&head);
                break;

            case TAIL_DELETION:
                Tail_deletion(&head);
                break;

            case MIDDLE_INSERTION:
                {
                    uint32_t position;
                    printf("请输入插入的位置: ");
                    scanf("%u", &position);
                    while (getchar() != '\n');
                    Sode* temp_Node = New_node();
                    stuinput(temp_Node->data);
                    Insert_middle(&head, temp_Node, position);
                }
                break;

            case MIDDLE_DELETION:
                {
                    uint32_t position;
                    printf("请输入删除的位置: ");
                    scanf("%u", &position);
                    while (getchar() != '\n');
                    Delete_middle(&head, position);
                }
                break;

            case MIDDLE_REVISE:
                {
                    uint32_t position;
                    printf("请输入修改的位置: ");
                    scanf("%u", &position);
                    while (getchar() != '\n');
                    Sode_revise(&head, position);
                }
                break;

            case POSITION_FIND:
                {
                    uint32_t position;
                    printf("0为无效值请输入查找的位置: ");
                    scanf("%u", &position);
                    while (getchar() != '\n');
                    if(position == 0)
                    {
                        printf("无效值\n");
                    }
                    else
                    {
                        Sode* found = Sode_Find(head, position);
                        if (found != NULL)
                        {
                            Sode_printf(found);
                        }
                    }
                }
                break;

            case SORTING:
                {
                    printf("链表排序,请输入要排序的类别（学号、年龄等）：");
                    char sort_type[20];
                    scanf("%s", sort_type);
                    while (getchar() != '\n');
                    Insert_sorting(&head, sort_type);
                }
                break;

            case PRINT_LIST:
                {
                    Sode* current = head;
                    while (current != NULL)
                    {
                        Sode_printf(current);
                        current = current->next;
                    }
                }
                break;

            case RELEASE_LIST:
                release(&head);
                break;

            case FILE_READ:
                {
                    char file_name[256];
                    printf("请输入文件名并且已经转义: ");
                    scanf("%s", file_name);
                    while (getchar() != '\n');
                    SodeFile_read(&head, file_name);
                }
                break;

            case FILE_WRITE:
                {
                    char file_name[256];
                    printf("请输入文件名并且已经转义: ");
                    scanf("%s", file_name);
                    while (getchar() != '\n');
                    SodeFile_write(&head, file_name);
                }
                break;

            default:
                printf("无效选择，请重新输入。\n");
                break;
        }
    }
    return 0;
}