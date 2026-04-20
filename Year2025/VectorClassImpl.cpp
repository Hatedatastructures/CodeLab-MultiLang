#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtemplate-body"
#include <iostream>
#include <string>
#include <algorithm>
template<typename T>
class vector 
{
private:
    T* _DataPointer;
    T* _SizePointer;
    T* _CapacityPointer;

    // 禁止拷贝（为简化示例，实际应实现拷贝语义）
    vector(const vector&) = delete;
    vector& operator=(const vector&) = delete;

public:
    // 默认构造函数
    vector() : _DataPointer(nullptr), _SizePointer(nullptr), _CapacityPointer(nullptr) {}

    // 移动构造函数
    vector(vector&& other) noexcept 
        : _DataPointer(other._DataPointer), 
          _SizePointer(other._SizePointer), 
          _CapacityPointer(other._CapacityPointer) {
        other._DataPointer = nullptr;
        other._SizePointer = nullptr;
        other._CapacityPointer = nullptr;
    }

    // 移动赋值运算符
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            clear();
            operator delete(_DataPointer);
            
            _DataPointer = other._DataPointer;
            _SizePointer = other._SizePointer;
            _CapacityPointer = other._CapacityPointer;
            
            other._DataPointer = nullptr;
            other._SizePointer = nullptr;
            other._CapacityPointer = nullptr;
        }
        return *this;
    }

    // 析构函数
    ~vector() {
        clear();
        operator delete(_DataPointer);
    }

    // 容量相关方法
    [[nodiscard]] size_t size() const { return _SizePointer - _DataPointer; }
    size_t capacity() const { return _CapacityPointer - _DataPointer; }
    bool empty() const { return _DataPointer == _SizePointer; }

    // 内存管理
    void reserve(size_t newCapacity) {
        if (newCapacity <= capacity()) return;
        
        T* newData = static_cast<T*>(operator new(newCapacity * sizeof(T)));
        size_t oldSize = size();
        
        // 移动现有元素
        for (size_t i = 0; i < oldSize; ++i) {
            new (newData + i) T(std::move(_DataPointer[i]));
            _DataPointer[i].~T(); // 显式调用析构函数
        }
        
        // 释放旧内存
        operator delete(_DataPointer);
        
        // 更新指针
        _DataPointer = newData;
        _SizePointer = _DataPointer + oldSize;
        _CapacityPointer = _DataPointer + newCapacity;
    }

    // 清空容器
    void clear() {
        for (T* p = _DataPointer; p != _SizePointer; ++p) {
            p->~T(); // 显式调用析构函数
        }
        _SizePointer = _DataPointer;
    }

    // PushBack 方法
    template<typename U>
    void push_back(U&& value) {
        if (_SizePointer == _CapacityPointer) {
            size_t newCapacity = capacity() == 0 ? 1 : capacity() * 2;
            reserve(newCapacity);
        }
        
        // 使用完美转发
        new (_SizePointer) T(std::forward<U>(value));
        ++_SizePointer;
    }

    // 访问元素（带边界检查）
    T& at(size_t index) {
        if (index >= size()) throw std::out_of_range("Index out of range");
        return _DataPointer[index];
    }

    const T& at(size_t index) const {
        if (index >= size()) throw std::out_of_range("Index out of range");
        return _DataPointer[index];
    }
};
void testVector() {
    vector<std::string> vec;
    
    // 测试添加元素
    vec.push_back("Hello");
    vec.push_back(std::string("World"));
    
    // 测试移动语义
    vector<std::string> vec2 = std::move(vec);
    
    // 测试边界情况
    try 
    {
        vec2.at(10); // 应抛出异常
    } catch (const std::out_of_range& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }
}
int main()
{
    testVector();
    // std::swap(1,2); // removed: invalid rvalue swap
    return 0;
}

#pragma GCC diagnostic pop
