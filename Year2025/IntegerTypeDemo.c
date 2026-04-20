#include <stdio.h>
#include <stdint.h>

int main() 
{
    int8_t a = 127; // 8 位有符号整数
    uint8_t b = 255; // 8 位无符号整数
    int16_t c = 32767; // 16 位有符号整数
    uint16_t d = 65535; // 16 位无符号整数
    int32_t e = 2147483647; // 32 位有符号整数
    uint32_t f = 4294967295U; // 32 位无符号整数
    int64_t g = 9223372036854775807LL; // 64 位有符号整数
    uint64_t h = 18446744073709551615ULL; // 64 位无符号整数
    void* p = &a;
    printf("int8_t: %d\n", a);
    printf("uint8_t: %u\n", b);
    printf("int16_t: %d\n", c);
    printf("uint16_t: %u\n", d);
    printf("int32_t: %d\n", e);
    printf("uint32_t: %u\n", f);
    printf("int64_t: %lld\n", g);
    printf("uint64_t: %llu\n", h);
    printf("void*: %hhu\n", *(int8_t*)p);
    return 0;
}
