#include <iostream> //  引入标准输入输出流库
#include <cstdio> //  引入C标准输入输出库
#include <cstring> //  引入C字符串处理库
#include <cctype> //  引入C字符处理库
#include <cstdlib> //  引入C标准库
#include <cmath> //  引入数学计算库
#include <ctime> //  引入时间处理库
#include <iomanip> //  引入参数化输入输出库
#include <limits> //  引入数值限制库

// 函数结果状态代码
#define TRUE 1 //  定义TRUE为1，表示操作成功
#define FALSE 0 //  定义FALSE为0，表示操作失败
#define OK 1 //  定义OK为1，表示操作成功
#define ERROR 0 //  定义ERROR为0，表示操作错误
#define INFEASIBLE -1 //  定义INFEASIBLE为-1，表示操作不可行
#define SUCCESS 1 //  定义SUCCESS为1，表示操作成功
#define UNSUCCESS 0 //  定义UNSUCCESS为0，表示操作未成功
#define DUPLICATED -1 //  定义DUPLICATED为-1，表示存在重复记录
#define NULL_KEY 0 // 0为无记录标志
#define MAXSIZE 100 // 最大职工数量
#define PI 3.14

// Status是函数返回值的类型，其值是函数结果状态代码，如OK ERROR等 
typedef int Status;

// 定义职工信息结构体
struct Worker {
        char num1[30];  // 职工号
        char name[20];  // 姓名
        char department[20]; // 部门
        char sex[6];    // 性别
        double salary;  // 工资
        char phonenumber[15]; // 电话
};

// 顺序表结构体
struct WorkerList {
        Worker data[MAXSIZE]; //  存储职工信息的数组
        int length; //  当前顺序表的长度，即职工数量
};

// 初始化顺序表
// 函数声明：初始化工人列表
// 参数：list - 引用传递的工人列表对象
void initList(WorkerList& list) {
        // 将工人列表的长度初始化为0，表示列表为空
        list.length = 0;
}

// 输出职工信息
// 函数名：output
// 功能：输出职工信息列表
// 参数：const WorkerList& list - 职工信息列表的引用，不可修改
void output(const WorkerList& list) {
        // 检查列表长度是否为0，如果为0则输出提示信息并返回
        if (list.length <= 0) { //  检查顺序表是否为空
                std::cout << "没有职工信息可显示\n"; //  如果为空，输出提示信息
                return; //  结束函数
        }

        // 输出职工信息列表的标题
        std::cout << "\n职工信息列表：\n";
        // 输出表头，包括职工号、姓名、部门、性别、工资、电话
        std::cout << "职工号\t姓名\t部门\t性别\t工资\t电话\n";
        // 遍历职工信息列表，逐个输出职工信息
        for (int i = 0; i < list.length; ++i) {
                // 输出当前职工的职工号
                std::cout << list.data[i].num1 << "\t"
                                    // 输出当前职工的姓名
                                    << list.data[i].name << "\t"
                                    // 输出当前职工的部门
                                    << list.data[i].department << "\t"
                                    // 输出当前职工的性别
                                    << list.data[i].sex << "\t"
                                    // 输出当前职工的工资，保留两位小数
                                    << std::fixed << std::setprecision(2) << list.data[i].salary << "\t"
                                    // 输出当前职工的电话号码
                                    << list.data[i].phonenumber << "\n";
        }
        // 输出分隔线，用于美化输出格式
        std::cout << "-------------------------\n";
}

// 检查职工号是否重复
// 函数声明：检查工人数组中是否存在重复的工号
// 参数：list - 工人数组，num - 要检查的工号
// 返回值：如果存在重复的工号，返回true；否则返回false
bool isDuplicateNum(const WorkerList& list, const char* num) {
        // 遍历工人数组中的每一个工人
        for (int i = 0; i < list.length; ++i) {
                // 使用strcmp函数比较当前工人的工号和要检查的工号是否相同
                if (std::strcmp(list.data[i].num1, num) == 0) {
                        // 如果相同，说明存在重复的工号，返回true
                        return true;
                }
        }
        // 如果遍历完所有工人都没有找到重复的工号，返回false
        return false;
}

