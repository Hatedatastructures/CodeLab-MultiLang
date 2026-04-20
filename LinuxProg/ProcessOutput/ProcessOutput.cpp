#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t ID = fork();
    if(ID == 0)
    {
        int count = 10;
        while(count)
        {
            std::cout << "父进程ID : " << getppid() << " 子进程ID : " << getpid() << " count 值 : " << count-- <<  std::endl;
            sleep(1);
        }
        exit(10);
    }
    sleep(5);
    pid_t child_id = wait(nullptr);
    std::cout << "等待成功 ! 子进程ID为 : " << child_id << std::endl;
    return 0; 
}