#include <iostream>
#include <string>

int main()
{
    const char* tr = "nihao";
    std::string str = "Hello, World!";
    std::string ltr (tr);
    const size_t n = 100;
    std::string ntr(n,'c');
    ntr.push_back('d');
    str.push_back('!');
    std::string str2(str+ntr);
    std::cout << str << std::endl;
    std::cout << ltr << std::endl;
    std::cout << ntr << std::endl;
    std::cout << str + ltr << std::endl;
    std::cout << str.size() << std::endl;
    std::cout << str.capacity() << std::endl;
    std::cout << str2 << std::endl;
    std::cout << str2.size() << std::endl;
    return 0;
}