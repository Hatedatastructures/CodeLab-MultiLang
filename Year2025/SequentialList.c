/*顺序表*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

#define NAME_MAX 20                  /*姓名*/
#define ADDRESS_MAX 200              /*地址*/
#define SEX_MAX 10                   /*性别*/
#define Capacity_increase_multiple 2 /*倍数*/

#define Write_address "C:\\Users\\C1373\\Desktop\\students_data.txt"
#define Read_address "C:\\Users\\C1373\\Desktop\\students_data.txt"

typedef unsigned char subjects;
typedef unsigned int size_int;
enum medu
{
    EXITS,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    ELEVEN,
    TWELVE,
    THIRTEEN,
    FOURTEEN,
    FIFTEEN,
};

// 定义函数指针类型
typedef int (*cmp_func_t)(const void *, const void *);

typedef struct student
{
    subjects Chinese;   /*语文*/
    subjects Math;      /*数学*/
    subjects English;   /*英语*/
    subjects politics;  /*政治*/
    subjects history;   /*历史*/
    subjects geography; /*地理*/
    subjects biology;   /*生物*/
    subjects chemistry; /*化学*/
} s;
typedef struct students
{
    int num;                   /*学号*/
    char name[NAME_MAX];       /*姓名*/
    int age;                   /*年龄*/
    char sex[SEX_MAX];         /*性别*/
    long long tel;             /*电话*/
    char address[ADDRESS_MAX]; /*地址*/
    long long identity_card;   /*证号*/
    s wing;                    /*成绩*/
} stu;

typedef struct studentss
{
    stu *arr;
    size_t size;     /*当前元素个数*/
    size_t capacity; /*当前容量*/
} st;


// 初始化函数
void initialize(st *s);

// 增容函数
void Capacity(st *p);

// 删除函数
void Delete(st *p, int index);

// 头插函数
void stuHead_insertion(st *p, stu *x);

// 头删函数
void stuHeader_removal(st *p);

// 中插函数
void stuInsert_in_the_middle(st *p, stu *x, size_t pos);

// 中删函数
void stuIntermediate_deletion(st *p, size_t pos);

// 尾插函数
void stuTail_insertion(st *p, stu *x);

// 尾删函数
void stuTail_deletion(st *p);

// 打印函数
void stuprint(const st *p);

// 读取文件函数
void Read_file(FILE *fp, st *p);

// 写入文件函数
void Write_file(FILE *fp, const st *p);

// 学生信息输入函数
stu *stuinput();

// 排序选择函数
size_t sorts();

// 排序函数
void my_sort(const  st *p, size_t index);

// 显示菜单函数
void display_menu();

// 比较函数
static int cmp_num(const void *a, const void *b);

static int cmp_name(const void *a, const void *b);

static int cmp_age(const void *a, const void *b);

static int cmp_sex(const void *a, const void *b);

static int cmp_tel(const void *a, const void *b);

static int cmp_address(const void *a, const void *b);

static int cmp_identity_card(const void *a, const void *b);

static int cmp_Chinese(const void *a, const void *b);

static int cmp_Math(const void *a, const void *b);

static int cmp_English(const void *a, const void *b);

static int cmp_politics(const void *a, const void *b);

static int cmp_history(const void *a, const void *b);

static int cmp_geography(const void *a, const void *b);

static int cmp_biology(const void *a, const void *b);

static int cmp_chemistry(const void *a, const void *b);

// 输入验证函数
static BOOL namejudgment(const void *name);

static BOOL agejudgment(const void *age);

static BOOL sexjudgment(const void *sex);

static BOOL teljudgment(const void *tel);

static BOOL IDnumberjudgment(const void *identity_card);

static BOOL MainSubjectjudgment(const void *MainSubject);

static BOOL Sub_sectionjudgment(const void *Sub_section);

static BOOL Sub_sectionsjudgment(const void *Sub_sections);

static BOOL stuinputs(char *Promptwords,const char *Placeholders, void *address);

