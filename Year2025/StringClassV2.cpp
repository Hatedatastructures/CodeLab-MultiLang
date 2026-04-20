#include <iostream>
#include <cstring>
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
        size_t size()
        {
            //返回有效字符串长度
            return _size;
        }
        size_t capacity()
        {
            //返回容量
            return _capacity;
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
            if(automaticScaling(len) != true)
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
            if(_str_temp.automaticScaling(_str_temp_len) != true)
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
        string& conversionsOldest()
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
        string& conversionsFew()
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
        string& noseInsertionSubstrings(const char*& c_str_substring)
        {
            //前部插入子串
            size_t len = strlen(c_str_substring);
            size_t new_nose_insert_substrings = _size + len;
            if(automaticScaling(new_nose_insert_substrings) != true)
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
        string& interlocutoryInsertionSubstrings(const char*& c_str_substring,const size_t& oid_pos)
        {
            //中间位置插入子串
            if(oid_pos > _size)
            {
                std::cout << "插入位置越界！" << std::endl;
                return *this;
            }
            size_t len = strlen(c_str_substring);
            size_t new_interlocutory_insert_substrings = _size + len;
            if(automaticScaling(new_interlocutory_insert_substrings) != true)
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
        string strWithdraw(const size_t& old_pos)
        {
            //提取字串到'\0'
            if(old_pos > _size)
            {
                std::cout << "提取位置越界！" << std::endl;
                return string();
            }
            string _strWithdraw_temp;
            size_t _strWithdraw_temp_len = _size - old_pos;
            if(_strWithdraw_temp.automaticScaling(_strWithdraw_temp_len) != true)
            {
                std::cout << "开辟内存失败！" << std::endl;
                return string();
            }
            strcpy(_strWithdraw_temp._data , _data + old_pos);
            _strWithdraw_temp._size = _strWithdraw_temp_len;
            _strWithdraw_temp._data[_strWithdraw_temp._size] = '\0';
            return _strWithdraw_temp;
        }
        string strWithdrawExtremity(const size_t& old_begin)
        {
            //提取字串到末尾
            if(old_begin > _size)
            {
                std::cout << "提取位置越界！" << std::endl;
                return string();
            }
            string _strWithdrawExtremity_temp;
            size_t _strWithdrawExtremity_temp_len = _size - old_begin;
            if(_strWithdrawExtremity_temp.automaticScaling(_strWithdrawExtremity_temp_len) != true)
            {
                std::cout << "开辟内存失败！" << std::endl;
                return string();
            }
            strcpy(_strWithdrawExtremity_temp._data , _data + old_begin);
            _strWithdrawExtremity_temp._size = _strWithdrawExtremity_temp_len;
            _strWithdrawExtremity_temp._data[_strWithdrawExtremity_temp._size] = '\0';
            return _strWithdrawExtremity_temp;
        }
        string strWithdrawDetail(const size_t& old_begin ,const size_t& old_end)
        {
            //提取字串到指定位置
            if(old_begin > _size || old_end > _size || old_begin > old_end)
            {
                std::cout << "提取位置越界！" << std::endl;
                return string();
            }
            string _strWithdrawDetail_temp;
            size_t _strWithdrawDetail_temp_len = old_end - old_begin;
            if(_strWithdrawDetail_temp.automaticScaling(_strWithdrawDetail_temp_len) != true)
            {
                std::cout << "开辟内存失败！" << std::endl;
                return string();
            }
            strcpy(_strWithdrawDetail_temp._data , _data + old_begin);
            _strWithdrawDetail_temp._size = _strWithdrawDetail_temp_len;
            _strWithdrawDetail_temp._data[_strWithdrawDetail_temp._size] = '\0';
            return _strWithdrawDetail_temp;
        }
        bool automaticScaling(const size_t& temporary_variable)
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
                if(automaticScaling(newcapacity) != true)
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
                if(automaticScaling(new_capacity) != true)
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
                if(automaticScaling( new_capacity) != true)
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
                if(automaticScaling(new_size) != true)
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
            if(automaticScaling(new_capacity) != true)
            {
                std::cout << "开辟内存失败！" << std::endl;
            }
            return _data;
            //返回首地址迭代器
        }
        string& swapS(string& data_str)
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
        string rollbackLimit(const size_t& limit_begin , const size_t& limit_end)
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
        void stringPrint()
        {
            for(string::const_iterator originate = begin();originate != end();originate++)
            {
                std::cout << *originate;
            }
            std::cout << std::endl;
        }
        void stringPrintReverse()
        {
            for(string::const_reverse_iterator originate = rbegin();originate != rend();originate--)
            {
                std::cout << *originate;
            }
            std::cout << std::endl;
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
//模拟实现vector
namespace wang
{
    template <typename VectorType>
    class vector
    {
    public:
        typedef VectorType*       iterator;
        typedef const VectorType* const_iterator;
        typedef VectorType*       reverse_iterator;
        typedef const VectorType* const_reverse_iterator;
    private:
        iterator _data_pointer;     //指向数据的头
        iterator _size_pointer;     //指向数据的尾
        iterator _capacity_pointer; //指向容量的尾
    public:
        iterator begin()
        {
            return _data_pointer;
        }
        iterator end()
        {
            return _size_pointer;
        }
        size_t size()
        {
            return _size_pointer - _data_pointer;
        }
        size_t capacity()
        {
            return _capacity_pointer - _data_pointer;
        }
        size_t size() const
        {
            return _size_pointer - _data_pointer;
        }
        size_t capacity() const 
        {
            return _capacity_pointer - _data_pointer;
        }
        vector()
        {
            _data_pointer = nullptr;
            _size_pointer = nullptr;
            _capacity_pointer = nullptr;
        }
        vector(const size_t& sum_size , const VectorType& data = VectorType())
        :_data_pointer(new VectorType[sum_size]),_size_pointer(_data_pointer + sum_size)
        ,_capacity_pointer(_data_pointer + sum_size)
        {
            for(size_t i = 0;i < sum_size;i++)
            {
                _data_pointer[i] = data;
            }
        }
        VectorType& head()
        {
            return _data_pointer;
        }
        VectorType& tail()
        {
            return _size_pointer;
        }
        VectorType& find(const size_t& find_szie_)
        {
            return _data_pointer[find_szie_];
        }
        vector<VectorType>& Completion(const size_t& Completion_szie_ , const vector<VectorType>& Completion_temp_)
        {
            size_t Completion_temp_szie_ =size();
            size_t Completion_capacity_  =capacity();
            if(Completion_szie_ > Completion_capacity_)
            {
                resize(Completion_szie_);
                for(size_t i = Completion_capacity_; i < Completion_szie_ ; i++)
                {
                    _data_pointer[i] = Completion_temp_;
                }
            }
            else
            {
                if(Completion_szie_ > Completion_temp_szie_)
                {
                    for(size_t i = Completion_temp_szie_; i < Completion_szie_ ; i++)
                    {
                        _data_pointer[i] = Completion_temp_;
                    }
                }
                else if (Completion_szie_ < Completion_temp_szie_)
                {
                    _size_pointer = _data_pointer + Completion_szie_;
                }
            }
            return *this;
        }
        vector(const vector<VectorType>& temp_data)
        {
            _data_pointer = new VectorType [temp_data._capacity_pointer - temp_data._data_pointer];
            for(size_t i = 0; i < (size_t)(temp_data._capacity_pointer - temp_data._data_pointer); i++)
            {
                _data_pointer[i] = temp_data._data_pointer[i];
            }
            _size_pointer = _data_pointer + (temp_data._size_pointer - temp_data._data_pointer);
            _capacity_pointer = _data_pointer + (temp_data._capacity_pointer - temp_data._data_pointer);
        }
        ~vector()
        {
            delete[]_data_pointer;
            _data_pointer = _size_pointer =_capacity_pointer =nullptr;
        }
        void swap_v(vector<VectorType>& temp_data)
        {
            std::swap(_data_pointer, temp_data._data_pointer);
            std::swap(_size_pointer, temp_data._size_pointer);
            std::swap(_capacity_pointer, temp_data._capacity_pointer);
        }
        iterator erase(iterator pos)
        {
            //删除元素
            iterator temp = pos + 1;
            while (temp != _size_pointer)
            {
                //(temp-1)就是pos的位置，从pos位置开始覆盖，覆盖到倒数第1个结束，最后一个会被--屏蔽掉
                *(temp-1) = *temp;
                temp++;
            }
            --_size_pointer;
            return pos;
        }
        vector<VectorType>& resize(const size_t& new_capacity)
        {
            size_t old_size = size();  // 先保存原来的元素数量
            if ((size_t)(_capacity_pointer - _data_pointer) < new_capacity) 
            {
                //涉及到迭代器失效问题，不能调用szie_v()函数，会释放未知空间
                iterator new_data = new VectorType[new_capacity]; 
                // 复制原先的数据
                for (size_t i = 0; i < old_size; i++) 
                {
                    new_data[i] = _data_pointer[i];
                }
                delete [] _data_pointer;
                _data_pointer = new_data;
                _size_pointer = _data_pointer + old_size;  // 使用 old_size 来重建 _size_pointer
                _capacity_pointer = _data_pointer + new_capacity;
            }
            return *this;
        }
        vector<VectorType>& push_back(const VectorType& push_back_temp_)
        {
            if(_size_pointer == _capacity_pointer)
            {
                size_t push_banck_size_ = _data_pointer == nullptr ? 10 : (size_t)(_capacity_pointer-_data_pointer)*2;
                resize(push_banck_size_);
            }
            //注意—_szie_pointer是原生迭代器指针，需要解引用才能赋值
            *_size_pointer = push_back_temp_;
            _size_pointer++;
            return *this;
        }
        vector<VectorType>& pop_back() 
        {
            if (_size_pointer > _data_pointer) 
            { // 至少有一个元素
                --_size_pointer; // 尾指针前移
            }
            return *this;
        }
        vector<VectorType>& push_front(const VectorType& pop_back_temp_)
        {
            //头插
            if(_size_pointer == _capacity_pointer)
            {
                size_t pop_banck_size_ = _data_pointer == nullptr ? 10 : (size_t)(_capacity_pointer-_data_pointer)*2;
                resize(pop_banck_size_);
            }
            _size_pointer++;
            for(size_t pop_back_for_szie = size();pop_back_for_szie>0;pop_back_for_szie--)
            {
                _data_pointer[pop_back_for_szie] = _data_pointer[pop_back_for_szie -1];
            }
            *_data_pointer = pop_back_temp_;
            return *this;
        }
        vector<VectorType>& pop_front()
        {
            for(size_t i = 0;i<size();i++)
            {
                _data_pointer[i] = _data_pointer[i+1];
            }
            --_size_pointer;
            return *this;
        }
        VectorType& operator[](const size_t& _size_operator)
        {
            return _data_pointer[_size_operator];
        }
        const VectorType& operator[](const size_t& _size_operator)const 
        {
            return _data_pointer[_size_operator];
        }
        vector<VectorType>& operator=(const vector<VectorType>&vector_temp_)
        {
            if (this != &vector_temp_) 
            {
                delete[] _data_pointer; // 释放原有内存
                size_t new_cap = vector_temp_.capacity();
                _data_pointer = new VectorType[new_cap];
                _size_pointer = _data_pointer + vector_temp_.size();
                _capacity_pointer = _data_pointer + new_cap;
                for (size_t i = 0; i < vector_temp_.size(); ++i) 
                {
                    _data_pointer[i] = vector_temp_[i]; 
                    // 深拷贝元素
                }
            }
            return *this;
        }
        vector<VectorType>& operator+=(const vector<VectorType>& _temp_)
        {
            if(_temp_.size() == 0|| _temp_._data_pointer == nullptr)
            {
                return *this;
            }
            size_t _temp_szie_ = _temp_.size();
            size_t _szie_ = size();
            size_t _capacity_ = capacity();
            if(_temp_szie_ + _szie_ > _capacity_)
            {
                resize(_temp_szie_ + _szie_);
               
            } 
            size_t sum = 0;
            for(size_t i = _szie_ ; i < (_temp_szie_ + _szie_); i++)
            {
                _data_pointer[i] = _temp_._data_pointer[sum++];
            }
            _size_pointer = _data_pointer + (_temp_szie_ + _szie_);
            return *this;
        }
        template <typename const_vector_output_templates>
        friend std::ostream& operator<< (std::ostream& vector_ostream, const vector<const_vector_output_templates>& Dynamic_arrays_data);
    };
    template <typename const_vector_output_templates>
    std::ostream& operator<<(std::ostream& vector_ostream, const vector<const_vector_output_templates>& Dynamic_arrays_data)
    {
        for(size_t i = 0; i < Dynamic_arrays_data.size(); i++)
        {
            vector_ostream << Dynamic_arrays_data[i] << " ";
        }
        return vector_ostream;
    }
}
int main()
{
    //警惕迭代器失效问题，在扩容数据，移动数据，提前更新迭代器，或者提前保存有效数据个数
    wang::vector<int> test(5,1);
    for(auto i: test)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    wang::vector<int> test1(test);
    for(const  auto& i  : test1 )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    wang::vector<int> test2 = test1;
    for(const auto i : test2)
    {
        std::cout << i << " ";
    }
    wang::string s2 = "name";
    std::cout << std::endl;
    wang::vector<wang::string> name_test(10,s2);
    for(const auto& i : name_test )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    wang::vector<wang::string> name_test1 =name_test ;
    for(const auto& i : name_test1 )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    wang::string s3 = "hello word!";
    name_test1.push_back(s3);
    for(const auto& i : name_test1 )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    name_test1.push_front(s3);
    for(const auto& i : name_test1 )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    name_test1+=name_test;
    for(const auto& i : name_test1 )
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << name_test1 << std::endl;
    std::cout << name_test1.pop_back() << std::endl;
    return 0;
}
