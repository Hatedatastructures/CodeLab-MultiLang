#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define NUM_MAX 1000
#define ENGLISH_MAX 90
#define ENGLISH_MINI 65
#define CHINASE_MAX 122
#define CHINA_MINI 97
// 生成一个字符串，包含大小写字母和数字，长度为1000，然后查找字符串中第一个字母a的位置

int main()
{
    char strstrs[NUM_MAX+1];
    memset(strstrs, 58, sizeof(strstrs));
    for(uint64_t i = 0; i < NUM_MAX; i++)
    {
        for(uint8_t English = ENGLISH_MINI; English <= ENGLISH_MAX; English++)
        {
            if(i >= NUM_MAX|| strstrs[i] > ENGLISH_MAX)
            {
                strstrs[i] = '\0';
                break;
            }
            strstrs[i] = English;
            ++i;
        }
        if(i >= NUM_MAX)
        {
            strstrs[i] = '\0';
            break;
        }
        for(uint8_t China = CHINA_MINI; China <= CHINASE_MAX; China++)
        {
            if(i >= NUM_MAX || strstrs[i] > CHINASE_MAX)
            {
                strstrs[i] = '\0';
                break;
            }
            strstrs[i] = China;
            ++i;
        }
    }
    printf("%s\n", strstrs);
    char *ptr = strstr(strstrs, "a");
    int nums = ptr ? ptr - strstrs : -1;
    printf("%d\n", nums);
    return 0;
}
