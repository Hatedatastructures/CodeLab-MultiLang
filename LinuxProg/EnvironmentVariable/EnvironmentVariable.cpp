#include <iostream>
#include <unistd.h>
extern char** environ;
int main(int argc,char* argv[],char* env[])
{
    std::cout << "当前目录位置 : " << getenv("PWD") << std::endl;
    for(int ch = 0; ch < argc ; ch++)
    {
        std::cout << argv[ch] << std::endl; 
    }
    std::cout << "输出命令行" << std::endl;
    for(int ch = 0; env[ch] != nullptr ; ch++)
    {
        std::cout << env[ch] << std::endl; 
    }
    std::cout << "命令行输出完毕 !" << std::endl;
    return 0;
}