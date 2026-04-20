#include <stdio.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
/*文件拷贝*/
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    char arr[100];
    char str[100];
    printf("请输入要修改的源文件名,文件必须存在！\n"); 
    //输入文件名
    printf("请输入源文件名：->");
    scanf("%s", str);
    /*读取文件*/
    FILE*pr=fopen(str,"rb");
    if(pr==NULL)
    {
        printf("打开文件失败,或找不到该文件\n");
        printf("%s\n",strerror(errno));
        return 1;  
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    /*2进制修改*/
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pr);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pr);
    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    printf("width=%d,height=%d\n", width, height);
    fseek(pr, 0, SEEK_END);
    /*定位文件指针*/
    long fileSize = ftell(pr);
    printf("file size: %ld bytes\n", fileSize);
    /*输出*/
    printf("请输入目的文件地址->\n");
    scanf("%s", arr);
    /*写入文件*/
    FILE*pw=fopen(arr,"wb");
    if(pw==NULL)
    {
        printf("打开文件失败\n");
        printf("%s\n",strerror(errno));
        return 1;
    }
    char buffer[1048579];  
    /*可以根据实际情况调整缓冲区大小*/
    rewind(pr);  
    /*将文件指针重新定位到文件开头，方便后续读取*/
    while (!feof(pr)) 
    {
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), pr);
        if (bytesRead > 0) 
        {
            fwrite(buffer, 1, bytesRead, pw);
        }
    }
    /*关闭文件*/
    fclose(pw);
    pw=NULL;
    fclose(pr);
    pr=NULL;
    printf("拷贝成功！\n");
    printf("请按任意键退出程序\n");
    system("pause");
    return 0;
}