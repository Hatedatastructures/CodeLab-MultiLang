#pragma once
#include "SimulateException.hpp"
namespace practicality
{
    /*
   * @brief  #### `pair` 类模板

    *   - 常用于函数返回多值、哈希表键值对等场景

   * 模板参数:

   * * - `pair_data_type_example_t`: 第一个元素的类型（`first` 成员）
   *
   * * - `pair_data_type_example_k`: 第二个元素的类型（`second` 成员）

   * 成员变量:

   * * - `T first`: 存储第一个值

   * * - `K second`: 存储第二个值

   * 构造函数:

   * * - 默认构造函数: 值初始化 `first` 和 `second`
   *
   * * - 带参构造函数: 用给定值初始化 `first` 和 `second`
   *
   * * - 拷贝构造函数: 复制另一个 `pair` 的值
   *
   * * - 移动构造函数: 移动另一个 `pair` 的值（右值引用）

   * 赋值运算符:

   * * - 拷贝赋值: 复制另一个 `pair` 的值
   *
   * * - 移动赋值: 移动另一个 `pair` 的值（右值引用）

   * 比较运算符:

   * * - `operator==`: 比较两个 `pair` 的值是否相等

   * * - `operator!=`: 比较两个 `pair` 的值是否不等

   * 特殊方法:

   * * - `operator->`: 返回指向自身的指针，用于成员访问

   * * - 友元 `operator<<`: 支持流输出，格式为 "(first:second)"

   * 特性:
   * * - 所有操作均为 `noexcept`（前提是元素类型的对应操作也 `noexcept`）
   *
   * * - 支持完美转发（Perfect Forwarding）的移动语义
   *
   * * - 轻量级结构，无额外内存开销

   * 注意事项:

   * * - 元素类型 `T` 和 `K` 需支持默认构造（若使用默认构造函数）

   * * - 比较操作依赖 `T` 和 `K` 的 `==` 运算符实现

   * * - 流输出 `operator<<` 要求 `T` 和 `K` 支持 `operator<<`

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
*/
    template <typename pair_data_type_example_t, typename pair_data_type_example_k>
    class pair
    {
        using T = pair_data_type_example_t;
        using K = pair_data_type_example_k;

    public:
        T first;
        K second;
        pair() noexcept
        {
            first = T();
            second = K();
        }

        pair(const T &_first, const K &_second) noexcept
        {
            first = _first;
            second = _second;
        }
        pair(const pair &other) noexcept
        {
            first = other.first;
            second = other.second;
        }

        pair(T &&_first, K &&_second) noexcept
                : first(std::forward<T>(_first)), second(std::forward<K>(_second))
        {
            ;
        }
        pair(pair &&other) noexcept
                : first(std::move(other.first)), second(std::move(other.second))
        {
            ;
        }
        pair &operator=(const pair &other) noexcept
        {
            if (this != &other)
            {
                first = other.first;
                second = other.second;
            }
            return *this;
        }
        pair &operator=(pair &&other) noexcept
        {
            if (this != &other)
            {
                first = std::move(other.first);
                second = std::move(other.second);
            }
            return *this;
        }
        bool operator==(const pair &other) const noexcept
        {
            return (this == &other) ? true : (first == other.first && second == other.second);
        }
        bool operator==(const pair &other) noexcept
        {
            return this == &other ? true : (first == other.first && second == other.second);
        }
        bool operator!=(const pair &other) noexcept
        {
            return !(*this == other);
        }
        pair *operator->() noexcept { return this; }
        const pair *operator->() const noexcept { return this; }
        template <typename pair_ostream_t, typename pair_ostream_k>
        friend std::ostream &operator<<(std::ostream &os, const pair<pair_ostream_t, pair_ostream_k> &p);
    };
    template <typename pair_ostream_t, typename pair_ostream_k>
    std::ostream &operator<<(std::ostream &os, const pair<pair_ostream_t, pair_ostream_k> &p)
    {
        os << "(" << p.first << ":" << p.second << ")";
        return os;
    }
    /*
   * @brief  #### `make_pair` 函数模板

    *   - 创建并返回一个 `pair` 对象，自动推导元素类型

   * 模板参数:

   * * - `make_pair_t`: 第一个元素的类型（自动推导）

   * * - `make_pair_k`: 第二个元素的类型（自动推导）

   * 参数:

   * * - `_first`: 用于初始化 pair 第一个元素的值

   * * - `_second`: 用于初始化 pair 第二个元素的值

   * 返回值:

   * * - 一个 `pair<make_pair_t, make_pair_k>` 对象，包含传入的两个值

   * 特性:

   * * - 类型推导: 自动根据传入参数类型确定 pair 的元素类型

   * * - 拷贝语义: 参数按 const 引用传递，内部进行值拷贝

   * * - 简单高效: 直接构造 pair 对象，无额外内存开销

   * 典型应用场景:

   * * - 作为函数返回值返回多个值

   * * - 快速构造 pair 对象，避免显式指定模板参数

   * * - 与容器（如 `map`、`unordered_map`）配合使用，插入键值对

   * 注意事项:

   * * - 元素类型会按值推导规则处理（如 `const char*` 不会自动转换为 `std::string`）

   * * - 如需引用语义或完美转发，建议直接使用 pair 构造函数

   * * - 对于字符串字面量，返回的 pair 元素类型为 `const char*`，可能需要显式转换

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename make_pair_t, typename make_pair_k>
    pair<make_pair_t, make_pair_k> make_pair(const make_pair_t &_first, const make_pair_k &_second)
    {
        return pair<make_pair_t, make_pair_k>(_first, _second);
    }
}
namespace standard_con
{
    using practicality::make_pair;
    using practicality::pair;
}