static int cmp_num(const void *a, const void *b)
{
    return ((stu *)a)->num - ((stu *)b)->num;
}
static int cmp_name(const void *a, const void *b)
{
    return strcmp(((stu *)a)->name, ((stu *)b)->name);
}
static int cmp_age(const void *a, const void *b)
{
    return ((stu *)a)->age - ((stu *)b)->age;
}
static int cmp_sex(const void *a, const void *b)
{
    return strcmp(((stu *)a)->sex, ((stu *)b)->sex);
}
static int cmp_tel(const void *a, const void *b)
{
    return (int)(((stu *)a)->tel - ((stu *)b)->tel);
}
static int cmp_address(const void *a, const void *b)
{
    return strcmp(((stu *)a)->address, ((stu *)b)->address);
}
static int cmp_identity_card(const void *a, const void *b)
{
    return (int)(((stu *)a)->identity_card - ((stu *)b)->identity_card);
}
static int cmp_Chinese(const void *a, const void *b)
{
    return ((stu *)a)->wing.Chinese - ((stu *)b)->wing.Chinese;
}
static int cmp_Math(const void *a, const void *b)
{
    return ((stu *)a)->wing.Math - ((stu *)b)->wing.Math;
}
static int cmp_English(const void *a, const void *b)
{
    return ((stu *)a)->wing.English - ((stu *)b)->wing.English;
}
static int cmp_politics(const void *a, const void *b)
{
    return ((stu *)a)->wing.politics - ((stu *)b)->wing.politics;
}
static int cmp_history(const void *a, const void *b)
{
    return ((stu *)a)->wing.history - ((stu *)b)->wing.history;
}
static int cmp_geography(const void *a, const void *b)
{
    return ((stu *)a)->wing.geography - ((stu *)b)->wing.geography;
}
static int cmp_biology(const void *a, const void *b)
{
    return ((stu *)a)->wing.biology - ((stu *)b)->wing.biology;
}
static int cmp_chemistry(const void *a, const void *b)
{
    return ((stu *)a)->wing.chemistry - ((stu *)b)->wing.chemistry;
}
static BOOL namejudgment(const void *name)
{
    char *p = (char *)name;
    if (strlen(p) > 20)
    {
        printf("输入错误!,该限制不能超过20个字符!\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL agejudgment(const void *age)
{
    if ((*(int *)age) == 0 || (*(int *)age >= 150))
    {
        printf("输入错误!,年龄不能大于等于150!\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL sexjudgment(const void *sex)
{
    if (strcmp(sex, "男") == 0 || strcmp(sex, "女") == 0 || strcmp(sex, "man") == 0 || strcmp(sex, "woman") == 0)
    {
        return TRUE;
    }
    else
    {
        printf("输入错误!,性别只能为男或女\n");
        return FALSE;
    }
}

static BOOL teljudgment(const void *tel)
{
    long long *p = (long long *)tel;
    subjects len = 1;
    while (1)
    {
        if (*p > 10)
        {
            *p /= 10;
            len++;
        }
        else
        {
            break;
        }
    }
    if (len != 11)
    {
        printf("输入错误!,电话号码必须为11位,当前%d位\n", len);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL IDnumberjudgment(const void *identity_card)
{
    long long *p = (long long *)identity_card;
    subjects len = 1;
    while (1)
    {
        if (*p > 10)
        {
            *p /= 10;
            len++;
        }
        else
        {
            break;
        }
    }
    if (len != 18)
    {
        printf("输入错误!,身份证号必须为18位,当前%d位\n", len);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL MainSubjectjudgment(const void *MainSubject)
{
    if ((*(subjects *)MainSubject) > 150)
    {
        printf("输入错误!,成绩不合理\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL Sub_sectionjudgment(const void *Sub_section)
{
    if ((*(subjects *)Sub_section) > 70)
    {
        printf("输入错误!,成绩不合理\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL Sub_sectionsjudgment(const void *Sub_sections)
{
    if ((*(subjects *)Sub_sections) > 50)
    {
        printf("输入错误!,成绩不合理\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static BOOL stuinputs(char *Promptwords, const char *Placeholders, void *address)
{
    while (1)
    {
        printf("%s", Promptwords);
        if (scanf_s(Placeholders, address) != EOF)
        {
            while (getchar() != '\n')
            {
                ;
            }// 清空输入缓冲区
            return TRUE;
        }
        else
        {
            printf("输入错误!\n");
            while (getchar() != '\n')
            {
               ;
            }// 清空输入缓冲区
            continue;
        }
    }
}

static cmp_func_t cmp_funcs[] =
    {
        NULL,
        cmp_num,
        cmp_name,
        cmp_age,
        cmp_sex,
        cmp_tel,
        cmp_address,
        cmp_identity_card,
        cmp_Chinese,
        cmp_Math,
        cmp_English,
        cmp_politics,
        cmp_history,
        cmp_geography,
        cmp_biology,
        cmp_chemistry
    }; // 函数指针数组

void initialize(st *s) /*初始化*/
{
    assert(s != NULL);
    const size_t INITIAL_CAPACITY = 10;

    s->arr = (stu *)malloc(sizeof(stu) * INITIAL_CAPACITY);
    if (s->arr == NULL)
    {
        fprintf(stderr, "内存分配失败: 无法分配 %zu 字节\n", sizeof(stu) * INITIAL_CAPACITY);
        exit(EXIT_FAILURE); // 直接退出程序
    }

    s->size = 0;
    s->capacity = INITIAL_CAPACITY;
    memset(s->arr, 0, sizeof(stu) * INITIAL_CAPACITY); // 初始化内存
}

void Capacity(st *p) /*增容*/
{
    assert(p != NULL);
    if (p->size < p->capacity)
    {
        return;
    }
    size_t new_capacity = p->capacity * Capacity_increase_multiple;
    if (new_capacity <= p->capacity)
    { // 检查溢出
        fprintf(stderr, "容量溢出: 当前容量 %zu, 新容量 %zu\n", p->capacity, new_capacity);
        exit(EXIT_FAILURE);
    }

    stu *new_arr = (stu *)realloc(p->arr, sizeof(stu) * new_capacity);
    if (new_arr == NULL)
    {
        fprintf(stderr, "内存重分配失败: 无法分配 %zu 字节\n", sizeof(stu) * new_capacity);
        exit(EXIT_FAILURE);
    }

    p->arr = new_arr;
    p->capacity = new_capacity;

    // 初始化新分配的内存
    memset(p->arr + p->size, 0, sizeof(stu) * (new_capacity - p->size));
}

// 修改 Delete 函数，不分配内存，只进行元素的删除操作
void Delete(st *p, int index)
{
    assert(p != NULL);
    if (p->size == 0)
    {
        printf("当前人数为 0,无法删除!\n");
        return;
    }
    if (index < 0 || index >= (int)(p->size))
    {
        printf("无效的索引!\n");
        return;
    }

    // 将后面的元素前移
    for (int i = index; i < ((int)(p->size)) - 1; i++)
    {
        p->arr[i] = p->arr[i + 1];
    }
    p->size--;

    // 如果容量过大，可以适当缩小
    if (p->size < p->capacity / 4 && p->capacity > 10)
    {
        size_t new_capacity = p->capacity / 2;
        stu *new_arr = (stu *)realloc(p->arr, sizeof(stu) * new_capacity);
        if (new_arr == NULL)
        {
            fprintf(stderr, "内存重分配失败: 无法分配 %zu 字节\n", sizeof(stu) * new_capacity);
            // 保持原有容量，不缩小
            return;
        }
        p->arr = new_arr;
        p->capacity = new_capacity;

        // 初始化新分配的内存
        memset(p->arr + p->size, 0, sizeof(stu) * (new_capacity - p->size));
    }
}

void stuHead_insertion(st *p, stu *x) /*头插*/
{
    assert(p != NULL);
    Capacity(p);
    for (int i = ((int)(p->size)) - 1; i >= 0; i--)
    {
        p->arr[i + 1] = p->arr[i];
    }
    p->arr[0] = *x;
    p->size++;
    free(x);
}

void stuHeader_removal(st *p) /*头删*/
{
    assert(p != NULL);
    Delete(p, 0);
}

void stuInsert_in_the_middle(st *p, stu *x, size_t pos) /*中插*/
{
    assert(p != NULL);
    assert(x != NULL);
    if ( pos > p->size || p->size == 0)
    {
        printf("当前没有人数或插入位置无效!\n");
        free(x);
        return;
    }
    if (p->size >= p->capacity)
    {
        Capacity(p);
    }
    p->size++;
    for (size_t i = p->size - 1; i > pos; i--)
    {
        p->arr[i] = p->arr[i - 1];
    }
    p->arr[pos] = *x;
    free(x);
}

void stuIntermediate_deletion(st *p, size_t pos) /*中删*/
{
    assert(p != NULL);
    if (pos >= p->size || p->size == 0)
    {
        printf("当前没有人数或删除位置无效!\n");
        return;
    }
    Delete(p, (int)pos);
    for (size_t i = pos; i < p->size; i++)
    {
        p->arr[i] = p->arr[i + 1];
    }
    printf("删除成功!\n");
}

void stuTail_insertion(st *p, stu *x) /*尾插*/
{
    assert(p != NULL);
    if (x == NULL)
    {
        fprintf(stderr, "错误: 传入的学生指针为空\n");
        return;
    }

    // 检查容量并尝试扩容
    if (p->size >= p->capacity)
    {
        size_t new_capacity = p->capacity * Capacity_increase_multiple;
        stu *new_arr = (stu *)realloc(p->arr, sizeof(stu) * new_capacity);
        if (new_arr == NULL)
        {
            fprintf(stderr, "内存分配失败: 无法分配 %zu 字节\n", sizeof(stu) * new_capacity);
            free(x); // 释放传入的学生结构体
            return;
        }
        p->arr = new_arr;
        p->capacity = new_capacity;

        // 初始化新分配的内存
        memset(p->arr + p->size, 0, sizeof(stu) * (new_capacity - p->size));
    }

    // 插入新学生
    p->arr[p->size] = *x;
    p->size++;
    free(x);
    printf("插入成功!\n");
}

void stuTail_deletion(st *p) /*尾删*/
{
    assert(p != NULL);
    if (p->size == 0)
    {
        printf("当前没有学生信息!\n");
        return;
    }

    printf("确定要删除最后一名学生吗？(Y/N) ");
    char confirm;
    scanf_s("%c", &confirm, 1);
    if (toupper(confirm) == 'Y')
    {
        Delete(p, (int)(p->size - 1));
        printf("删除成功!\n");
    }
    else
    {
        printf("取消删除操作\n");
    }
}

void stuprint(const st *p) /*打印*/
{
    assert(p != NULL);
    if (p->size == 0)
    {
        printf("当前人数为 0!请输入信息！\n");
        return;
    }
    printf("当前人数:%zu\n", p->size);
    for (size_t i = 0; i < p->size; i++)
    {
        printf("学号%d 姓名：%s 年龄：%d 性别：%s 电话：%lld 地址：%s 证号：%lld"
               "语文：%d 数学:%d 英语:%d 政治:%d 历史:%d 地理:%d 生物:%d 化学：%d\n",
               p->arr[i].num, p->arr[i].name, p->arr[i].age, p->arr[i].sex, p->arr[i].tel,
               p->arr[i].address, p->arr[i].identity_card, p->arr[i].wing.Chinese,
               p->arr[i].wing.Math, p->arr[i].wing.English, p->arr[i].wing.politics,
               p->arr[i].wing.history, p->arr[i].wing.geography, p->arr[i].wing.biology,
               p->arr[i].wing.chemistry);
    }
}

void Read_file(FILE *fp, st *p) /*读取*/
{
    assert(p != NULL);
    assert(fp != NULL);

    initialize(p);

    stu stg;
    int line_num = 1;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (line[0] == '\n' || line[0] == '#')
        {
            line_num++;
            continue;
        }
        int result = sscanf(line, "%d %19s %d %9s %lld %99s %lld %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
                            &stg.num, stg.name, &stg.age, stg.sex, &stg.tel, stg.address, &stg.identity_card,
                            &stg.wing.Chinese, &stg.wing.Math, &stg.wing.English, &stg.wing.politics,
                            &stg.wing.history, &stg.wing.geography, &stg.wing.biology, &stg.wing.chemistry);

        if (result != 15)
        {
            fprintf(stderr, "文件格式错误: 第 %d 行\n", line_num);
            continue;
        }
        if (stg.age < 0 || stg.age > 150)
        {
            fprintf(stderr, "年龄无效: 第 %d 行\n", line_num);
            continue;
        }

        p->arr[p->size] = stg;
        p->size++;
        Capacity(p);
        line_num++;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "文件读取错误: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("成功读取 %zu 条记录\n", p->size);
    fclose(fp); // 关闭文件
}
void Write_file(FILE *fp, const st *p) /*写入*/
{
    assert(p != NULL);
    if (p->size == 0)
    {
        printf("当前人数为 0,无法写入!\n");
        exit(-1);
    }
    else
    {
        for (size_t i = 0; i < p->size; i++)
        {
            fprintf(fp, "%8d %15s %2d %6s %11lld %15s %18lld %3d %3d %3d %3d %3d %3d %3d %3d\n",
                    p->arr[i].num, p->arr[i].name, p->arr[i].age, p->arr[i].sex, p->arr[i].tel,
                    p->arr[i].address, p->arr[i].identity_card, p->arr[i].wing.Chinese,
                    p->arr[i].wing.Math, p->arr[i].wing.English, p->arr[i].wing.politics,
                    p->arr[i].wing.history, p->arr[i].wing.geography, p->arr[i].wing.biology,
                    p->arr[i].wing.chemistry);
        }
        printf("\n写入成功!\n");
        printf("当前写入人数:%zu\n", p->size);
        printf("当容量为：%zu\n", p->capacity);
    }
    fclose(fp); // 关闭文件
}

stu *stuinput() /*输入*/
{
    stu *input = (stu *)malloc(sizeof(stu));
    if (input == NULL)
    {
        perror("malloc");
        printf("内存申请失败!\n");
        system("pause");
        return NULL;
    }
    memset(input, 0, sizeof(stu));
    printf("请输入学生信息:\n\a");
    struct input
    {
        char *Promptwords;
        char *Placeholders;
        void *address;
        BOOL (*Enteralimit)(const void *);
    } inputs[] =
        {
            {"请输入学号：", "%d", &input->num, NULL},
            {"请输入姓名：", "%s", &input->name, namejudgment},
            {"请输入年龄：", "%d", &input->age, agejudgment},
            {"请输入性别：", "%s", &input->sex, sexjudgment},
            {"请输入电话：", "%lld", &input->tel, teljudgment},
            {"请输入地址：", "%s", &input->address, NULL},
            {"请输入证号：", "%lld", &input->identity_card, IDnumberjudgment},
            {"请输入语文成绩：", "%d", &input->wing.Chinese, MainSubjectjudgment},
            {"请输入数学成绩：", "%d", &input->wing.Math, MainSubjectjudgment},
            {"请输入英语成绩：", "%d", &input->wing.English, MainSubjectjudgment},
            {"请输入政治成绩：", "%d", &input->wing.politics, Sub_sectionjudgment},
            {"请输入历史成绩：", "%d", &input->wing.history, Sub_sectionsjudgment},
            {"请输入地理成绩：", "%d", &input->wing.geography, Sub_sectionsjudgment},
            {"请输入生物成绩：", "%d", &input->wing.biology, Sub_sectionsjudgment},
            {"请输入化学成绩：", "%d", &input->wing.chemistry, Sub_sectionsjudgment},
        };
    for (subjects i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
    {
        while (1)
        {
            if (stuinputs(inputs[i].Promptwords, inputs[i].Placeholders, (void *)(inputs[i].address)) != FALSE)
            {
                if (inputs[i].Enteralimit != NULL)
                {
                    if (inputs[i].Enteralimit((void *)inputs[i].address) == FALSE)
                    {
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
    return input;
}
size_t sorts()
{
    printf("请选择排序方式:\n\a");
    printf("1  按学号排序\n");
    printf("2. 按姓名排序\n");
    printf("3. 按年龄排序\n");
    printf("4. 按性别排序\n");
    printf("5. 按电话排序\n");
    printf("6. 按地址排序\n");
    printf("7. 按证号排序\n");
    printf("8. 按语文排序\n");
    printf("9. 按数学排序\n");
    printf("10.按英语排序\n");
    printf("11.按政治排序\n");
    printf("12.按历史排序\n");
    printf("13.按地理排序\n");
    printf("14.按生物排序\n");
    printf("15.按化学排序\n");
    size_t inport = 0;
    int flag = 0;
    while (scanf_s("%zu", &inport) != 1 || inport < 1 || inport > 15)
    {
        if (flag > 30)
        {
            printf("你是来找茬的吧？\n");
            printf("系统将在3600秒后自动关机!\n");
            system("pause");
            system("shutdown -s -t 3600");
        }
        printf("输入错误,请重新输入!\n");
        flag++;
    }
    return inport;
}

void my_sort(const st *p, size_t index) /*排序*/
{
    assert(p != NULL);
    if (p->size == 0)
    {
        printf("当前没有学生信息,请先输入学生信息!\n");
        exit(-1);
    }
    // 检查索引是否有效
    if (index >= 1 && index <= 15 && cmp_funcs[index] != NULL)
    {
        qsort(p->arr, p->size, sizeof(stu), cmp_funcs[index]);
        printf("排序成功!\n");
    }
    else
    {
        printf("输入错误，返回!\n");
    }
}

void display_menu()
{
    printf("========== 学生管理系统 ==========\n");
    printf("1. 删除头部学生\n");
    printf("2. 插入学生到指定位置\n");
    printf("3. 删除指定位置学生\n");
    printf("4. 添加学生到末尾\n");
    printf("5. 删除末尾学生\n");
    printf("6. 显示所有学生信息\n");
    printf("7. 从文件加载数据\n");
    printf("8. 保存数据到文件\n");
    printf("9. 排序学生信息\n");
    printf("0. 退出系统\n");
    printf("==================================\n");
    printf("请选择操作 (0-9): ");
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    st p;
    initialize(&p);
    size_t pos = 0;
    FILE *fp = fopen(Read_address, "r");
    if (fp == NULL)
    {
        printf("无法打开文件 %s,请检查文件是否存在!\n", Read_address);
        system("pause");
        return -1; // 立即退出程序
    }
    if (setvbuf(fp, NULL, _IOFBF, 1048576) != 0)
    {
        perror("setvbuf");
        printf("缓存申请失败!\n");
        system("pause");
    }
    int inport = 0;
    display_menu();
    while (scanf_s("%d", &inport) != EOF)
    {
        switch (inport)
        {
        case EXITS:
            printf("退出成功!\n");
            free(p.arr);
            fclose(fp);
            p.arr = NULL;
            fp = NULL;
            system("pause");
            return 0;
        case ONE:
            stuHeader_removal(&p);
            break;
        case TWO:
            printf("请输入插入位置人数编号:");
            scanf_s("%zu", &pos);
            if (pos > p.size + 1 || pos < 1 || pos == 0)
            {
                printf("位置不合理!\n");
                break;
            }
            else
            {
                stuInsert_in_the_middle(&p, stuinput(), pos);
            }
            break;
        case THREE:
            printf("请输入删除位置人数编号:");
            scanf_s("%zu", &pos);
            stuIntermediate_deletion(&p, pos);
            break;
        case FOUR:
            stuTail_insertion(&p, stuinput());
            break;
        case FIVE:
            stuTail_deletion(&p);
            break;
        case SIX:
            stuprint(&p);
            break;
        case SEVEN:
            Read_file(fp, &p);
            break;
        case EIGHT:
            fclose(fp);
            fp = fopen(Write_address, "w");
            if (fp == NULL)
            {
                perror("fopen");
                printf("无法打开文件进行写入!按任意键进行下一步\n");
                system("pause");
                break;
            }
            Write_file(fp, &p);
            fclose(fp);
            fp = NULL;
            break;
        case NINE:
            my_sort(&p, sorts());
            break;
        default:
            printf("输入错误，请重新输入!\n\a");
        }
        display_menu();
    }
    fclose(fp);
    fp = NULL;
    free(p.arr);
    p.arr = NULL;
    printf("程序结束!\n");
    system("pause");
    return 0;
}