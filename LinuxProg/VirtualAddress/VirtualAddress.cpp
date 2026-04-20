#include <iostream>
#include <unistd.h>
int output_counters = 10;
int main()
{
    pid_t create_value = fork();
    if(create_value == 0)
    {
        std::cout << "子进程 "  << std::endl;
        int child = 0;
        while(output_counters)
        {
            std::cout << "我是子进程 : "<< " 子进程ID " << getpid() << " 父进程ID " 
                      << getppid() << " output_counters值 " << output_counters << " 地址 " << &output_counters <<  std::endl;
            if( child == 10 )
            {
                std::cout << "子进程改变了output_counters" << std::endl;
                output_counters = 800;
            }
            sleep(1);
            child++;
        }
    }
    else if(create_value > 0)
    {
        std::cout << "父进程 "  << std::endl;
        while(output_counters)
        {
            std::cout  << "我是父进程 : " << " 子进程ID " << getpid() << " 父进程ID " 
                      << getppid() << " output_counters值 " << output_counters << " 地址 " << &output_counters <<  std::endl;
            sleep(2);
        }
    }
    else
    {
        std::cout << "创建失败 !" << std::endl;
        return 0;
    }
}