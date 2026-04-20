#include <stdio.h>
#include <float.h>

int main() 
{
    printf("Minimum positive normalized long double: %Le\n", LDBL_MIN);
    printf("Maximum long double: %Le\n", LDBL_MAX);
    printf("Number of decimal digits of precision for long double: %d\n", LDBL_DIG);
    return 0;
}