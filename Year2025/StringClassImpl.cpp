#include <iostream>
#include <cstring>
//模拟string
namespace wang
{
    /*
    * 迭代器失效规则：
    * - 所有改变容量（_capacity）的操作（如 push_back、insert、resize、operator+= 等）会导致所有迭代器失效。
    * - 插入/删除元素且容量不变时，插入/删除点之后的正向迭代器失效，之前的迭代器保持有效。
    * - 反向迭代器的失效规则与正向迭代器对称（插入/删除点之前的反向迭代器失效）。
    * - 使用reserve函数时会返回一个新的首地址迭代器，根据使用情况自己改变迭代器位置。
    */
    class string
    {
        private:
            char *_data;
            size_t _size;
            size_t _capacity;
        public:
            //创建迭代器
            typedef char* iterator;
            typedef const char* const_iterator;
            typedef char* reverse_iterator;
            typedef const char* const_reverse_iterator;
            //反向迭代器
            //限定字符串最大值
            static const size_t nops = -1;
            iterator begin() 
            {  
                return _data; 
            }
            iterator end()
            {  
                return _data + _size; 
            }
            const_iterator cbegin()const
            { 
                return const_iterator(_data);
            }
            const_iterator cend()const
            { 
                return const_iterator(_data + _size); 
            }
            reverse_iterator rbegin()
            {
                return reverse_iterator(end()- 1);
            }
            reverse_iterator rend()
            {
                return reverse_iterator(begin()- 1);
            }
            const_reverse_iterator crbegin()const
            {
                return const_reverse_iterator(cend()- 1);
            }
            const_reverse_iterator crend()const
            {
                return const_reverse_iterator(cbegin()- 1);
            }

