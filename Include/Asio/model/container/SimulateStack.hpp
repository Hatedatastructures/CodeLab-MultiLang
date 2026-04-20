#pragma once
#include "SimulateVector.hpp"
#include "SimulateAlgorithm.hpp"
#include "SimulateException.hpp"
namespace stack_adapter
{
    /*
            * @brief  #### `stack` 类模板

            *   - 自定义栈容器适配器，遵循 LIFO（后进先出）原则

            *   - 基于底层容器实现（默认为 `standard_con::vector`），提供栈的标准操作接口

            * 模板参数:

            * * - `stack_type`: 栈中存储的元素类型
            *
            * * - `vector_based_stack`: 底层容器类型，默认为 `standard_con::vector<stack_type>`
            *   - 需支持 `push_back()`、`pop_back()`、`back()`、`size()`、`empty()` 等操作

            * 成员变量:

            * * - `vector_object`: 底层容器对象，用于存储栈中的元素

            * 构造函数:

            * * - 默认构造函数: 创建空栈
            *
            * * - 拷贝构造函数: 深拷贝另一个栈的所有元素
            *
            * * - 移动构造函数: 接管另一个临时栈的资源
            *
            * * - 初始化列表构造: 从元素初始化列表构造栈
            *
            * * - 单元素构造: 用单个元素初始化栈

            * 析构函数:

            * * - 默认行为，依赖底层容器的析构函数释放资源

            * 元素操作方法:

            * * - `push()`: 入栈操作（支持拷贝和移动语义）
            *
            * * - `pop()`: 出栈操作（移除栈顶元素，不返回值）
            *
            * * - `top()`: 返回栈顶元素的引用（常量和非常量版本）
            *
            * * - `footer()`: 同 `top()`，返回栈顶元素的引用

            * 容量相关方法:

            * * - `size()`: 返回栈中元素的数量
            *
            * * - `empty()`: 判断栈是否为空

            * 赋值运算符:

            * * - `operator=`: 赋值运算符（支持拷贝赋值和移动赋值）

            * 特性:

            * * - 适配性: 可基于不同底层容器实现（如 vector、list 等）
            *
            * * - 移动语义: 支持高效的资源转移
            *
            * * - 异常安全: 依赖底层容器的异常安全保证

            * 注意事项:

            * * - `pop()` 不返回值: 如需获取栈顶元素，需先调用 `top()` 再调用 `pop()`
            *
            *
            * * - 空栈操作: 在栈为空时调用 `top()` 或 `pop()` 会导致未定义行为
            *
            * * - 底层容器选择: 不同容器类型会影响性能特性（如 vector 随机访问快，list 插入删除快）

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename stack_type, typename vector_based_stack = standard_con::vector<stack_type>>
    class stack
    {
    private:
        vector_based_stack vector_object;

    public:
        ~stack() { ; }

        void push(stack_type &&stack_type_data)
        {
            vector_object.push_back(std::forward<stack_type>(stack_type_data));
        }

        void push(const stack_type &stack_type_data)
        {
            vector_object.push_back(stack_type_data);
        }

        void pop()
        {
            vector_object.pop_back();
        }

        uint64_t size() noexcept
        {
            return vector_object.size();
        }

        [[nodiscard]] stack_type &top() const noexcept
        {
            return vector_object.back();
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return vector_object.empty();
        }

        explicit stack(const stack<stack_type> &stack_data)
        {
            vector_object = stack_data.vector_object;
        }

        stack_type &footer() noexcept
        {
            return vector_object.back();
        }

        explicit stack(stack<stack_type> &&stack_data) noexcept
        {
            vector_object = std::move(stack_data.vector_object); // std::move将对象转换为右值引用
        }
        stack(std::initializer_list<stack_type> stack_type_data)
        {
            for (auto &chained_values : stack_type_data)
            {
                vector_object.push_back(chained_values);
            }
        }
        explicit stack(const stack_type &stack_type_data)
        {
            vector_object.push_back(stack_type_data);
        }
        stack &operator=(const stack<stack_type> &stack_data)
        {
            if (this != &stack_data)
            {
                vector_object = stack_data.vector_object;
            }
            return *this;
        }
        stack &operator=(stack<stack_type> &&stack_data) noexcept
        {
            if (this != &stack_data)
            {
                vector_object = std::move(stack_data.vector_object);
            }
            return *this;
        }
        stack() = default;
    };
}
namespace standard_con
{
    using stack_adapter::stack;
}