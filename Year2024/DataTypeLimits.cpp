#include <iostream>
#include <iomanip>
#include <stdio.h>
using namespace std;

int main()
{
    unsigned short us = 65535;
    signed short ss = 65535;
    long l;

    l = us;
    cout << "unsigned long : ";
    cout << setw(8) << hex << l << "," << setw(8) << dec << l << endl;

    l = ss;
    cout << "signed long : ";
    cout << setw(8) << hex << l << "," << setw(8) << dec << l << endl;
    return 0;
}
// int main()
// {
//     printf("Hello, World!\n");
//     return 0;
// }