            size_t size()const
            {
                //返回有效字符串长度
                return _size;
            }
            size_t capacity()const
            {
                //返回容量
                return _capacity;
            }
            char* c_str()const
            {
                //返回C风格字符串
                return _data;
            }
            string(const char* data_str = "")
            :_size(data_str == nullptr ? 0 : strlen(data_str)),_capacity(_size)
            {
                //传进来的字符串是常量字符串，不能直接修改，需要拷贝一份，并且常量字符串在数据段(常量区)浅拷贝会导致程序崩溃
                if(data_str != nullptr)
                {
                    _data = new char[_capacity + 1];
                    strcpy(_data,data_str);
                    _data[_size] = '\0';
                }
                else
                {
                    _data = new char[1];
                    _data[0] = '\0';
                }
            }
            string(const string &data_str)
            :_data(nullptr),_size(data_str._size),_capacity(data_str._capacity)
            {
                //拷贝构造函数，拿传入对象的变量初始化本地变量，对于涉及开辟内存的都要深拷贝
                size_t capacity = data_str._capacity;
                _data = new char[capacity + 1];
                strcpy(_data,data_str._data);
                _data[_size] = '\0';
            }
            ~string()
            {
                delete [] _data;
                _capacity = _size = 0;
            }
            string& conversions_oldest()
            {
                //字符串转大写
               for(string::iterator originate = _data; originate != _data + _size; originate++)
                {
                    if(*originate >= 'a' && *originate <= 'z')
                    {
                        *originate -= 32;
                    }
                }
                return *this;
            }
            string& conversions_few()
            {
                //字符串转小写
                for(string::iterator originate = _data; originate != _data + _size; originate++)
                {
                    if(*originate >= 'A' && *originate <= 'Z')
                    {
                        *originate += 32;
                    }
                }
                return *this;
            }
            // size_t str_substring_kmp(const char*& c_str_substring)
            // {
            //     //查找子串
            // }
            string& nose_Insertion_substrings(const char*& c_str_substring)
            {
                //前部插入子串
                size_t len = strlen(c_str_substring);
                size_t new_nose_insert_substrings = _size + len;
                if(Automatic_scaling(new_nose_insert_substrings) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return *this;
                }
                char* _c_nose_insert_substrings_temp = new char[_capacity + 1];
                //临时变量
                memmove(_c_nose_insert_substrings_temp , _data , _size + 1);
                memmove(_data , c_str_substring , len);
                memmove(_data + len , _c_nose_insert_substrings_temp , _size + 1);
                //比memcpy更安全，memcpy会覆盖原有数据，memmove会先拷贝到临时变量再拷贝到目标地址
                _size = new_nose_insert_substrings;
                _data[_size] = '\0';
                delete [] _c_nose_insert_substrings_temp;
                return *this;
            }
            string& interlocutory_Insertion_substrings(const char*& c_str_substring,const size_t& oid_pos)
            {
                //中间位置插入子串
                if(oid_pos > _size)
                {
                    std::cout << "插入位置越界！" << std::endl;
                    return *this;
                }
                size_t len = strlen(c_str_substring);
                size_t new_interlocutory_insert_substrings = _size + len;
                if(Automatic_scaling(new_interlocutory_insert_substrings) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return *this;
                }
                char* _c_interlocutory_insert_substrings_temp = new char[new_interlocutory_insert_substrings + 1];
                //临时变量
                strcpy(_c_interlocutory_insert_substrings_temp,_data);
                //从oid_pos开始插入
                strcpy(_data + oid_pos, c_str_substring);
                strcpy(_data + oid_pos + len , _c_interlocutory_insert_substrings_temp + oid_pos);
                _size = new_interlocutory_insert_substrings;
                _data[_size] = '\0';
                delete [] _c_interlocutory_insert_substrings_temp;
                return *this;
            }
            string str_withdraw(const size_t& old_pos)
            {
                //提取字串到'\0'
                if(old_pos > _size)
                {
                    std::cout << "提取位置越界！" << std::endl;
                    return string();
                }
                string _str_withdraw_temp;
                size_t _str_withdraw_temp_len = _size - old_pos;
                if(_str_withdraw_temp.Automatic_scaling(_str_withdraw_temp_len) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return string();
                }
                strcpy(_str_withdraw_temp._data , _data + old_pos);
                _str_withdraw_temp._size = _str_withdraw_temp_len;
                _str_withdraw_temp._data[_str_withdraw_temp._size] = '\0';
                return _str_withdraw_temp;
            }
            string str_withdraw_extremity(const size_t& old_begin)
            {
                //提取字串到末尾
                if(old_begin > _size)
                {
                    std::cout << "提取位置越界！" << std::endl;
                    return string();
                }
                string _str_withdraw_extremity_temp;
                size_t _str_withdraw_extremity_temp_len = _size - old_begin;
                if(_str_withdraw_extremity_temp.Automatic_scaling(_str_withdraw_extremity_temp_len) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return string();
                }
                strcpy(_str_withdraw_extremity_temp._data , _data + old_begin);
                _str_withdraw_extremity_temp._size = _str_withdraw_extremity_temp_len;
                _str_withdraw_extremity_temp._data[_str_withdraw_extremity_temp._size] = '\0';
                return _str_withdraw_extremity_temp;
            }
            string str_withdraw_detail(const size_t& old_begin ,const size_t& old_end)
            {
                //提取字串到指定位置
                if(old_begin > _size || old_end > _size || old_begin > old_end)
                {
                    std::cout << "提取位置越界！" << std::endl;
                    return string();
                }
                string _str_withdraw_detail_temp;
                size_t _str_withdraw_detail_temp_len = old_end - old_begin;
                if(_str_withdraw_detail_temp.Automatic_scaling(_str_withdraw_detail_temp_len) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return string();
                }
                strcpy(_str_withdraw_detail_temp._data , _data + old_begin);
                _str_withdraw_detail_temp._size = _str_withdraw_detail_temp_len;
                _str_withdraw_detail_temp._data[_str_withdraw_detail_temp._size] = '\0';
                return _str_withdraw_detail_temp;
            }
            bool Automatic_scaling(const size_t& temporary_variable)
            {
                //检查string空间大小，来分配内存
                if(temporary_variable <= _capacity)
                {
                    //防止无意义频繁拷贝
                    return true;
                }
                char* temporary_ = new char[temporary_variable+1];
                if(temporary_)
                {
                    strcpy(temporary_,_data);
                    delete[] _data;
                    _data = temporary_;
                    _capacity = temporary_variable;
                    return true;
                }
                return false;
            }
            string& push_back(const char& c_temp_str)
            {
                if(_size == _capacity)
                {
                    size_t newcapacity = _capacity == 0 ? 2 :_capacity*2;
                    if(Automatic_scaling(newcapacity) != true)
                    {
                        std::cout << "开辟内存失败！" << std::endl;
                        return *this;
                    }
                }
                _data[_size] = c_temp_str;
                ++_size;
                _data[_size] = '\0';
                return *this;
            }
            string& push_back(const string& cpp_temp_str)
            {
                size_t len = _size + cpp_temp_str._size;
                if(len > _capacity)
                {
                    size_t new_capacity = len;
                    if(Automatic_scaling(new_capacity) != true)
                    {
                        std::cout << "开辟内存失败！" << std::endl;
                        return *this;
                    }
                }
                strcpy(_data+_size,cpp_temp_str._data);
                _size =_size + cpp_temp_str._size;
                _data[_size] = '\0';
                return *this;
            }
            string& push_back(const char* c_temp_str)
            {
                size_t len = strlen( c_temp_str );
                size_t new_capacity = len + _size ;
                if(new_capacity >_capacity)
                {
                    if(Automatic_scaling( new_capacity) != true)
                    {
                        std::cout << "开辟内存失败！" << std::endl;
                        return *this;
                    }
                }
                strcpy(_data+_size , c_temp_str);
                _size = _size + len;
                _data[_size] = '\0';
                return *this;
            }
            string& resize(const size_t& new_size ,const char& c_temp_str = '\0')
            {
                //扩展字符串长度
                if(new_size >_capacity)
                {
                    //长度大于容量，重新开辟内存
                    if(Automatic_scaling(new_size) != true)
                    {
                        std::cout << "开辟内存失败！" << std::endl;
                        return *this;
                    }
                    for(string::iterator originate = _data + _size;originate != _data + new_size;originate++)
                    {
                        *originate = c_temp_str;
                    }
                    _size = new_size;
                    _data[_size] = '\0';
                }
                else
                {
                    //如果新长度小于当前字符串长度，直接截断放'\0'
                    _size = new_size;
                    _data[_size] = '\0';
                }
                return *this;
            }
            iterator reserve(const size_t& new_capacity)
            {
                if(Automatic_scaling(new_capacity) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                }
                return _data;
                //返回首地址迭代器
            }
            string& swap_s(string& data_str)
            {
                std::swap(_data,data_str._data);
                std::swap(_size,data_str._size);
                std::swap(_capacity,data_str._capacity);
                return *this;
            }
            string rollback()
            {
                if(_size == 0)
                {
                    std::cout << "回滚失败，字符串为空！" << std::endl;
                    return string();
                }
                string _rollback_temp;

                // for(size_t i = _size - 1; i != nops; i--)
                // {
                //     _rollback_temp.push_back(_data[i]);
                // }
                for(string::const_reverse_iterator rollback = rbegin();rollback != rend();rollback--)
                {
                    _rollback_temp.push_back(*rollback);
                }
                return _rollback_temp;
            }
            string rollback_limit(const size_t& limit_begin , const size_t& limit_end)
            {
                string _rollback_linit_temp;
                if(limit_begin > _size || limit_end > _size || limit_begin > limit_end ||_size == 0)
                {
                    std::cout << "回滚位置越界！" << std::endl;
                    return string();
                }
                // for(size_t i = limit_end - 1; i != limit_begin - 1; i--)
                // {
                //     //[]遍历
                //     _rollback_linit_temp.push_back(_data[i]);
                // } 

                for(string::const_reverse_iterator rollback = _data + limit_end - 1;rollback != _data + limit_begin - 1;rollback--)
                {
                    _rollback_linit_temp.push_back(*rollback);
                }
                return _rollback_linit_temp;
            }
            void string_print()
            {
                for(string::const_iterator originate = begin();originate != end();originate++)
                {
                    std::cout << *originate;
                }
                std::cout << std::endl;
            }
            void string_print_reverse()
            {
                for(string::const_reverse_iterator originate = rbegin();originate != rend();originate--)
                {
                    std::cout << *originate;
                }
                std::cout << std::endl;
            }
            friend std::ostream& operator<<(std::ostream& string_ostream,const string &data_str);
            friend std::ostream& operator<<(std::ostream& string_ostream,string &data_str);
            friend std::istream& operator>>(std::istream& string_istream,string &data_str);
            string& operator=(const string &data_str)
            {
                //防止无意义拷贝
                if(this != &data_str)
                {
                    delete [] _data;
                    size_t capacity = data_str._capacity;
                    _data = new char[capacity + 1];
                    strcpy(_data,data_str._data);
                    _capacity = data_str._capacity;
                    _size = data_str._size;
                    _data[_size] = '\0';
                }
                return *this;
            }
            string& operator+=(const string& data_str)
            {
                size_t len = _size + data_str._size;
                if(Automatic_scaling(len) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return *this;
                }
                strcpy(_data + _size,data_str._data);
                _size = _size + data_str._size;
                _data[_size] = '\0';
                return *this;
            }
            char& operator[](const size_t& ergodic_value)
            {
                //引用就是出了函数作用域还能用其他的变量名访问，不需要拷贝就能访问，所以可以直接返回引用减少内存开销
                //在函数创建的变量出了函数作用域就不能访问了，这下才要返回拷贝值，如果返回引用就会未定义
                if(ergodic_value >= _size)
                {
                    //如果越界了就返回第一个元素的引用
                    return _data[0];
                }
                return _data[ergodic_value]; //返回第ergodic_value个元素的引用
                //就像_data在外面就能访问它以及它的成员，所以这种就可以理解成出了函数作用域还在，进函数之前也能访问的就是引用
            }
            const char& operator[](const size_t& ergodic_value)const
            {
                if(ergodic_value >= _size)
                {
                    //如果越界了就返回第一个元素的引用
                    return _data[0];
                }
                return _data[ergodic_value]; 
            }
            string operator+(const string& cpp_str_)
            {
                string _str_temp;
                size_t _str_temp_len = _size + cpp_str_._size;
                if(_str_temp.Automatic_scaling(_str_temp_len) != true)
                {
                    std::cout << "开辟内存失败！" << std::endl;
                    return string();
                }
                strcpy(_str_temp._data , _data);
                strcpy(_str_temp._data + _size , cpp_str_._data);
                _str_temp._size = _size + cpp_str_._size;
                _str_temp._data[_str_temp._size] = '\0';
                return _str_temp;
            }
    };
    std::ostream& operator<<(std::ostream& string_ostream,string &data_str) 
    {
        //当前没实现【】访问.可以用迭代器
        for(wang::string::const_iterator originate = data_str.begin();originate != data_str.end();originate++)
        {
            string_ostream << *originate;
        }
        return string_ostream;
    }
    std::ostream& operator<<(std::ostream& string_ostream,const string &data_str) 
    {
        for(size_t i = 0;i < data_str._size;i++)
        {
            string_ostream << data_str._data[i];
        }
        return string_ostream;
    }
    std::istream& operator>>(std::istream& string_istream, string &data_str)
    {
        while(true)
        {
            char C_istream_str = string_istream.get();
            //gat函数只读取一个字符
            if(C_istream_str == '\n' || C_istream_str == EOF)
            {
                break;
            }
            else
            {
                data_str.push_back(C_istream_str);
            }
        }
        return string_istream;
    }
}