// 职工信息输入
void input(WorkerList& list) {
        // 定义要输入的职工数量
        int numToInput;
        // 提示用户输入职工数量
        std::cout << "请输入要输入的职工数量（1-" << MAXSIZE - list.length << "）：";
        // 循环直到用户输入有效的职工数量
        while (!(std::cin >> numToInput) || numToInput <= 0 || list.length + numToInput > MAXSIZE) {
                // 清除输入流错误标志
                std::cin.clear();
                // 忽略输入流中的错误输入
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//跳过一行输入
                // 如果输入的职工数量超过最大限制，提示用户重新输入
                if (list.length + numToInput > MAXSIZE) {
                        std::cout << "输入的职工数量超过最大限制" << MAXSIZE - list.length 
                     << "，请重新输入（1-" << MAXSIZE - list.length << "）：";
                } else if (numToInput <= 0) {
                        // 如果输入无效，提示用户重新输入一个正整数
                        std::cout << "输入无效，请输入一个正整数（1-" << MAXSIZE - list.length << "）：";
                }
        }

        // 循环输入每个职工的信息
        for (int i = 0; i < numToInput; ++i) {
                // 定义标志变量，用于判断输入是否有效
                bool validInput = false;
                // 循环直到输入有效
                while (!validInput) {
                        // 提示用户输入职工信息
                        std::cout << "输入第 " << i + 1 << " 个职工信息：职工号、姓名、部门、性别、工资、电话。\n";
                        // 从输入流中读取职工信息
                        std::cin >> list.data[list.length].num1 >> list.data[list.length].name >> list.data[list.length].department 
                     >> list.data[list.length].sex >> list.data[list.length].salary >> list.data[list.length].phonenumber;

                        // 检查职工号是否重复
                        if (isDuplicateNum(list, list.data[list.length].num1)) {
                                // 如果职工号重复，提示用户重新输入
                                std::cout << "职工号重复，输入失败，请重新输入该职工信息\n";
                        } else {
                                // 如果输入有效，设置标志变量为true，并增加职工数量
                                validInput = true;
                                ++list.length;
                        }
                }
        }
        // 输出职工列表
        output(list);
}


// 职工信息插入（插入到尾部）
// 定义一个函数用于向职工列表中插入新的职工信息
void insert(WorkerList& list) {
        // 检查当前职工列表是否已满
        if (list.length >= MAXSIZE) {
                // 如果已满，输出提示信息并返回
                std::cout << "职工数量已达上限，无法插入\n";
                return;
        }

        // 初始化一个标志变量，用于判断输入是否有效
        bool validInput = false; //  定义一个布尔变量validInput，用于标记输入是否有效
        // 使用循环确保输入有效
        while (!validInput) {
                // 提示用户输入新的职工信息
                std::cout << "输入要插入的职工信息：职工号、姓名、部门、性别、工资、电话。\n";
                // 从标准输入读取职工信息，并存储到列表的当前末尾位置
                std::cin >> list.data[list.length].num1 >> list.data[list.length].name >> list.data[list.length].department 
                 >> list.data[list.length].sex >> list.data[list.length].salary >> list.data[list.length].phonenumber;

                // 检查输入的职工号是否重复
                if (isDuplicateNum(list, list.data[list.length].num1)) {
                        // 如果重复，输出提示信息并继续循环
                        std::cout << "职工号重复，插入失败，请重新输入\n";
                } else {
                        // 如果不重复，设置输入有效标志为true，并增加列表长度
                        validInput = true;
                        ++list.length;
                }
        }
        // 输出插入成功的提示信息
        std::cout << "职工信息插入成功\n";
        // 调用output函数输出当前职工列表
        output(list);
}

// 按职工号删除
int deleteByNum(WorkerList& list, const char* num) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法删除\n";
                return UNSUCCESS;
        }

        for (int i = 0; i < list.length; ++i) { //  遍历职工列表
                if (std::strcmp(list.data[i].num1, num) == 0) { //  比较当前职工的职工号与要删除的职工号
                        for (int j = i; j < list.length - 1; ++j) { //  找到要删除的职工，将其后的职工向前移动一位
                                list.data[j] = list.data[j + 1];
                        }
                        --list.length; //  职工列表长度减一
                        std::cout << "成功删除职工号为 " << num << " 的职工\n"; //  输出删除成功的消息
                        std::cout << "当前剩余职工数量：" << list.length << "\n";
                        return SUCCESS; //  返回成功标志
                }
        }
        std::cout << "未找到职工号为 " << num << " 的职工\n"; //  输出未找到指定职工号的提示信息
        return UNSUCCESS; //  返回操作未成功的标志
}

