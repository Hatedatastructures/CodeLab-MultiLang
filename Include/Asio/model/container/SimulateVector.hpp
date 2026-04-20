#pragma once
#include "SimulateException.hpp"
#include "SimulateAlgorithm.hpp"
namespace vector_container
{
    /*
   * @brief  #### `vector` 类模板

            *   - 自定义动态数组容器类，用于存储和管理一组元素

            *   - 支持元素的动态添加、删除、访问和修改，自动管理内存

            *   - 提供迭代器遍历、容量自动扩展和异常处理机制

            * 模板参数:

            * * - `vector_type`: 容器中存储的元素类型

            * 类型别名:

            * * - `iterator`: 元素指针类型，用于遍历容器
            *
            * * - `const_iterator`: 常量元素指针类型，用于只读遍历
            *
            * * - `reverse_iterator`: 反向迭代器类型（与 iterator 相同）
            *
            * * - `const_reverse_iterator`: 常量反向迭代器类型（与 const_iterator 相同）

            * 成员变量:

            * * - `_data_pointer`: 指向动态数组的起始地址
            *
            * * - `_size_pointer`: 指向数组中最后一个元素的下一个位置（表示当前元素数量）
            *
            * * - `_capacity_pointer`: 指向数组容量的末尾位置（表示可容纳的最大元素数量）

            * 迭代器相关方法:

            * * - `begin()`: 返回指向容器第一个元素的迭代器
            *
            * * - `end()`: 返回指向容器最后一个元素后一位的迭代器

            * 容量相关方法:

            * * - `size()`: 返回容器当前元素数量
            *
            * * - `capacity()`: 返回容器当前可容纳的最大元素数量
            *
            * * - `empty()`: 判断容器是否为空（元素数量为 0）
            *
            * * - `resize()`: 调整容器容量，不足时填充指定元素，超出时截断
            *
            * * - `size_adjust()`: 调整容器大小，不足时填充指定元素

            * 元素访问方法:

            * * - `front()`: 返回容器第一个元素的引用（同 head()）
            *
            * * - `back()`: 返回容器最后一个元素的引用（同 tail()）
            *
            * * - `head()`: 返回容器第一个元素的引用
            *
            * * - `tail()`: 返回容器最后一个元素的引用
            *
            * * - `find()`: 根据索引查找元素，超出范围时抛出异常
            *
            * * - `operator[]`: 通过索引访问元素（支持读写和只读版本）

            * 构造函数:

            * * - 默认构造函数: 初始化空容器（指针均为 nullptr）
            *
            * * - 带容量和初始值构造: 创建指定容量的容器，并用指定元素初始化
            *
            * * - 初始化列表构造: 从元素初始化列表构造容器
            *
            * * - 拷贝构造函数: 深拷贝另一个容器的元素和容量
            *
            * * - 移动构造函数: 接管另一个临时容器的资源，原容器指针置空

            * 析构函数:

            * * - 释放动态数组内存，重置所有指针为 nullptr

            * 元素修改方法:

            * * - `push_back()`: 向容器末尾添加元素（支持拷贝和移动语义）
            *
            * * - `pop_back()`: 移除容器末尾的元素（尾指针前移）
            *
            * * - `push_front()`: 向容器头部插入元素（元素后移，效率较低）
            *
            * * - `erase()`: 删除指定位置的元素，后续元素前移，返回下一个元素的迭代器
            *
            * * - `swap()`: 与另一个容器交换内部资源（指针和容量信息）

            * 运算符重载:

            * * - `operator=`: 赋值运算符（支持拷贝赋值和移动赋值）
            *
            * * - `operator+=`: 容器拼接，将另一个容器的元素添加到当前容器末尾
            *
            * * - 友元 `operator<<`: 输出容器所有元素到流（元素间用空格分隔）

            * 特性:

            * * - 容量自动扩展: 当元素数量达到容量时，容量翻倍（初始容量为 10）
            *
            * * - 支持移动语义: 减少不必要的元素拷贝，提高性能
            *
            * * - 异常处理: 越界访问等操作会抛出 `fault` 异常
            *
            * * - 迭代器可能失效: 扩容（resize）或删除元素（erase）后，原有迭代器可能失效

            * 注意事项:

            * * - `push_front()` 操作效率较低（需移动所有元素），建议优先使用 `push_back()`
            *
            * * - 扩容会导致原有迭代器失效，需重新获取迭代器
            *
            * * - 越界访问会抛出异常，使用时需确保索引在有效范围内
            *
            * * - 移动构造/赋值后，原容器会被清空（指针置空），不可再使用

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename vector_type>
    class vector
    {
    public:
        using iterator = vector_type *;
        using const_iterator = const vector_type *;
        using reverse_iterator = iterator;
        using const_reverse_iterator = const_iterator;

    private:
        iterator _data_pointer;     // 指向数据的头
        iterator _size_pointer;     // 指向数据的尾
        iterator _capacity_pointer; // 指向容量的尾
    public:
        [[nodiscard]] iterator begin() noexcept
        {
            return _data_pointer;
        }

        [[nodiscard]] iterator end() noexcept
        {
            return _size_pointer;
        }
        [[nodiscard]] uint64_t size() const noexcept
        {
            return _data_pointer ? (_size_pointer - _data_pointer) : 0;
        }

        [[nodiscard]] uint64_t capacity() const noexcept
        {
            return _data_pointer ? (_capacity_pointer - _data_pointer) : 0;
        }

        [[nodiscard]] vector_type &front() const noexcept
        {
            return head();
        }

        [[nodiscard]] vector_type &back() const noexcept
        {
            return tail();
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return size() == 0;
        }

        [[nodiscard]] vector_type &head() const noexcept
        {
            return *_data_pointer;
        }

        [[nodiscard]] vector_type &tail() const noexcept
        {
            return *(_size_pointer - 1);
        }

        vector() noexcept
        {
            _data_pointer = nullptr;
            _size_pointer = nullptr;
            _capacity_pointer = nullptr;
        }
        explicit vector(const uint64_t &container_capacity, const vector_type &vector_data = vector_type())
                : _data_pointer(new vector_type[container_capacity]), _size_pointer(_data_pointer + container_capacity), _capacity_pointer(_data_pointer + container_capacity)
        {
            for (uint64_t corresponding_location = 0; corresponding_location < container_capacity; corresponding_location++)
            {
                _data_pointer[corresponding_location] = vector_data;
            }
        }
        vector(std::initializer_list<vector_type> lightweight_container)
                : _data_pointer(new vector_type[lightweight_container.size()]), _size_pointer(_data_pointer + lightweight_container.size()), _capacity_pointer(_data_pointer + lightweight_container.size())
        {
            // 链式拷贝
            uint64_t corresponding_location = 0;
            for (auto &chained_values : lightweight_container)
            {
                _data_pointer[corresponding_location] = std::move(chained_values);
                corresponding_location++;
            }
        }
        vector_type &find(const uint64_t &find_size)
        {
            try
            {
                if (find_size >= size())
                {
                    throw custom_exception::fault("传入数据超出容器范围", "vector::find", __LINE__);
                }
                else
                {
                    return _data_pointer[find_size];
                }
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        vector<vector_type> &size_adjust(const uint64_t &data_size, const vector_type &padding_temp_data = vector_type())
        {
            uint64_t container_size = size();
            uint64_t container_capacity = capacity();
            if (data_size > container_capacity)
            {
                resize(data_size);
                for (uint64_t assignment_traversal = container_capacity; assignment_traversal < data_size; ++assignment_traversal)
                {
                    _data_pointer[assignment_traversal] = padding_temp_data;
                }
            }
            else
            {
                if (data_size > container_size)
                {
                    for (uint64_t assignment_traversal = container_size; assignment_traversal < data_size; ++assignment_traversal)
                    {
                        _data_pointer[assignment_traversal] = padding_temp_data;
                    }
                }
                else if (data_size < container_size)
                {
                    _size_pointer = _data_pointer + data_size;
                }
            }
            return *this;
        }
        vector(const vector<vector_type> &vector_data)
                : _data_pointer(vector_data.capacity() ? new vector_type[vector_data.capacity()] : nullptr),
                    _size_pointer(_data_pointer + vector_data.size()), _capacity_pointer(_data_pointer + vector_data.capacity())
        {
            for (uint64_t copy_assignment_traversal = 0; copy_assignment_traversal < vector_data.size(); copy_assignment_traversal++)
            {
                _data_pointer[copy_assignment_traversal] = vector_data._data_pointer[copy_assignment_traversal];
            }
        }
        vector(vector<vector_type> &&vector_data) noexcept
        {
            _data_pointer = std::move(vector_data._data_pointer);
            _size_pointer = std::move(vector_data._size_pointer);
            _capacity_pointer = std::move(vector_data._capacity_pointer);
            vector_data._data_pointer = vector_data._size_pointer = vector_data._capacity_pointer = nullptr;
        }
        ~vector() noexcept
        {
            delete[] _data_pointer;
            _data_pointer = _size_pointer = _capacity_pointer = nullptr;
        }
        void swap(vector<vector_type> &vector_data) noexcept
        {
            standard_con::algorithm::swap(_data_pointer, vector_data._data_pointer);
            standard_con::algorithm::swap(_size_pointer, vector_data._size_pointer);
            standard_con::algorithm::swap(_capacity_pointer, vector_data._capacity_pointer);
        }
        iterator erase(iterator delete_position) noexcept
        {
            // 删除元素
            iterator next_position = delete_position + 1;
            while (next_position != _size_pointer)
            {
                *(next_position - 1) = *next_position; //(temp-1)就是pos的位置，从pos位置开始覆盖，覆盖到倒数第1个结束，最后一个会被--屏蔽掉
                ++next_position;
            }
            --_size_pointer;
            return next_position; // 返回下一个位置地址
        }
        vector<vector_type> &resize(const uint64_t &new_container_capacity, const vector_type &vector_data = vector_type())
        {
            try
            {
                uint64_t original_size = size(); // 先保存原来的元素数量
                if (static_cast<uint64_t>(_capacity_pointer - _data_pointer) < new_container_capacity)
                {
                    // 涉及到迭代器失效问题，不能调用size()函数，会释放未知空间
                    auto new_vector_type_array = new vector_type[new_container_capacity];
                    // 复制原先的数据
                    for (uint64_t original_data_traversal = 0; original_data_traversal < original_size; original_data_traversal++)
                    {
                        new_vector_type_array[original_data_traversal] = std::move(_data_pointer[original_data_traversal]);
                    }
                    for (uint64_t assignment_traversal = original_size; assignment_traversal < new_container_capacity; ++assignment_traversal)
                    {
                        new_vector_type_array[assignment_traversal] = vector_data;
                    }
                    delete[] _data_pointer;
                    _data_pointer = new_vector_type_array;
                    _size_pointer = _data_pointer + original_size; // 使用 original_size 来重建 _size_pointer
                    _capacity_pointer = _data_pointer + new_container_capacity;
                }
            }
            catch (const std::bad_alloc &process)
            {
                delete[] _data_pointer;
                _data_pointer = _size_pointer = _capacity_pointer = nullptr;
                std::cerr << process.what() << std::endl;
                throw;
            }
            return *this;
        }
        vector<vector_type> &push_back(const vector_type &vector_type_data)
        {
            if (_size_pointer == _capacity_pointer)
            {
                const uint64_t new_container_capacity = _data_pointer == nullptr ? 10 : static_cast<uint64_t>((_capacity_pointer - _data_pointer) * 2);
                resize(new_container_capacity);
            }
            // 注意—_size_pointer是原生迭代器指针，需要解引用才能赋值
            *_size_pointer = vector_type_data;
            ++_size_pointer;
            return *this;
        }
        vector<vector_type> &push_back(vector_type &&vector_type_data)
        {
            if (_size_pointer == _capacity_pointer)
            {
                const uint64_t new_container_capacity = _data_pointer == nullptr ? 10 : static_cast<uint64_t>((_capacity_pointer - _data_pointer) * 2);
                resize(new_container_capacity);
            }
            // 注意_size_pointer是原生迭代器指针，需要解引用才能赋值
            *_size_pointer = std::move(vector_type_data);
            // new (_data_pointer) vector_type(std::forward<vector_type>(vector_type_data));
            ++_size_pointer;
            return *this;
        }
        vector<vector_type> &pop_back()
        {
            if (_size_pointer > _data_pointer)
            {                  // 至少有一个元素
                --_size_pointer; // 尾指针前移
            }
            return *this;
        }
        vector<vector_type> &push_front(const vector_type &vector_type_data)
        {
            // 头插
            if (_size_pointer == _capacity_pointer)
            {
                const uint64_t new_container_size = _data_pointer == nullptr ? 10 : static_cast<uint64_t>((_capacity_pointer - _data_pointer) * 2);
                resize(new_container_size);
            }
            for (uint64_t container_size = size(); container_size > 0; --container_size)
            {
                _data_pointer[container_size] = _data_pointer[container_size - 1];
            }
            *_data_pointer = vector_type_data;
            ++_size_pointer;
            return *this;
        }
        vector<vector_type> &pop_front()
        {
            if (size() > 0)
            {
                for (uint64_t assignment_traversal = 1; assignment_traversal < size(); assignment_traversal++)
                {
                    _data_pointer[assignment_traversal - 1] = _data_pointer[assignment_traversal];
                }
                --_size_pointer;
            }
            return *this;
        }
        vector_type &operator[](const uint64_t &access_location)
        {
            try
            {
                if (access_location >= capacity())
                {
                    throw custom_exception::fault("传入参数越界", "vector::operatot[]", __LINE__);
                }
                else
                {
                    return _data_pointer[access_location];
                }
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        const vector_type &operator[](const uint64_t &access_location) const
        {
            // return _data_pointer[access_location];
            try
            {
                if (access_location >= capacity())
                {
                    throw custom_exception::fault("传入参数越界", "vector::operatot[]", __LINE__);
                }
                else
                {
                    return _data_pointer[access_location];
                }
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        vector<vector_type> &operator=(const vector<vector_type> &vector_data)
        {
            if (this != &vector_data)
            {
                vector<vector_type> return_vector_object(vector_data); // 拷贝构造
                swap(return_vector_object);                            // 交换资源，temp析构时会释放原资源
            }
            return *this;
        }
        vector<vector_type> &operator=(vector<vector_type> &&vector_mobile_data) noexcept
        {
            if (this != &vector_mobile_data)
            {
                _data_pointer = std::move(vector_mobile_data._data_pointer);
                _size_pointer = std::move(vector_mobile_data._size_pointer);
                _capacity_pointer = std::move(vector_mobile_data._capacity_pointer);
                vector_mobile_data._data_pointer = vector_mobile_data._size_pointer = vector_mobile_data._capacity_pointer = nullptr;
            }
            return *this;
        }
        vector<vector_type> &operator+=(const vector<vector_type> &vector_data)
        {
            if (vector_data.size() == 0 || vector_data._data_pointer == nullptr)
            {
                return *this;
            }
            uint64_t vector_data_size = vector_data.size();
            uint64_t container_size = size();
            uint64_t container_capacity = capacity();
            if (vector_data_size + container_size > container_capacity)
            {
                resize(vector_data_size + container_size);
            }
            uint64_t array_counter = 0;
            for (uint64_t slicing_traversal = container_size; slicing_traversal < (vector_data_size + container_size); ++slicing_traversal)
            {
                _data_pointer[slicing_traversal] = vector_data._data_pointer[array_counter++];
            }
            _size_pointer = _data_pointer + (vector_data_size + container_size);
            return *this;
        }
        template <typename const_vector_output_templates>
        friend std::ostream &operator<<(std::ostream &vector_ostream, const vector<const_vector_output_templates> &dynamic_arrays_data);
    };
    template <typename const_vector_output_templates>
    std::ostream &operator<<(std::ostream &vector_ostream, const vector<const_vector_output_templates> &dynamic_arrays_data)
    {
        for (uint64_t input_traversal = 0; input_traversal < dynamic_arrays_data.size(); input_traversal++)
        {
            vector_ostream << dynamic_arrays_data[input_traversal] << " ";
        }
        return vector_ostream;
    }
}
namespace standard_con
{
    using vector_container::vector;
}