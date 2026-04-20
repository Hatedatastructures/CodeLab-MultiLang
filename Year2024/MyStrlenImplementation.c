#include <stdio.h>
#include <assert.h>
//断言头文件，断言防止出现野指针
#include <string.h>
//模拟实现字符串函数

size_t my_strlen(const char* str)
//只要访问但是不改变内容的函数 都要加上const 防止误操作
//计算字符串长度
{
    size_t count = 0;
    // 计数器
    assert(str);
    // 断言 如果传入的字符串为空指针 就会报错 防止出现野指针
    while (*str!= '\0')
    //遇到\0 就停止循环
    {
        str++;
        // 指针++ 跳过一个字符
        count++;
        // 累计记字符个数
    }
    return count;
}

char* my_strcpy(char* dest, const char* src)
//源字符串 目标空间
// 拷贝字符串
{
    assert(dest && src);
    //两个只要一个为空指针就会报错
    char* ret = dest;
    //纪录开始的位置
    while (*src!= '\0')
    {
        *dest = *src;
        //dest是改变值不能加上const
        dest++;
        src++;
    }
    
    // while (*dest++ = *src++)
    //当前*src 为'\0'时 循环停止，因为*src++是赋值操作
    // {
    //     ;可以直接简写成这样
    // }
    
    *dest = '\0';
    //最后一个字符'\0' 也拷贝过去 防止出现乱码
    return ret;
    //返回要改空间的地址  
}

char* my_strcat(char* dest, const char* src)
{
    assert(dest && src);
    char* ret = dest;
    while (*dest)
    {
        dest++;
        // 找\0
    }
    while (*src)
    // 找\0
    {
        *dest = *src;
        //在*dest\0的位置开始拷贝，把\0覆盖掉实现字符串的拼接
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
    //返回要改空间的地址
}
int main()
{
    char *p =NULL;
    char str[] = "Hello world how are you";
    char arr1[] = "hello";
    char arr2[] = "world";
    char arr3[30] = "xxxxXXXXXXXX";
    char arr4[20] = "xxxxXXXXXXXX";
    size_t ret = my_strlen(arr1);
    //size_t 是无符号整型
    printf("1:%zu\n", ret);

    my_strcpy(arr1, arr2);
    printf("2:%s\n", arr1);

    my_strcpy(arr1, "hawei");
    //字符串传参的本质就是首字符地址
    printf("3:%s\n", arr1);

    my_strcat(arr3, arr2);
    //要修改的空间  源空间
    //strcat 是字符串追加函数会把arr2追加到arr3后面并且会覆盖掉arr3后面的\0
    printf("4:%s\n", arr3);

    strncpy(arr3, arr2, 5);
    //后面是要拷贝的字符个数，是无符号整型
    //可控拷贝字符串函数
    printf("5:%s\n", arr3);

    strncat(arr3, arr2, 5);
    //可控追加字符串函数
    printf("%s\n", arr3);

    p =strtok(str, " ");
    //分割字符串函数
    //分割字符串的本质是把字符串中的分隔符替换成\0 
    printf("%s\n", p);
    p =strtok(NULL, " ");
    // 分割字符串的本质是把字符串中的分隔符替换成\0
    // 所以要传NULL
    printf("%s\n", p);
    p =strtok(NULL, " ");
    printf("%s\n", p);
    //注意%s期望的是地址

    memcpy(arr4, arr2, 5);
    // 内存拷贝函数,5是5个字节
    printf("%s\n", arr4);

    memmove(arr4, arr1, 5);
    // 内存移动函数
    printf("%s\n", arr4);

    
    return 0;
}