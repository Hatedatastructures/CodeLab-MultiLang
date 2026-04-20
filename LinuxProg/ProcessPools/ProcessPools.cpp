#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
enum class process_state
{
    idle,        // 空闲
    running,     // 运行中
    completed,   // 已完成
    failed       // 失败
};
struct process
{
    pid_t process_id;                   //进程ID
    time_t start_time;                  //开始时间
    time_t end_time;                    //结束时间
    process_state status;               //状态
    int exit_code;                      //退出码
    std::string task_name;              //任务名
    std::string address;                //管道文件地址    
    int pipeline_descriptor[2];         //管道描述符
    bool read_task;                     //读
    bool write_task;                    //写
    unsigned long long read_byte;       //读字节数
    unsigned long long write_byte;      //读字节数
    std::function<void()> function_objects; //任务
    process()
    :process_id(0),start_time(0),end_time(0),
    status(process_state::idle){}
    process(const process& new_data)  = delete;
    process(const process&& new_data) = delete;
    process& operator() (const process& new_data)  = delete;
    process& operator() (const process&& new_data) = delete;
};
class process_cistern
{
private:
    std::unordered_map<pid_t,size_t> hash_table;
    std::vector<process> process_list;
    const static int process_max = 50;
    const static int default_size = 5;
    int current_size; 
public:
    process_cistern(const int& new_current_size = default_size)
    :current_size(new_current_size){}
};