int main()
{
    wang::string string_test1("hello");
    wang::string string_test2("world");
    
    wang::string string_test3 = string_test1 + string_test2;
    std::cout << "string_test3: " << string_test3 << std::endl;
    string_test3.push_back('!');
    const char* insert_str = "inserted";
    string_test3.nose_Insertion_substrings(insert_str);
    std::cout << "str3 after insertion: " << string_test3 << std::endl;


    size_t old_pos = strlen(insert_str);
    wang::string string_test4 = string_test3.str_withdraw(old_pos);
    std::cout << "string_test4: " << string_test4 << std::endl;


    std::cout << string_test3.conversions_oldest() << std::endl;
    std::cout << string_test3.conversions_few() << std::endl;


    wang::string string_test5 = string_test3.str_withdraw_extremity(5);
    std::cout << "string_test5: " << string_test5 << std::endl;


    wang::string string_test6 = string_test3.str_withdraw_detail(5, 10);
    std::cout << "string_test6: " << string_test6 << std::endl;


    std::cout << "str3 size: " << string_test3.size() << std::endl;
    std::cout << "str3 capacity: " << string_test3.capacity() << std::endl;
    std::cout << "string_test3 after resize: " << string_test3.resize(21, '*') << std::endl;


    std::cout << "string_test3 after rollback: " << string_test3.rollback() << std::endl;


    std::cout << "string_test3 after rollback_limit: " << string_test3.rollback_limit(5, 10) << std::endl;

    string_test3.string_print();
    string_test3.string_print_reverse();

    for(auto i :string_test3)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    for(wang::string::const_iterator i = string_test3.begin();i != string_test3.end();i++)
    {
        std::cout << *i << " ";
    }
    std::cout << std::endl;
    //测试Linux上传GitHub
    return 0;
}