// 按姓名删除 
// 函数deleteByName用于从WorkerList中删除指定姓名的职工
// 参数list是传入的WorkerList引用，name是要删除的职工姓名
int deleteByName(WorkerList& list, const char* name) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法删除\n";
                return UNSUCCESS;
        }

        int count = 0; // count用于记录删除的职工数量
        // 遍历WorkerList中的每一个职工
        for (int i = 0; i < list.length; ) {
                // 如果当前职工的姓名与要删除的姓名相同
                if (std::strcmp(list.data[i].name, name) == 0) {
                        // 从当前位置开始，将后面的职工向前移动一位，覆盖当前职工
                        for (int j = i; j < list.length - 1; ++j) {
                                list.data[j] = list.data[j + 1];
                        }
                        ++count; // 删除计数加一
                        --list.length; // 职工总数减一
                } else {
                        // 如果当前职工的姓名不匹配，继续检查下一个职工
                        ++i;
                }
        }
        // 如果有删除的职工，输出成功删除的信息
        if (count > 0) {
                std::cout << "成功删除" << count << "个姓名为 " << name << " 的职工\n";
                std::cout << "当前剩余职工数量：" << list.length << "\n";
        } else {
                // 如果没有找到匹配的职工，输出未找到的信息
                std::cout << "未找到姓名为 " << name << " 的职工\n";
        }
        return count; // 返回删除的职工数量
}

// 修改职工信息
// 函数声明：modify 用于修改WorkerList中指定职工号的信息
void modify(WorkerList& list, const char* num) {
        // 遍历WorkerList中的所有职工信息
        for (int i = 0; i < list.length; ++i) {
                // 比较当前职工的职工号与输入的职工号是否相同
                if (std::strcmp(list.data[i].num1, num) == 0) 
                {
                        // 如果找到匹配的职工号，输出当前职工信息
                        std::cout << "找到职工号为 " << num << " 的职工，当前信息如下：\n";
                        std::cout << "姓名：" << list.data[i].name << "\n"
                                            << "部门：" << list.data[i].department << "\n"
                                            << "性别：" << list.data[i].sex << "\n"
                                            << "工资：" << std::fixed << std::setprecision(2) << list.data[i].salary << "\n"
                                            << "电话：" << list.data[i].phonenumber << "\n";

                        // 提示用户输入新的职工信息（职工号不可修改）
                        std::cout << "请输入新的信息（职工号不可修改）：\n";
                        std::cout << "姓名："; std::cin >> list.data[i].name;
                        std::cout << "部门："; std::cin >> list.data[i].department;
                        std::cout << "性别："; std::cin >> list.data[i].sex;
                        std::cout << "工资："; std::cin >> list.data[i].salary;
                        std::cout << "电话："; std::cin >> list.data[i].phonenumber;

                        // 输出修改成功的提示信息
                        std::cout << "职工信息修改成功！\n";
                        return; // 退出函数
                }
        }
        // 如果遍历完所有职工信息仍未找到匹配的职工号，输出未找到的提示信息
        std::cout << "未找到职工号为 " << num << " 的职工\n";
}

// 按职工号查询
// 函数声明：根据职工号查询职工信息
// 参数：list - 职工列表，num - 要查询的职工号
void queryByNum(const WorkerList& list, const char* num) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法查询\n";
                return;
        }

        // 遍历职工列表
        for (int i = 0; i < list.length; ++i) {
                // 比较当前职工的职工号与要查询的职工号是否相同
                if (std::strcmp(list.data[i].num1, num) == 0) {
                        // 如果找到匹配的职工号，输出职工信息
                        std::cout << "\n找到职工号为 " << num << " 的职工，详细信息如下：\n";
                        std::cout << "---------------------------------\n";
                        std::cout << "职工号：\t" << list.data[i].num1 << "\n"
                                            << "姓名：\t\t" << list.data[i].name << "\n"
                                            << "部门：\t\t" << list.data[i].department << "\n"
                                            << "性别：\t\t" << list.data[i].sex << "\n"
                                            << "工资：\t\t" << std::fixed << std::setprecision(2) << list.data[i].salary << "\n"
                                            << "电话：\t\t" << list.data[i].phonenumber << "\n";
                        std::cout << "---------------------------------\n";
                        return;
                }
        }
        // 如果遍历完整个列表都没有找到匹配的职工号，输出未找到的信息
        std::cout << "未找到职工号为 " << num << " 的职工\n";
}

