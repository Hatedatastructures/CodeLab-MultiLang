#pragma once
#include "SimulateException.hpp"
// #include "String.hpp"
// #include "Vector.hpp"
// #include "List.hpp"

namespace imitation_functions
{
    /*
   * @brief  #### `less` 类

    *   - 仿函数（函数对象）实现，用于比较两个同类型对象的大小关系

    *   - 采用小于运算符（`<`）定义比较逻辑，符合严格弱序规则

   * 模板参数:

   * * - `imitation_functions_less`: 待比较的对象类型，需支持 `<` 运算符重载

   * 核心操作符:
   * * - `constexpr bool operator()(const imitation_functions_less& _test1, const imitation_functions_less& _test2) noexcept`
   *
   *     - 功能：比较两个对象的大小，返回 `_test1 < _test2` 的结果
   *
   *     - 参数：`_test1` 和 `_test2` 为待比较的常量引用
   *
   *     - 返回值：若 `_test1` 小于 `_test2` 则返回 `true`，否则返回 `false`
   *
   *     - 特性：`constexpr` 支持编译期计算，`noexcept` 保证不抛出异常

   * 适用场景:
   * * - 作为排序算法（如快速排序、归并排序）的比较器
   *
   * * - 用于关联容器（如 `tree_map`、`tree_set`）的键值排序
   *
   * * - 任何需要自定义小于比较逻辑的场景

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

   * 注意事项:

   * * - 依赖类型 `imitation_functions_less` 已正确重载 `<` 运算符

   * * - 若用于自定义类型，需确保 `<` 运算符满足严格弱序（非自反、传递性等）

   * * - 线程安全：无共享状态，可在多线程环境中安全使用
    */
    template <typename imitation_functions_less>
    class less
    {
    public:
        constexpr bool operator()(const imitation_functions_less &_test1, const imitation_functions_less &_test2) noexcept
        {
            return _test1 < _test2;
        }
    };
    /*
   * @brief  #### `greater` 类模板

    *   - 实现"大于"比较的函数对象（仿函数）

    *   - 用于定义对象间的严格弱序关系，与标准库 `std::greater` 功能一致

   * 模板参数:

   * * - `imitation_functions_greater`: 待比较对象的类型（需支持 `>` 运算符重载）

   * 核心操作符:
   *
   *     - 功能：比较两个对象的大小，返回 `_test1 > _test2` 的结果
   *
   *     - 参数：`_test1` 和 `_test2` 为待比较的常量引用
   *
   *     - 返回值：若 `_test1` 大于 `_test2` 则返回 `true`，否则返回 `false`
   *
   *     - 特性：`constexpr` 支持编译期计算，`noexcept` 保证不抛出异常

   * 典型应用场景:

   *
   * * - 作为关联容器（如 `std::map`、`std::set`）的键比较函数，构建降序容器
   *
   * * - 用于自定义算法中的比较操作，替代默认的小于比较

   * 注意事项:

   * * - 依赖类型 `imitation_functions_greater` 正确实现 `>` 运算符
   *
   * * - 比较操作需满足严格弱序关系（非自反性、传递性、反对称性）
   *
   * * - 若用于指针类型，比较的是地址值而非指向的内容
   *
   * * - 线程安全：无状态类，可在多线程环境中安全使用

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename imitation_functions_greater>
    class greater
    {
    public:
        constexpr bool operator()(const imitation_functions_greater &_test1, const imitation_functions_greater &_test2) noexcept
        {
            return _test1 > _test2;
        }
    };
}
namespace standard_con
{
    using imitation_functions::greater;
    using imitation_functions::less;
}
