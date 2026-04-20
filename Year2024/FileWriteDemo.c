#include <stdio.h>
#include <string.h>
#include <errno.h>
int main()
{
    FILE* fp = fopen("C:\\Users\\C1373\\Desktop\\text.txt", "w");
    if (fp == NULL)
    {
        printf("open file failed\n");
        printf("%s", strerror(errno));
        return 1;
    }
    fprintf(fp, "hello,word!\n");
    //关闭文件
    fclose(fp);
    fp=NULL;
    return 0;
}