// 按姓名查询
// 函数声明：queryByName
// 功能：根据姓名在职工列表中查询职工信息
// 参数：
//   list - 职工列表，类型为WorkerList的常量引用
//   name - 要查询的职工姓名，类型为const char*（C风格字符串）
void queryByName(const WorkerList& list, const char* name) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法查询\n";
                return;
        }

        // 布尔变量found用于标记是否找到匹配的职工
        bool found = false;
        // 遍历职工列表
        for (int i = 0; i < list.length; ++i) {
                // 使用strcmp函数比较当前职工的姓名与要查询的姓名是否相同
                if (std::strcmp(list.data[i].name, name) == 0) {
                        // 如果是第一次找到匹配的职工，输出提示信息
                        if (!found) {
                                std::cout << "\n找到姓名为 " << name << " 的职工，信息如下：\n";
                                found = true; // 设置found为true，表示已经找到至少一个匹配的职工
                        }
                        // 输出当前匹配职工的详细信息
                        std::cout << "---------------------------------\n";
                        std::cout << "职工号：\t" << list.data[i].num1 << "\n"
                                            << "姓名：\t\t" << list.data[i].name << "\n"
                                            << "部门：\t\t" << list.data[i].department << "\n"
                                            << "性别：\t\t" << list.data[i].sex << "\n"
                                            << "工资：\t\t" << std::fixed << std::setprecision(2) << list.data[i].salary << "\n"
                                            << "电话：\t\t" << list.data[i].phonenumber << "\n";
                        std::cout << "---------------------------------\n";
                }
        }
        // 如果遍历完整个列表都没有找到匹配的职工，输出未找到的提示信息
        if (!found) 
        {
                std::cout << "未找到姓名为 " << name << " 的职工\n";
        }
}

// 按部门计算平均工资
// 函数avgSalaryByDept用于计算并输出指定部门的平均工资
void avgSalaryByDept(const WorkerList& list) {
        // 定义一个字符数组dept用于存储用户输入的部门名称
        char dept[20];
        // 提示用户输入要查询的部门名称
        std::cout << "请输入要查询的部门：";
        // 从标准输入读取部门名称
        std::cin >> dept;

        // 定义变量total用于累加指定部门所有职工的工资
        double total = 0;
        // 定义变量count用于统计指定部门的职工人数
        int count = 0;
        // 遍历WorkerList中的所有职工记录
        for (int i = 0; i < list.length; ++i) {
                // 检查当前职工的部门是否与用户输入的部门名称相同
                if (std::strcmp(list.data[i].department, dept) == 0) {
                        // 如果相同，累加当前职工的工资到total
                        total += list.data[i].salary;
                        // 职工人数加1
                        ++count;
                }
        }

        // 如果找到指定部门的职工
        if (count > 0) {
                // 输出指定部门的平均工资，保留两位小数
                std::cout << dept << "部门的平均工资为：" << std::fixed << std::setprecision(2) << total / count << "\n";
        } else {
                // 如果未找到指定部门的职工，输出提示信息
                std::cout << "未找到" << dept << "部门的职工\n";
        }
}

// 按工资排序
// 定义一个函数，用于按工资升序排序WorkerList中的工人
void sortBySalary(WorkerList& list) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法排序\n";
                return;
        }

        // 外层循环，控制排序的轮数，每次循环将当前未排序部分的最大值放到末尾
        for (int i = 0; i < list.length - 1; ++i) {
                // 内层循环，控制每轮排序中的比较次数
                for (int j = 0; j < list.length - i - 1; ++j) {
                        // 比较相邻两个工人的工资，如果前一个工人的工资大于后一个工人的工资，则交换他们
                        if (list.data[j].salary > list.data[j + 1].salary) {
                                // 定义一个临时变量temp，用于存储当前工人
                                Worker temp = list.data[j];
                                // 将当前工人替换为下一个工人
                                list.data[j] = list.data[j + 1];
                                // 将下一个工人替换为临时变量temp中的工人
                                list.data[j + 1] = temp;
                        }
                }
        }
        // 输出排序完成的信息
        std::cout << "\n按工资升序排序完成，排序结果如下：\n";
        // 调用output函数，输出排序后的WorkerList
        output(list);
}

