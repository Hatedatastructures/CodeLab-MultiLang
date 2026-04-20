#include <stdio.h>  
  
int main() {  
    int yuhaiwei = 0; // 初始化变量  
    printf("wan yuan shen \n"); // 输出初始信息  
  
    // 使用while循环，直到yuhaiwei达到10000000  
    while (1) { 
        if (yuhaiwei <= 999){
            // 使用%d来格式化输出yuhaiwei的值  
            printf("牢大: %d\n", yuhaiwei); // 注意这里的": %d"  
            yuhaiwei++; // 增加yuhaiwei的值  
        }else{
            printf("all task is done\n");
            break;
        }
    }  
  
    // 由于while循环的条件，这里不需要再检查yuhaiwei的值  
    // 因为当循环结束时，yuhaiwei一定大于或等于10000000  
    printf("玩原神玩的\n"); // 直接输出这个信息  
  
    return 0;  
}