#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
struct file
{
    file* fopen(const std::string& file_name,const std::string& mode)
    {
        int flag = 0,fd = 0,default_mode = 0666;
        if(mode == "r")
        {
            flag |= O_RDONLY;
        }
        else if(mode == "w")
        {
            flag |= (O_WRONLY | O_CREAT | O_TRUNC);
        }
        else if(mode == "a")
        {
            flag |= (O_WRONLY | O_CREAT | O_APPEND);
        }
        if(flag & O_RDONLY)
        {
            fd = open(file_name.c_str(),flag);
        }
        else
        {
            fd = open(file_name.c_str(),flag,default_mode);
        }
        if(fd < 0)
        {
            throw  std::string("打开文件失败");
        }
        return 
    }
    int my_flags;
    std::string buffer; 
private:
    
}