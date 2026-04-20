#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//信息生成
#define NAME_MAX 50
#define SEX_MAX 10
#define ADDRESS_MAX 100
#define NUM_STUDENTS 10000

typedef struct student
{
    unsigned char Chinese;          /*语文*/
    unsigned char Math;             /*数学*/
    unsigned char English;          /*英语*/
    unsigned char politics;         /*政治*/
    unsigned char history;          /*历史*/
    unsigned char geography;        /*地理*/
    unsigned char biology;          /*生物*/
    unsigned char chemistry;        /*化学*/
} s;

typedef struct students
{
    int num;                        /*学号*/
    char name[NAME_MAX];            /*姓名*/
    int age;                        /*年龄*/
    char sex[SEX_MAX];              /*性别*/
    long long tel;                  /*电话*/
    char address[ADDRESS_MAX];      /*地址*/
    long long identity_card;        /*证号*/
    s wang;                              /*成绩*/
} stu;

// 生成一个随机的分数，范围在 0 到 100 之间
unsigned char random_score() 
{
    return (unsigned char)(rand() % 101);
}

// 生成一个随机的电话号码
long long random_tel() 
{
    long long tel = 0;
    for (int i = 0; i < 11; i++) 
    {
        tel = tel * 10 + (rand() % 10);
    }
    return tel;
}

// 生成一个随机的身份证号
long long random_identity_card() 
{
    long long id = 0;
    for (int i = 0; i < 18; i++) 
    {
        id = id * 10 + (rand() % 10);
    }
    return id;
}


int main() 
{
    FILE *fp;
    fp = fopen("C:\\Users\\C1373\\Desktop\\students_data.txt", "w");
    if (fp == NULL) 
    {
        perror("Error opening file");
        return 1;
    }
    srand(time(NULL));  // 初始化随机数种子
    size_t amg = 0;
    for (int i = 0; i < NUM_STUDENTS; i++) 
    {
        stu student;
        student.num = i + 1;
        // 生成随机姓名，这里简单使用 "StudentX" 的形式
        sprintf(student.name, "Student%d", i + 1);
        student.age = rand() % 30 + 18;  // 年龄在 18 到 47 岁之间
        // 性别随机为 "Male" 或 "Female"
        if (rand() % 2 == 0) 
        {
            sprintf(student.sex, "Male");
        } 
        else 
        {
            sprintf(student.sex, "Female");
        }
        student.tel = random_tel();
        // 生成随机地址，这里简单使用 "AddressX" 的形式
        sprintf(student.address, "Address%d", i + 1);
        student.identity_card = random_identity_card();
        student.wang.Chinese = random_score();
        student.wang.Math = random_score();
        student.wang.English = random_score();
        student.wang.politics = random_score();
        student.wang.history = random_score();
        student.wang.geography = random_score();
        student.wang.biology = random_score();
        student.wang.chemistry = random_score();

        // 将学生信息写入文件
        fprintf(fp, "%d %s %d %s %lld %s %lld %d %d %d %d %d %d %d %d\n",
                student.num, student.name, student.age, student.sex, student.tel,
                student.address, student.identity_card,
                student.wang.Chinese, student.wang.Math, student.wang.English,
                student.wang.politics, student.wang.history, student.wang.geography,
                student.wang.biology , student.wang.chemistry);
        amg++;
    }
    fclose(fp);
    fp =NULL;
    printf("写入成功！写入%zu条数据",amg);
    return 0;
}
