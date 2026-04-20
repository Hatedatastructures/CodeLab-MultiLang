#include <stdio.h>
enum sum
{
    SUM = 1,
    SUB = 8,    
    MUL = 16,
    DIV = 32
};

int main()
{
    int a = 10, b = 5;
    int sum = a + b;
    int sub = a - b;
    int mul = a * b;
    int div = a / b;
    int result = 0;
    enum sum op = SUM;
    switch (op)
    {
    case SUM:
        result = sum;
        break;
    case SUB:
        result = sub;
        break;
    case MUL:
        result = mul;
        break;
    case DIV:
        result = div;
        break;
    default:
        break;
    }
    printf("result = %d\n", result);
    return 0;
}