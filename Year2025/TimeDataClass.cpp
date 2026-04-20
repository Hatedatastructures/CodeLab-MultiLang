#include <iostream>
#include <windows.h>
#include <cstdint>

class time_data
{
    private:
    //私有类
    class _Time_test_
    {
        public:
            typedef struct _Time_test_variable
            {
                int64_t _year;    
                int64_t _month;
                int64_t _day;
                int64_t _hour;
                int64_t _minute;
                int64_t _second;
            }_Time_test_variable;
            /*年 月 天 时 分 秒*/
            _Time_test_variable _variable_data_; //要想在外面用结构体必须要创建一个变量来用
    }; //时间测试类

    _Time_test_ _variable_s_;//和上面同理

    int64_t monthRules(_Time_test_& _year_month_variable_ )
    {
        //判断闰年以及月份天数检查
        static int64_t _month_rules_[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if((_year_month_variable_._variable_data_._year % 4 == 0 && _year_month_variable_._variable_data_._year % 100 != 0) 
        || _year_month_variable_._variable_data_._year % 400 == 0)
        {
            return (_month_rules_[_year_month_variable_._variable_data_._month]) + 1;
        }
        return _month_rules_[_year_month_variable_._variable_data_._month];
    }
    BOOL timeAdjustmentDetails(_Time_test_& _Temporary_Data_)
    {
        //时间调整细节函数
        if( _Temporary_Data_._variable_data_._second >= 60 ||
            _Temporary_Data_._variable_data_._minute >= 60 ||
            _Temporary_Data_._variable_data_._hour >= 24   ||
            _Temporary_Data_._variable_data_._day >= monthRules(_Temporary_Data_) ||
            _Temporary_Data_._variable_data_._month >= 12 )
            {
                return false;
            }
        return true;
    }
    BOOL subtractiveJudgment(_Time_test_& _Temporary_Data_)
    {
        //减法判断
        if( _Temporary_Data_._variable_data_._second < 0 ||
            _Temporary_Data_._variable_data_._minute < 0 ||
            _Temporary_Data_._variable_data_._hour < 0   ||
            _Temporary_Data_._variable_data_._day < 1    ||
            _Temporary_Data_._variable_data_._month < 1  ||
            _Temporary_Data_._variable_data_._year < 0)
        {
            return false;
        }
        return true;
    }
    public:

    time_data(int64_t year_ = 0, int64_t month_ = 1, int64_t day_ = 1, int64_t hour_ = 1, int64_t minute_ = 1, int64_t second_ = 1)
    {
        //全缺省构造函数
        _variable_s_._variable_data_._year = year_;
        _variable_s_._variable_data_._month = month_;
        _variable_s_._variable_data_._day = day_;
        _variable_s_._variable_data_._hour = hour_;
        _variable_s_._variable_data_._minute = minute_;
        _variable_s_._variable_data_._second = second_;
        std::cout << "全缺省构造函数" << std::endl;
    }

    time_data(const time_data& _Temporary_Data_)
    {
        //拷贝构造函数
        _variable_s_._variable_data_._year    =  _Temporary_Data_._variable_s_._variable_data_._year;
        _variable_s_._variable_data_._month   =  _Temporary_Data_._variable_s_._variable_data_._month;
        _variable_s_._variable_data_._day     =  _Temporary_Data_._variable_s_._variable_data_._day;
        _variable_s_._variable_data_._hour    =  _Temporary_Data_._variable_s_._variable_data_._hour;
        _variable_s_._variable_data_._minute  =  _Temporary_Data_._variable_s_._variable_data_._minute;
        _variable_s_._variable_data_._second  =  _Temporary_Data_._variable_s_._variable_data_._second;
    }
    ~time_data()
    {
        std::cout << "析构函数" << std::endl;
        _variable_s_._variable_data_._year    = 0;
        _variable_s_._variable_data_._month   = 1;
        _variable_s_._variable_data_._day     = 1;
        _variable_s_._variable_data_._hour    = 1;
        _variable_s_._variable_data_._minute  = 1;
        _variable_s_._variable_data_._second  = 1;
        //析构函数是专门在对象销毁时调用的函数，演示一下，主要是释放开辟内存，但这里没有开辟内存，所以不用写
    }
    void timeDataPrintf(const time_data& _Temporary_Data_)
    {
        std::cout << "timeDataPrintf" << std::endl;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._year    << " " ;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._month   << " " ;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._day     << " " ;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._hour    << " " ;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._minute  << " " ;
        std::cout << _Temporary_Data_._variable_s_._variable_data_._second  << std::endl;
    }
    
    void timeAdjustment(_Time_test_& _Temporary_Data_)
    {
        //判断时间是否合法
        while(!timeAdjustmentDetails(_Temporary_Data_))
        {
            if(_Temporary_Data_._variable_data_._second >= 60)
            {
                _Temporary_Data_._variable_data_._second -= 60;
                _Temporary_Data_._variable_data_._minute += 1;
            }
            if(_Temporary_Data_._variable_data_._minute > 60)
            {
                _Temporary_Data_._variable_data_._minute -= 60;
                _Temporary_Data_._variable_data_._hour   += 1;
            }
            if(_Temporary_Data_._variable_data_._hour > 24)
            {
                _Temporary_Data_._variable_data_._hour -= 24;
                _Temporary_Data_._variable_data_._day   += 1;
            }
            if(_Temporary_Data_._variable_data_._day >= monthRules(_Temporary_Data_) )
            {
                _Temporary_Data_._variable_data_._day -= monthRules(_Temporary_Data_);
                _Temporary_Data_._variable_data_._month += 1;
            }
            if(_Temporary_Data_._variable_data_._month >= 12)
            {
                _Temporary_Data_._variable_data_._month -= 12;
                _Temporary_Data_._variable_data_._year  += 1;
            }
        }
        std::cout << "Time_adjustment_" << std::endl;
    }
    time_data operator+ (const time_data& rhs) const 
    {
        time_data result(*this);
        result._variable_s_._variable_data_._second += rhs._variable_s_._variable_data_._second;
        result._variable_s_._variable_data_._minute += rhs._variable_s_._variable_data_._minute;
        result._variable_s_._variable_data_._hour   += rhs._variable_s_._variable_data_._hour;
        result._variable_s_._variable_data_._day    += rhs._variable_s_._variable_data_._day;
        result._variable_s_._variable_data_._month  += rhs._variable_s_._variable_data_._month;
        result._variable_s_._variable_data_._year   += rhs._variable_s_._variable_data_._year;
        result.timeAdjustment(result._variable_s_); // 处理进位
        return result;
    }    
    time_data operator- (time_data& _Temporary_Data_)
    //待解决
    {
        std::cout << "operator-" << std::endl;
        _Time_test_ M_variable_s_ ;
        M_variable_s_._variable_data_._second -= _Temporary_Data_._variable_s_._variable_data_._second;
        M_variable_s_._variable_data_._minute -= _Temporary_Data_._variable_s_._variable_data_._minute;
        M_variable_s_._variable_data_._hour   -= _Temporary_Data_._variable_s_._variable_data_._hour;
        M_variable_s_._variable_data_._day    -= _Temporary_Data_._variable_s_._variable_data_._day;
        M_variable_s_._variable_data_._month  -= _Temporary_Data_._variable_s_._variable_data_._month;
        M_variable_s_._variable_data_._year   -= _Temporary_Data_._variable_s_._variable_data_._year;
        if(subtractiveJudgment(M_variable_s_))
        {
            std::cout << "数据异常" << std::endl;
            return *this;
        }
        else
        {
            _variable_s_._variable_data_._second -= _Temporary_Data_._variable_s_._variable_data_._second;
            _variable_s_._variable_data_._minute -= _Temporary_Data_._variable_s_._variable_data_._minute;
            _variable_s_._variable_data_._hour   -= _Temporary_Data_._variable_s_._variable_data_._hour;
            _variable_s_._variable_data_._day    -= _Temporary_Data_._variable_s_._variable_data_._day;
            _variable_s_._variable_data_._month  -= _Temporary_Data_._variable_s_._variable_data_._month;
            _variable_s_._variable_data_._year   -= _Temporary_Data_._variable_s_._variable_data_._year;
        }
        timeDataPrintf(_Temporary_Data_);
        return *this;
    }
    BOOL operator== (time_data& _Temporary_Data_)
    {
        return (_variable_s_._variable_data_._second == _Temporary_Data_._variable_s_._variable_data_._second) &&
               (_variable_s_._variable_data_._minute == _Temporary_Data_._variable_s_._variable_data_._minute) &&
               (_variable_s_._variable_data_._hour   == _Temporary_Data_._variable_s_._variable_data_._hour) &&
               (_variable_s_._variable_data_._day    == _Temporary_Data_._variable_s_._variable_data_._day) &&
               (_variable_s_._variable_data_._month  == _Temporary_Data_._variable_s_._variable_data_._month) &&
               (_variable_s_._variable_data_._year   == _Temporary_Data_._variable_s_._variable_data_._year);
    }
};//一个时间类

int main()
{
    time_data t2(2022, 3, 20, 12, 30, 30);
    t2.timeDataPrintf(t2);
    time_data t3(t2);
    t3.timeDataPrintf(t3);
    time_data t4 = t2 + t3;
    t4.timeDataPrintf(t4);
    time_data t5 = t2 - t3;
    t5.timeDataPrintf(t5);
    time_data t6 = t2;
    if(t2 == t6)
    {
        t2.timeDataPrintf(t2);
        t6.timeDataPrintf(t6);
        std::cout << "相等" << std::endl;
    }
    else
    {
        t2.timeDataPrintf(t2);
        t6.timeDataPrintf(t6);
        std::cout << "不相等" << std::endl;
    }
    t5.timeDataPrintf(t5);
    // time t3(t2);
    return 0;
}