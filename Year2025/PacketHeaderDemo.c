#include <stdio.h>
#include <stdint.h>
typedef struct
{
    uint32_t pkg;
    uint64_t type;
}pkg_head;
int main()
{
    pkg_head head;
    head.pkg = 0x12345678;
    head.type = 0x8765432100000000;
    printf("%x\n", head.pkg);
    printf("%llx\n", head.type);
    return 0;
}