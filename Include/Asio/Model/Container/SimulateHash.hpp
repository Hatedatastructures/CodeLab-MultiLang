#pragma once
#include "SimulateString.hpp"
namespace hash
{
    /*
   * @brief  #### `hash_imitation_functions` 类

    *   - 哈希仿函数实现，为基础数据类型提供哈希值计算

    *   - 支持多种内置类型的哈希转换，可作为哈希容器的默认哈希函数

   * 核心操作符:

   * * - 针对不同基础类型重载的 `operator()`，均返回 `uint64_t` 类型的哈希值
   *
   *     - `int`: 直接转换为 `uint64_t`
   *
   *     - `uint64_t`: 直接返回原值
   *
   *     - `char`: 转换为对应的 `uint64_t` 值
   *
   *     - `double`: 转换为 `uint64_t`（注意：可能丢失精度）
   *
   *     - `float`: 转换为 `uint64_t`（注意：可能丢失精度）
   *
   *     - `long`: 转换为 `uint64_t`
   *
   *     - `short`: 转换为 `uint64_t`
   *
   *     - `long long`: 转换为 `uint64_t`
   *
   *     - `unsigned int`: 转换为 `uint64_t`
   *
   *     - `unsigned short`: 转换为 `uint64_t`

   * 特性:

   * * - 针对基础类型的哈希计算为 O(1) 时间复杂度

   * 适用场景:

   * * - 作为 `hash_map`、`hash_set` 等哈希容器的哈希函数
   *
   * * - 与 `hash_function` 类配合使用，扩展哈希算法
   *
   * * - 需要为基础类型快速生成哈希值的场景

   * 注意事项:

   * * - 浮点类型（`double`、`float`）的哈希可能丢失精度，不建议用于精确匹配场景
   *
   * * - 未提供自定义类型（如字符串、容器）的哈希实现，需用户自行重载扩展
   *
   * * - 对于指针类型或复杂结构，需额外实现重载版本
   *
   * * - 线程安全：无状态设计，可在多线程环境中安全使用

   * 扩展说明:

   * * - 注释中提供了字符串容器哈希计算的示例代码框架，可根据需求实现
   *
   * * - 支持为自定义类型添加 `operator()` 重载，扩展哈希能力
   *
    */
    class hash_imitation_functions
    {
    public:
        template <typename hash_type>
        [[nodiscard]] uint64_t operator()(const hash_type &data) noexcept
        {
            return static_cast<uint64_t>(data);
        }
        [[nodiscard]] uint64_t operator()(const standard_con::string &data_string) noexcept
        {
            uint64_t hash_value = 0;
            for (auto &str : data_string)
            {
                hash_value += static_cast<uint64_t>(str) * 31;
            }
            return hash_value;
        }
        // 有需要可以重载本文件的string容器和vector容器.list容器等计算哈希的函数, 这里就不重载了
    };
    /*
   * @brief  #### `hash_algorithm` 哈希算法命名空间
   *
   *   - `hash_function`： 提供了五种哈希函数，提供多种哈希算法实现，基于基础哈希仿函数扩展计算
   *
   */
    namespace hash_algorithm
    {
        /*
                * @brief  #### `hash_function` 类

                *   - 哈希函数适配器，提供多种哈希算法实现，基于基础哈希仿函数扩展计算

                *   - 支持对不同类型数据进行哈希计算，适用于哈希表、布隆过滤器等场景

                * 模板参数:

                * * - `hash_algorithm_type`: 待计算哈希值的数据类型
                *
                * * - `hash_if`: 基础哈希仿函数类型，默认为 `standard_con::imitation_functions::hash_imitation_functions`
                *
                *   （需实现 `operator()` 用于生成基础哈希值）

                * 构造与析构:

                * * - `constexpr hash_function()`:  constexpr 构造函数，初始化基础哈希仿函数对象
                *
                * * - 析构函数: 默认析构，无需额外资源清理

                * 核心哈希方法:

                * * - `hash_sdmmhash(const hash_algorithm_type& data_hash)`
                *
                * * - `hash_bkdrhash(const hash_algorithm_type& data_hash)`
                *
                * * - `hash_djbhash(const hash_algorithm_type& data_hash)`
                *
                * * - `hash_aphash(const hash_algorithm_type& data_hash)`
                *
                * * - `hash_pjwhash(const hash_algorithm_type& data_hash)`

                * 方法特性:

                * * - 所有哈希方法均为 `constexpr`，支持编译期计算

                * 适用场景:
                * * - 哈希表（如 `hash_map`、`hash_set`）的键值哈希计算
                *
                * * - 布隆过滤器（`bloom_filter`）中的多哈希函数实现
                *
                * * - 需要自定义哈希逻辑的容器或算法

                * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

                * 注意事项:
                * * - 依赖基础哈希仿函数 `hash_if` 对 `hash_algorithm_type` 类型的支持（需正确实现 `operator()`）
                *
                * * - 哈希结果的分布性依赖基础哈希值和乘数的选择，不同算法适用于不同数据特征
                *
                * * - 多线程安全：无共享状态，可在多线程环境中安全使用
                *
                * * - 自定义类型需确保基础哈希仿函数能正确处理，或提供自定义类型哈希仿函数
        */
        template <typename hash_algorithm_type, typename hash_if = hash_imitation_functions>
        class hash_function
        {
        public:
            constexpr hash_function()
            {
                hash_imitation_functions_object = hash_if();
            }
            ~hash_function() = default;
            hash_if hash_imitation_functions_object;
            [[nodiscard]] constexpr uint64_t hash_sdmmhash(const hash_algorithm_type &data_hash) noexcept
            {
                uint64_t return_value = hash_imitation_functions_object(data_hash);
                return_value = 65599 * return_value;
                return return_value;
            }
            [[nodiscard]] constexpr uint64_t hash_bkdrhash(const hash_algorithm_type &data_hash) noexcept
            {
                uint64_t return_value = hash_imitation_functions_object(data_hash);
                return_value = 131 * return_value;
                return return_value;
            }
            [[nodiscard]] constexpr uint64_t hash_djbhash(const hash_algorithm_type &data_hash) noexcept
            {
                uint64_t return_value = hash_imitation_functions_object(data_hash);
                return_value = 33 * return_value;
                return return_value;
            }
            [[nodiscard]] constexpr uint64_t hash_aphash(const hash_algorithm_type &data_hash) noexcept
            {
                uint64_t return_value = hash_imitation_functions_object(data_hash);
                return_value = return_value * 1031;
                return return_value;
            }
            [[nodiscard]] constexpr uint64_t hash_pjwhash(const hash_algorithm_type &data_hash) noexcept
            {
                uint64_t return_value = hash_imitation_functions_object(data_hash);
                return_value = (return_value << 2) + return_value;
                return return_value;
            }
        };
    }
}
namespace standard_con
{
    using hash::hash_imitation_functions;
    using hash::hash_algorithm::hash_function;
}