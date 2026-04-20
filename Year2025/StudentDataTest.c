#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define NAME_MAX 20
#define SEX_MAX 10
#define ADDRESS_MAX 50

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
// 生成随机姓名
void generate_random_name(char *name) 
{
    const char *first_names[] = {"张", "王", "李", "赵", "刘"};
    const char *last_names[] = {"伟", "芳", "娜", "敏", "静"};
    strcpy(name, first_names[rand() % 5]);
    strcat(name, last_names[rand() % 5]);
}

// 生成随机性别
void generate_random_sex(char *sex) 
{
    const char *sexes[] = {"男", "女"};
    strcpy(sex, sexes[rand() % 2]);
}

// 生成随机地址
void generate_random_address(char *address) 
{
    const char *cities[] = {"北京", "上海", "广州", "深圳", "成都"};
    const char *streets[] = {"街道A", "街道B", "街道C", "街道D", "街道E"};
    int house_num = rand() % 100 + 1;
    sprintf(address, "%s市%s%d号", cities[rand() % 5], streets[rand() % 5], house_num);
}

// 生成随机成绩
void generate_random_grades(subjects *grades) 
{
    grades->Chinese = rand() % 101;
    grades->Math = rand() % 101;
    grades->English = rand() % 101;
    grades->politics = rand() % 71;
    grades->history = rand() % 51;
    grades->geography = rand() % 51;
    grades->biology = rand() % 51;
    grades->chemistry = rand() % 51;
    grades->physics = rand() % 71;
}

// 生成随机电话号码
uint64_t generate_random_tel() 
{
    uint64_t base = 13000000000ULL;
    return base + rand() % 1000000000;
}

// 生成随机身份证号
uint64_t generate_random_identity_card() 
{
    uint64_t base = 110101200001010000ULL;
    return base + rand() % 10000;
}

int main() 
{
    const char *filename = "C:\\Users\\C1373\\Desktop\\students_data.txt";
    srand((unsigned int)time(NULL));
    uint32_t num_students = 10000;
    FILE *file = fopen(filename, "w");
    if (file == NULL) 
    {
        perror("无法打开文件");
        return 1;
    }

    for (uint32_t i = 1; i <= num_students; i++) 
    {
        students student;
        student.num = i;
        generate_random_name(student.name);
        student.age = rand() % 10 + 15;  // 年龄范围 15 - 24
        generate_random_sex(student.sex);
        student.tel = generate_random_tel();
        generate_random_address(student.address);
        student.identity_card = generate_random_identity_card();
        generate_random_grades(&student.grades);

        // 将学生信息写入文件
        fprintf(file, "%u ", student.num);
        fprintf(file, "%s ", student.name);
        fprintf(file, "%u ", student.age);
        fprintf(file, "%s ", student.sex);
        fprintf(file, "%llu ",student.tel);
        fprintf(file, "%s ", student.address);
        fprintf(file, "%llu ",student.identity_card);
        fprintf(file, "%.1f ", student.grades.Chinese);
        fprintf(file, "%.1f ", student.grades.Math);
        fprintf(file, "%.1f ", student.grades.English);
        fprintf(file, "%.1f ", student.grades.politics);
        fprintf(file, "%.1f ", student.grades.history);
        fprintf(file, "%.1f ", student.grades.geography);
        fprintf(file, "%.1f ", student.grades.biology);
        fprintf(file, "%.1f ", student.grades.chemistry);
        fprintf(file, "%.1f ", student.grades.physics);
        fprintf(file, "\n");
    }

    fclose(file);
    printf("成功生成学生信息并写入 %s 文件。\n", filename);

    return 0;
}