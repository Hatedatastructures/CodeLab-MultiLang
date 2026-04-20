#include <iostream>

class Time_w
{
private:
    int hour;
    int minute;
    int second;
public:
    friend std::ostream& operator<<(std::ostream& os, const Time_w& t);
    Time_w(int h, int m, int s)
    {
        hour = h;
        minute = m;
        second = s;
    }
    Time_w()
    {
        hour = 0;
        minute = 0;
        second = 0;     
    }
};
std::ostream& operator<<(std::ostream& os, const Time_w& t)
{
    os << t.hour << ":" << t.minute << ":" << t.second;
    return os;
}
int main()
{
    Time_w t1(1, 2, 3);
    Time_w t2;
    std::cout << t1 <<  std::endl;
    return 0;
}