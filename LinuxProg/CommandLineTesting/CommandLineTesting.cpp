#include <iostream>
#include <string>
int main(int argc, char *argv[])
{
    std::string print_value(argv[2]);
    if (argc > 0)
    {
        std::cout << argc << std::endl;
        std::cout << print_value << std::endl;
    }
    return 0;
}