#include <stdio.h>
int main()
{
    int a = 123456789;
    size_t len = 0;
    while(1)
    {
        if(a  > 10)
        {
            a /= 10;
            len++;
        }
        else
        {
            break;
        }
    }
    printf("%d\n", len);
    return len;
}