#include <iostream>

class data
{
    public:
        int x = 9;
        int y = 4;
        data(int a,int b)
        {
            std::cout << "构造函数" << std::endl;
            x=a;
            y=b;
        }
        data()
        {
            std::cout << "构造函数" << std::endl;
            x=1;
            y=2;
        }
        void print()
        {
            printf("%d %d\n",x,y);
        }
        ~data()
        {
            std::cout << "析构函数" << std::endl;
        }
};
int main()
{
    data().print();   //匿名对象生命周期仅限于该行

    data(89,72).print();

    data* tmp = (data*)malloc(sizeof(data));
    tmp->print();
    free(tmp);

    data* temp = new data();
    temp->print();
    delete temp ;

    //两者区别：分配内存时，new会调用构造函数，delete会调用析构函数，而malloc不会调用构造函数，free也不会调用析构函数
    return 0; 
}