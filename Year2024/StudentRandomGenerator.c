#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct stu
{
    char name[20];
    //名字
    char gender[15];
    //性别
    int age;
    //年龄
    long long identity_card;
    //身份证
    long long telephone_number;
    //电话
    char Faculties[50];
    //院系
};

// 生成随机名字，这里简单从一些预设名字里选
void generateName(char *name) 
{
    char names[][20] = {"张三", "李四", "王五", "赵六", "孙七"};
    int index = rand() % 5;
    for (int i = 0; names[index][i]!= '\0'; i++) 
    {
        name[i] = names[index][i];
    }
    name[sizeof(names[index]) - 1] = '\0';
}

// 生成随机性别
void generateGender(char *gender) 
{
    char genders[][15] = {"男", "女"};
    int index = rand() % 2;
    for (int i = 0; genders[index][i]!= '\0'; i++) 
    {
        gender[i] = genders[index][i];
    }
    gender[sizeof(genders[index]) - 1] = '\0';
}

// 生成随机年龄（假设合理范围是18到25岁）
int generateAge() 
{
    return 18 + rand() % 8;
}

// 生成随机身份证号码（这里只是简单生成一个比较长的数字示例，并非真实有效的身份证号生成逻辑）
long long generateIdentityCard() 
{
    long long base = 1000000000000000000LL;
    long long num = rand() % 9000000000000000000LL + 1000000000000000000LL;
    return num;
}

// 生成随机电话号码（这里同样是简单示例，非真实规则）
long long generateTelephoneNumber() 
{
    long long base = 10000000000LL;
    long long num = rand() % 90000000000LL + 10000000000LL;
    return num;
}

// 生成随机院系，简单从一些预设院系里选
void generateFaculties(char *faculties) 
{
    char facs[][50] = {"计算机学院", "机械工程学院", "外国语学院", "经济管理学院", "化学化工学院"};
    int index = rand() % 5;
    for (int i = 0; facs[index][i]!= '\0'; i++) 
    {
        faculties[i] = facs[index][i];
    }
    faculties[sizeof(facs[index]) - 1] = '\0';
}

int main() 
{
    struct stu student;
    srand((unsigned int)time(NULL));

    generateName(student.name);
    generateGender(student.gender);
    student.age = generateAge();
    student.identity_card = generateIdentityCard();
    student.telephone_number = generateTelephoneNumber();
    generateFaculties(student.Faculties);

    printf("姓名：%s\n", student.name);
    printf("性别：%s\n", student.gender);
    printf("年龄：%d\n", student.age);
    printf("身份证号：%lld\n", student.identity_card);
    printf("电话号码：%lld\n", student.telephone_number);
    printf("院系：%s\n", student.Faculties);

    return 0;
}