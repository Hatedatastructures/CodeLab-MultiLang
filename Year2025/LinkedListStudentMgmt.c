/*链表*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>

#define NAME_MAX 25
#define SEX_MAX 10
#define ADDRESS_MAX 100
#define TEL_MAX 12
#define ID_MAX 19
#define FILE_READS  "C:\\Users\\C1373\\Desktop\\students_data.txt"
#define FILE_WRITES "C:\\Users\\C1373\\Desktop\\students_data.txt"

typedef unsigned char subjects;
typedef unsigned int  size_int;

typedef struct student {
    subjects Chinese;   // 语文
    subjects Math;      // 数学
    subjects English;   // 英语
    subjects politics;  // 政治
    subjects history;   // 历史
    subjects geography; // 地理
    subjects biology;   // 生物
    subjects chemistry; // 化学
    subjects physics;   // 物理
} s;

typedef struct students {
    size_int num;               // 学号
    char name[NAME_MAX];        // 姓名
    subjects age;               // 年龄
    char sex[SEX_MAX];          // 性别
    char tel[TEL_MAX];          // 电话
    char address[ADDRESS_MAX];  // 地址
    char identity_card[ID_MAX]; // 身份证号
    s grades;                   // 成绩
} st;

typedef struct studentsnode {
    st* data;
    struct studentsnode* next;
} node;

typedef BOOL (*ValidationFunc)(const void*);

// 函数声明
BOOL namejudgment(const void* name);
BOOL agejudgment(const void* age);
BOOL sexjudgment(const void* sex);
BOOL teljudgment(const void* tel);
BOOL IDnumberjudgment(const void* identity_card);
BOOL MainSubjectjudgment(const void* MainSubject);
BOOL Sub_sectionjudgment(const void* Sub_section);
BOOL Sub_sectionsjudgment(const void* Sub_sections);
BOOL stuinputs(const char* Promptwords, const char* format, void* address, ValidationFunc validate);
node* newnode();
void Tail_plugging(node** head);
void Head_insertion(node** head);
void students_print(const node* head);
void file_write(node* head);
void file_reads(node** head);
void release(node** head);
void Tail_deletion(node** head);
void Head_deletion(node** head);
void Intermediate_deletion(node** head, size_int index);
void Insert_in_the_middle(node** head, size_int index);

BOOL namejudgment(const void* name) {
    const char *p = (const char*)name;
    for (size_t i = 0; i < strlen(p); i++) {
        if (!isalpha(p[i])) {
            printf("错误! 姓名只能为字母\n");
            return FALSE;
        }
    }
    return TRUE;
}

BOOL agejudgment(const void* age) {
    int a = *(const int*)age;
    if (a <= 0 || a >= 150) {
        printf("错误! 年龄必须在1-149之间\n");
        return FALSE;
    }
    return TRUE;
}

BOOL sexjudgment(const void* sex) {
    const char *s = (const char*)sex;
    if (strcmp(s, "男") != 0 && strcmp(s, "女") != 0 &&
        strcmp(s, "man") != 0 && strcmp(s, "woman") != 0) {
        printf("错误! 性别只能为男/女或man/woman\n");
        return FALSE;
    }
    return TRUE;
}

BOOL teljudgment(const void* tel) {
    const char *t = (const char*)tel;
    if (strlen(t) != 11) {
        printf("错误! 电话必须为11位数字\n");
        return FALSE;
    }
    for (int i = 0; i < 11; i++) {
        if (!isdigit(t[i])) {
            printf("错误! 电话必须全为数字\n");
            return FALSE;
        }
    }
    return TRUE;
}

BOOL IDnumberjudgment(const void* identity_card) {
    const char *id = (const char*)identity_card;
    if (strlen(id) != 18) {
        printf("错误! 身份证号必须为18位\n");
        return FALSE;
    }
    for (int i = 0; i < 18; i++) {
        if (!isdigit(id[i])) {
            printf("错误! 身份证号必须全为数字\n");
            return FALSE;
        }
    }
    return TRUE;
}

BOOL MainSubjectjudgment(const void* MainSubject) {
    subjects score = *(const subjects*)MainSubject;
    if (score > 150) {
        printf("错误! 主科成绩不能超过150\n");
        return FALSE;
    }
    return TRUE;
}

BOOL Sub_sectionjudgment(const void* Sub_section) {
    subjects score = *(const subjects*)Sub_section;
    if (score > 70) {
        printf("错误! 科目成绩不能超过70\n");
        return FALSE;
    }
    return TRUE;
}

BOOL Sub_sectionsjudgment(const void* Sub_sections) {
    subjects score = *(const subjects*)Sub_sections;
    if (score > 50) {
        printf("错误! 科目成绩不能超过50\n");
        return FALSE;
    }
    return TRUE;
}

BOOL stuinputs(const char* Promptwords, const char* format, void* address, ValidationFunc validate) {
    char buffer[256];
    while (1) {
        printf("%s", Promptwords);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = '\0'; // 去除换行符
            if (sscanf(buffer, format, address) == 1) {
                if (validate != NULL && !validate(address)) {
                    continue;
                }
                return TRUE;
            }
        }
        printf("输入格式错误，请重新输入!\n");
    }
}

node* newnode() {
    node* q = (node*)malloc(sizeof(node));
    if (!q) {
        printf("内存分配失败！\n");
        exit(EXIT_FAILURE);
    }
    q->data = (st*)malloc(sizeof(st));
    if (!q->data) {
        free(q);
        printf("内存分配失败！\n");
        exit(EXIT_FAILURE);
    }
    memset(q->data, 0, sizeof(st));
    return q;
}

void Tail_plugging(node** head) {
    node* new_node = newnode();
    
    // 输入数据
    stuinputs("请输入学号：", "%u", &new_node->data->num, NULL);
    stuinputs("请输入姓名：", "%24s", new_node->data->name, namejudgment);
    stuinputs("请输入年龄：", "%hhu", &new_node->data->age, agejudgment);
    stuinputs("请输入性别：", "%9s", new_node->data->sex, sexjudgment);
    stuinputs("请输入电话：", "%11s", new_node->data->tel, teljudgment);
    stuinputs("请输入地址：", "%99s", new_node->data->address, NULL);
    stuinputs("请输入身份证号：", "%18s", new_node->data->identity_card, IDnumberjudgment);
    stuinputs("请输入语文成绩：", "%hhu", &new_node->data->grades.Chinese, MainSubjectjudgment);
    stuinputs("请输入数学成绩：", "%hhu", &new_node->data->grades.Math, MainSubjectjudgment);
    stuinputs("请输入英语成绩：", "%hhu", &new_node->data->grades.English, MainSubjectjudgment);
    stuinputs("请输入政治成绩：", "%hhu", &new_node->data->grades.politics, Sub_sectionjudgment);
    stuinputs("请输入历史成绩：", "%hhu", &new_node->data->grades.history, Sub_sectionsjudgment);
    stuinputs("请输入地理成绩：", "%hhu", &new_node->data->grades.geography, Sub_sectionsjudgment);
    stuinputs("请输入生物成绩：", "%hhu", &new_node->data->grades.biology, Sub_sectionsjudgment);
    stuinputs("请输入化学成绩：", "%hhu", &new_node->data->grades.chemistry, Sub_sectionsjudgment);
    stuinputs("请输入物理成绩：", "%hhu", &new_node->data->grades.physics, Sub_sectionjudgment);

    if (*head == NULL) {
        *head = new_node;
    } else {
        node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    printf("尾部插入成功！\n");
}

void Head_insertion(node** head) {
    node* new_node = newnode();
    
    // 输入数据（同Tail_plugging）
    // ... 此处应添加与Tail_plugging相同的输入代码，建议提取为单独函数 ...

    new_node->next = *head;
    *head = new_node;
    printf("头部插入成功！\n");
}

void students_print(const node* head) {
    if (!head) {
        printf("链表为空！\n");
        return;
    }
    size_int count = 0;
    while (head) {
        printf("学号：%u 姓名：%s 年龄：%hhu 性别：%s 电话：%s\n地址：%s 身份证：%s\n"
               "成绩：语文%hhu 数学%hhu 英语%hhu 政治%hhu 历史%hhu 地理%hhu 生物%hhu 化学%hhu 物理%hhu\n",
               head->data->num, head->data->name, head->data->age, head->data->sex, head->data->tel,
               head->data->address, head->data->identity_card,
               head->data->grades.Chinese, head->data->grades.Math, head->data->grades.English,
               head->data->grades.politics, head->data->grades.history, head->data->grades.geography,
               head->data->grades.biology, head->data->grades.chemistry, head->data->grades.physics);
        head = head->next;
        count++;
    }
    printf("共打印%u条记录\n", count);
}

void file_write(node* head) {
    FILE* fp = fopen(FILE_WRITES, "w");
    if (!fp) {
        printf("无法打开写入文件！\n");
        return;
    }
    size_int count = 0;
    while (head) {
        fprintf(fp, "%u %s %hhu %s %s %s %s %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu\n",
                head->data->num, head->data->name, head->data->age, head->data->sex,
                head->data->tel, head->data->address, head->data->identity_card,
                head->data->grades.Chinese, head->data->grades.Math, head->data->grades.English,
                head->data->grades.politics, head->data->grades.history, head->data->grades.geography,
                head->data->grades.biology, head->data->grades.chemistry, head->data->grades.physics);
        head = head->next;
        count++;
    }
    fclose(fp);
    printf("成功写入%u条记录\n", count);
}

void file_reads(node** head) {
    FILE* fp = fopen(FILE_READS, "r");
    if (!fp) {
        printf("无法打开读取文件！\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        st student;
        memset(&student, 0, sizeof(st));
        int parsed = sscanf(line, "%u %24s %hhu %9s %11s %99s %18s %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
                           &student.num, student.name, &student.age, student.sex, student.tel,
                           student.address, student.identity_card, &student.grades.Chinese,
                           &student.grades.Math, &student.grades.English, &student.grades.politics,
                           &student.grades.history, &student.grades.geography, &student.grades.biology,
                           &student.grades.chemistry, &student.grades.physics);
        if (parsed != 16) {
            printf("解析错误，跳过该行\n");
            continue;
        }
        
        node* new_node = newnode();
        *new_node->data = student;
        
        if (*head == NULL) {
            *head = new_node;
        } else {
            node* current = *head;
            while (current->next) current = current->next;
            current->next = new_node;
        }
    }
    fclose(fp);
    printf("文件读取完成\n");
}

void release(node** head) {
    while (*head) {
        node* temp = *head;
        *head = (*head)->next;
        free(temp->data);
        free(temp);
    }
    printf("链表已释放\n");
}

void Tail_deletion(node** head) {
    if (!*head) {
        printf("链表为空！\n");
        return;
    }
    if (!(*head)->next) {
        free((*head)->data);
        free(*head);
        *head = NULL;
    } else {
        node* prev = NULL;
        node* current = *head;
        while (current->next) {
            prev = current;
            current = current->next;
        }
        prev->next = NULL;
        free(current->data);
        free(current);
    }
    printf("尾部删除成功\n");
}

void Head_deletion(node** head) {
    if (!*head) {
        printf("链表为空！\n");
        return;
    }
    node* temp = *head;
    *head = (*head)->next;
    free(temp->data);
    free(temp);
    printf("头部删除成功\n");
}

void Intermediate_deletion(node** head, size_int index) {
    if (index == 0) {
        Head_deletion(head);
        return;
    }
    node* prev = NULL;
    node* current = *head;
    for (size_int i = 0; current && i < index; i++) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        printf("索引超出范围\n");
        return;
    }
    prev->next = current->next;
    free(current->data);
    free(current);
    printf("位置%u删除成功\n", index);
}

void Insert_in_the_middle(node** head, size_int index) {
    if (index == 0) {
        Head_insertion(head);
        return;
    }
    node* new_node = newnode();
    // 输入数据（需补充）
    
    node* current = *head;
    for (size_int i = 0; current && i < index-1; i++) {
        current = current->next;
    }
    if (!current) {
        printf("插入位置无效\n");
        free(new_node->data);
        free(new_node);
        return;
    }
    new_node->next = current->next;
    current->next = new_node;
    printf("位置%u插入成功\n", index);
}

void text1() {
    node* head = NULL;
    Tail_plugging(&head);
    students_print(head);
    file_write(head);
    release(&head);
}

int main() {
    text1();
    return 0;
}