#include <iostream>
#include <time.h>
using namespace std;
using std::cout;

//单独的命名空间
namespace text
{
    int value = 0;
    void add(int left, int right)
    {
        value = left + right;
        cout << "value = " << value << endl;
    }
    namespace text2
    {
        void add(int left, int right)
        {
            cout << "value = " << left + right << endl;
        }
        namespace text3
        {
            void add(int left, int right)
            {
                cout << "value = " << left + right << endl;
            }
            namespace text4
            {
                inline void add(int left, int right)//内联函数,相当于宏定义
                {
                    cout << "value = " << left + right << endl;
                }
            }
        }
    }
}
int main()
{
    int a;
    double b;

    std::cout <<"请输入一个整数：->";
    cin >> a;

    std::cout << "请输入一个浮点数：->";
    cin >> b;

    cout << "你输入的整数是：" << a << endl;
    cout << "hello world" << endl;
    
    std::cout << "hello world" << std::endl;

    text::add(1, 2);
    text::text2::text3::text4::add(3, 4);
    cout << "value = " << text::value << endl;
    return 0;
}