#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int id = fork();
    if(id == 0)
    {
        printf("子进程开始执行: 当前id为 %d\n",getpid());
    }
    else if( id > 0)
    {
        printf("父进程开始执行: 当前id为 %d\n",getpid());
    }
    else
    {
        printf("创建子进程失败\n");
    }
    return 0;
}