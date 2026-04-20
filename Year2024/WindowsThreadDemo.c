#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
/*系统线程*/    
DWORD WINAPI my_xiancheng()
/*子线程1*/
{
    /*线程开始*/
    printf("子线程1线程开始.\n");
    /*读取文件*/
    Sleep(10);
    FILE*pr = fopen("W:\\text.bmp","rb");
    if(pr==NULL)
    {
        printf("打开文件失败\n");
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
    fclose(pr);
    pr=NULL;
    printf("子线程1线程结束.\n");
    /*线程结束*/
    return 0;
}
DWORD WINAPI my_diaomao()
/*子线程2*/
{
    printf("子线程2线程开始.\n");
    Sleep(10);
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
    printf("子线程2线程结束.\n");
    return 0;
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    printf("主线程开始.\n");
    HANDLE haiwei;
    HANDLE haiwei1;
    /*泛型指针*/
    DWORD dwThreadId;
    DWORD dwExitCode;
    /*无符号短整型*/
    haiwei = CreateThread(
            NULL,           /*默认安全性    */
            0,              /*默认堆栈大小  */
            my_xiancheng,   /*线程函数      */
            NULL,           /*线程函数参数  */
            0,              /*默认创建标志  */
            &dwThreadId);   /*线程名字      */
    /*创建成功*/
    haiwei1 = CreateThread(
            NULL,           /*默认安全性    */
            0,              /*默认堆栈大小  */
            my_diaomao,     /*线程函数      */
            NULL,           /*线程函数参数  */
            0,              /*默认创建标志  */
            &dwExitCode);   /*线程名字      */
    printf("%d %d\n",dwThreadId,dwExitCode);
    if(haiwei == NULL && haiwei1 == NULL)
    {
        /*创建失败*/
        printf("创建线程失败\n");
        printf("%s\n",strerror(errno));
        return 1;
    }
    /*等待线程结束*/
    WaitForSingleObject(haiwei,INFINITE);
    WaitForSingleObject(haiwei1,INFINITE);
    /*
    *第一个参数是要等待的对象的句柄
    *第二个参数指定等待的时间间隔传入INFINITE作为第二个参数时表示会一直等待
    */
    // 关闭线程句柄
    CloseHandle(haiwei);
    CloseHandle(haiwei1);
    printf("主线程结束.\n");
    printf("file:%s\n",__FILE__);/*文件路径*/
    printf("line:%d\n",__LINE__);/*当前行号*/
    printf("time:%s\n",__TIME__);/*编译日期*/
    printf("date:%s\n",__DATE__);/*编译时间*/
    system("pause");
    return 0;
}
/*
创建线程后，通常需要等待线程结束，以避免主程序退出后子线程仍在运行。
使用 CloseHandle 来关闭线程句柄，释放资源。
线程函数必须符合特定的签名，并且不能返回值。
确保线程函数中的代码是线程安全的，避免共享数据时出现竞态条件。
*/