// 按职工号排序
// 函数sortByNum用于对WorkerList类型的列表进行排序
void sortByNum(WorkerList& list) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法排序\n";
                return;
        }

        // 外层循环控制排序的轮数，总共需要length-1轮
        for (int i = 0; i < list.length - 1; ++i) {
                // 内层循环控制每一轮的比较次数，每次减少1次比较
                for (int j = 0; j < list.length - i - 1; ++j) {
                        // 使用strcmp比较两个职工号字符串，如果当前职工号大于下一个职工号
                        if (std::strcmp(list.data[j].num1, list.data[j + 1].num1) > 0) {
                                // 定义一个临时变量temp用于交换数据
                                Worker temp = list.data[j];
                                // 将当前数据替换为下一个数据
                                list.data[j] = list.data[j + 1];
                                // 将下一个数据替换为临时变量中的数据
                                list.data[j + 1] = temp;
                        }
                }
        }
        // 输出排序完成的信息
        std::cout << "\n按职工号升序排序完成，排序结果如下：\n";
        // 调用output函数输出排序后的列表
        output(list);
}

// 保存职工信息到文件
// 定义一个函数，用于将WorkerList类型的数据保存到文件中
void saveToFile(const WorkerList& list) {
        // 检查列表是否为空
        if (list.length <= 0) {
                std::cout << "职工列表为空，无法保存\n";
                return;
        }

        // 定义一个字符数组用于存储文件名，最大长度为50个字符
        char filename[50];
        // 提示用户输入要保存的文件名
        std::cout << "请输入要保存的文件名（不超过50个字符）：";
        // 从标准输入读取文件名
        std::cin >> filename;

        // 检查文件名长度
        if (strlen(filename) >= 50) {
                std::cout << "文件名过长，请使用少于50个字符的文件名\n";
                return;
        }

        // 使用fopen函数打开文件，以写入模式("w")打开
        FILE* fp = std::fopen(filename, "w");
        // 检查文件是否成功打开
        if (fp == nullptr) {
                std::cout << "文件打开失败，请检查文件路径和权限\n";
                return;
        }

        // 遍历WorkerList中的所有职工信息
        for (int i = 0; i < list.length; ++i) {
                // 使用fprintf函数将每个职工的信息写入文件
                std::fprintf(fp, "%s %s %s %s %.2f %s\n",
                     list.data[i].num1, list.data[i].name, list.data[i].department,
                     list.data[i].sex, list.data[i].salary, list.data[i].phonenumber);
        }

        // 关闭文件
        std::fclose(fp);
        // 输出成功保存的信息
        std::cout << "\n成功保存" << list.length << "条职工信息到" << filename << "\n";
}

// 从文件读取职工信息
// 函数声明：从文件中加载职工信息到WorkerList结构体中
void loadFromFile(WorkerList& list) {
        // 定义一个字符数组用于存储文件名，最大长度为50
        char filename[50];
        // 提示用户输入要读取的文件名
        std::cout << "请输入要读取的文件名（不超过50个字符）：";
        // 从标准输入读取文件名
        std::cin >> filename;

        // 检查文件名长度
        if (strlen(filename) >= 50) {
                std::cout << "文件名过长，请使用少于50个字符的文件名\n";
                return;
        }

        // 使用fopen函数打开文件，以只读模式("r")
        FILE* fp = std::fopen(filename, "r");
        // 检查文件是否成功打开
        if (fp == nullptr) {
                std::cout << "文件打开失败，请检查文件是否存在以及权限设置\n";
                return;
        }

        // 初始化WorkerList的长度为0
        list.length = 0;
        // 循环读取文件中的职工信息
        while (list.length < MAXSIZE && std::fscanf(fp, "%s %s %s %s %lf %s",
                                               list.data[list.length].num1, list.data[list.length].name, list.data[list.length].department,
                                               list.data[list.length].sex, &list.data[list.length].salary, list.data[list.length].phonenumber) == 6) {
                // 检查职工号是否重复
                if (isDuplicateNum(list, list.data[list.length].num1)) {
                        std::cout << "警告：职工号 " << list.data[list.length].num1 << " 已存在，跳过该记录\n";
                        continue;
                }
                ++list.length;
        }

        // 关闭文件
        std::fclose(fp);
        // 输出读取结果
        if (list.length > 0) {
                std::cout << "\n成功从" << filename << "读取" << list.length << "条职工信息\n";
        } else {
                std::cout << "未从" << filename << "读取到有效职工信息\n";
        }
}

