#include <iostream>
#include <vector>
#include <algorithm>
//算法库
int main()
{
    std::vector <int> test;
    test.push_back(5);
    test.push_back(4);
    test.push_back(3);
    test.push_back(2);
    test.push_back(1);

    for(std::vector <int> ::iterator i = test.begin();i != test.end();i++)
    {
        std::cout << *i << " ";
    }
    std::cout << std::endl;


    for(std::vector <int> ::reverse_iterator i = test.rbegin();i != test.rend();i++)
    {
        //反向迭代器
        std::cout << *i << " ";
    }
    std::cout << std::endl;
    std::sort(test.begin(),test.end());
    for(std::vector <int> ::iterator i = test.begin();i != test.end();i++)
    {
        std::cout << *i << " ";
    }
    std::cout << std::endl;

    std::sort(test.rbegin(),test.rend());
    for(std::vector <int> ::iterator i = test.begin();i != test.end();i++)
    {
        std::cout << *i << " ";
    }
    std::cout << std::endl;

    std::reverse(test.begin(),test.end());
    for(std::vector <int> ::iterator i = test.begin();i != test.end();i++)
    {
        std::cout << *i << " ";
    }
    std::cout << std::endl;
    return 0;
}