int main() {
        // 输出程序作者信息和当前日期时间
        std::cout << "240812038 吴婷婷 完成本程序时间：";
        system("date /t"); // 调用系统命令显示当前日期
        system("time /t");

        WorkerList list; // 定义一个职工列表
        initList(list); // 初始化职工列表

        int choice = -1; // 用于存储用户选择的操作
        char num[30], name[20]; // 用于存储职工号和姓名
        do {
                // 显示菜单
                std::cout << "\n=================================\n";
                std::cout << "      职工信息管理系统\n";
                std::cout << "=================================\n";
                std::cout << "1. 职工信息输入\n";
                std::cout << "2. 职工信息输出\n";
                std::cout << "3. 职工信息插入\n";
                std::cout << "4. 按职工号删除\n";
                std::cout << "5. 按姓名删除\n";
                std::cout << "6. 职工信息修改\n";
                std::cout << "7. 按职工号查询\n";
                std::cout << "8. 按姓名查询\n";
                std::cout << "9. 按部门计算平均工资\n";
                std::cout << "10. 按工资排序\n";
                std::cout << "11. 按职工号排序\n";
                std::cout << "12. 职工信息保存到文件\n";
                std::cout << "13. 从文件读取职工信息\n";
                std::cout << "0. 退出系统\n";
                std::cout << "=================================\n";
                std::cout << "当前职工总数：" << list.length << "\n";
                std::cout << "请输入你的选择（0-13）：";

                // 输入用户选择
                while (!(std::cin >> choice) || choice < 0 || choice > 13) {
                        std::cin.clear(); // 清除输入错误标志
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略错误输入
                        std::cout << "输入无效，请输入0到13之间的整数：";
                }

                // 根据用户选择执行相应操作
                switch (choice) {
                        case 1:
                                input(list); // 输入职工信息
                                break;
                        case 2:
                                output(list); // 输出职工信息
                                break;
                        case 3:
                                insert(list); // 插入职工信息
                                break;
                        case 4:
                                std::cout << "请输入要删除的职工号：";
                                std::cin >> num;
                                deleteByNum(list, num); // 按职工号删除
                                break;
                        case 5:
                                std::cout << "请输入要删除的职工姓名：";
                                std::cin >> name;
                                deleteByName(list, name); // 按姓名删除
                                break;
                        case 6:
                                std::cout << "请输入要修改的职工号：";
                                std::cin >> num;
                                modify(list, num); // 修改职工信息
                                break;
                        case 7:
                                std::cout << "请输入要查询的职工号：";
                                std::cin >> num;
                                queryByNum(list, num); // 按职工号查询
                                break;
                        case 8:
                                std::cout << "请输入要查询的职工姓名：";
                                std::cin >> name;
                                queryByName(list, name); // 按姓名查询
                                break;
                        case 9:
                                avgSalaryByDept(list); // 按部门计算平均工资
                                break;
                        case 10:
                                sortBySalary(list); // 按工资排序
                                break;
                        case 11:
                                sortByNum(list); // 按职工号排序
                                break;
                        case 12:
                                saveToFile(list); // 保存职工信息到文件
                                break;
                        case 13:
                                loadFromFile(list); // 从文件读取职工信息
                                break;
                        case 0: {
                                std::cout << "\n确定要退出系统吗？(y/n): ";
                                char confirm;
                                std::cin >> confirm;
                                if (confirm == 'y' || confirm == 'Y') 
                                {
                                        std::cout << "\n感谢使用职工信息管理系统，再见！\n";
                                        return 0;
                                }
                                break;
                        }
                        default:
                                std::cout << "\n无效的选择，请重新输入\n";
                }
        } while (choice != 0); // 循环直到用户选择退出

        return 0;
}
