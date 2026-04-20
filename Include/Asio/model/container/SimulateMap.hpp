#pragma once
#include "SimulateBase.hpp"
namespace map_container
{
    /**
   * @brief 基于红黑树实现的有序键值对映射容器
   *
   * 该容器是一个有序关联容器，存储键值对（key-value），并根据键的大小关系自动排序。
   *
   * 底层依赖红黑树（red_black_tree）实现，保证插入、删除、查找等操作的时间复杂度为 O(log n)。
   *
   * 键具有唯一性，不允许重复键，默认按键的升序排列（可通过自定义比较器修改排序规则）。
   *
   * 模板参数:
   *
   * * - `map_type_k`: 键（key）的类型，用于排序和唯一标识
   *
   * * - `map_type_v`: 值（value）的类型，与键关联的数据
   *
   * * - `comparators`: 键的比较器类型，默认为 `standard_con::imitation_functions::less<map_type_k>`
   *
   *   - 定义键的排序规则，返回 `true` 表示左操作数小于右操作数，决定键的排列顺序
   *
   * 迭代器类型:
   * 继承自底层红黑树的迭代器，支持正向、反向遍历，以及常量版本：
   *
   * * - `iterator`: 正向迭代器，指向键值对
   *
   * * - `const_iterator`: 常量正向迭代器，指向不可修改的键值对
   *
   * * - `reverse_iterator`: 反向迭代器，按逆序遍历键值对
   *
   * * - `const_reverse_iterator`: 常量反向迭代器，按逆序遍历不可修改的键值对
   *
   * * - `map_iterator`: 插入操作返回类型，为 `standard_con::pair<iterator, bool>`，其中 `iterator` 指向插入位置（或已有键的位置），`bool` 表示是否插入成功
   */
    template <typename map_type_k, typename map_type_v, typename comparators = standard_con::less<map_type_k>>
    class tree_map
    {
        using key_val_type = standard_con::pair<map_type_k, map_type_v>;
        struct key_val
        {
            const map_type_k &operator()(const key_val_type &key_value)
            {
                return key_value.first;
            }
        };
        using instance_rb = standard_con::red_black_tree<map_type_k, key_val_type, key_val, comparators>;
        instance_rb instance_tree_map;

    public:
        using iterator = typename instance_rb::iterator;
        using const_iterator = typename instance_rb::const_iterator;
        using reverse_iterator = typename instance_rb::reverse_iterator;
        using const_reverse_iterator = typename instance_rb::const_reverse_iterator;

        using map_iterator = standard_con::pair<iterator, bool>;
        ~tree_map() = default;
        tree_map &operator=(const tree_map &tree_map_data)
        {
            if (this != &tree_map_data)
            {
                instance_tree_map = tree_map_data.instance_tree_map;
            }
            return *this;
        }
        tree_map &operator=(tree_map &&tree_map_data) noexcept
        {
            if (this != &tree_map_data)
            {
                instance_tree_map = std::move(tree_map_data.instance_tree_map);
            }
            return *this;
        }
        map_iterator push(key_val_type &&tree_map_data) noexcept
        {
            return instance_tree_map.push(tree_map_data);
        }
        tree_map &operator=(std::initializer_list<key_val_type> lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_tree_map.push(std::move(chained_values));
            }
            return *this;
        }
        tree_map(const std::initializer_list<key_val_type> &lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_tree_map.push(std::move(chained_values));
            }
        }
        explicit tree_map(key_val_type &&tree_map_data) noexcept
        {
            instance_tree_map.push(std::forward<key_val_type>(tree_map_data));
        }
        tree_map() { ; }

        tree_map(const tree_map &tree_map_data) { instance_tree_map = tree_map_data.instance_tree_map; }

        tree_map(tree_map &&tree_map_data) noexcept { instance_tree_map = std::move(tree_map_data.instance_tree_map); }

        explicit tree_map(const key_val_type &tree_map_data)
        {
            instance_tree_map.push(tree_map_data);
        }

        map_iterator push(const key_val_type &tree_map_data) { return instance_tree_map.push(tree_map_data); }

        map_iterator pop(const key_val_type &tree_map_data) { return instance_tree_map.pop(tree_map_data); }

        iterator find(const key_val_type &tree_map_data) { return instance_tree_map.find(tree_map_data); }

        void middle_order_traversal() { instance_tree_map.middle_order_traversal(); }

        void pre_order_traversal() { instance_tree_map.pre_order_traversal(); }

        [[nodiscard]] uint64_t size() const
        {
            return instance_tree_map.size();
        }

        bool empty() { return instance_tree_map.empty(); }

        iterator begin() { return instance_tree_map.begin(); }

        iterator end() { return instance_tree_map.end(); }

        const_iterator cbegin() { return instance_tree_map.cbegin(); }

        const_iterator cend() { return instance_tree_map.cend(); }

        reverse_iterator rbegin() { return instance_tree_map.rbegin(); }

        reverse_iterator rend() { return instance_tree_map.rend(); }

        const_reverse_iterator crbegin() { return instance_tree_map.crbegin(); }

        const_reverse_iterator crend() { return instance_tree_map.crend(); }

        iterator operator[](const key_val_type &tree_map_data) { return instance_tree_map[tree_map_data]; }
    };
    /**
   * @brief 基于哈希表实现的无序键值对映射容器
   *
   * 该容器是一个无序关联容器，存储键值对（key-value），通过哈希函数实现高效的插入、删除和查找操作。
   *
   * 底层依赖哈希表（hash_table）实现，键具有唯一性，不允许重复键。
   *
   * 哈希函数通过组合键和值的哈希结果减少冲突，平均时间复杂度为 O(1)，最坏情况为 O(n)（哈希冲突严重时）。
   *
   * 模板参数:
   *
   * * - `hash_map_type_key`: 键（key）的类型，用于唯一标识和哈希计算
   *
   * * - `hash_map_type_value`: 值（value）的类型，与键关联的数据
   *
   * * - `first_external_hash_functions`: 键的哈希函数类型，默认为 `standard_con::imitation_functions::hash_imitation_functions`
   *   - 用于计算键的哈希值，影响键在哈希表中的映射位置
   *
   * * - `second_external_hash_functions`: 值的哈希函数类型，默认为 `standard_con::imitation_functions::hash_imitation_functions`
   *   - 用于计算值的哈希值，与键的哈希值组合生成最终哈希结果，减少冲突
   *
   * 迭代器类型:
   * 继承自底层哈希表的迭代器，支持按插入顺序遍历，包含普通和常量版本：
   *
   * * - `iterator`: 正向迭代器，指向键值对，支持按插入顺序遍历
   *
   * * - `const_iterator`: 常量正向迭代器，指向不可修改的键值对
   */
    template <typename hash_map_type_key, typename hash_map_type_value,
                        typename first_external_hash_functions = standard_con::hash_imitation_functions,
                        typename second_external_hash_functions = standard_con::hash_imitation_functions> // 两个对应的hash函数
    class hash_map
    {
        using key_val_type = standard_con::pair<hash_map_type_key, hash_map_type_value>;
        struct key_val
        {
            const hash_map_type_key &operator()(const key_val_type &key_value)
            {
                return key_value.first;
            }
        };
        class inbuilt_map_hash_functor
        {
        private:
            first_external_hash_functions first_hash_functions_object;
            second_external_hash_functions second_hash_functions_object;

        public:
            uint64_t operator()(const key_val_type &key_value) noexcept
            {
                uint64_t first_hash_value = first_hash_functions_object(key_value.first);
                first_hash_value = first_hash_value * 31;
                uint64_t second_hash_value = second_hash_functions_object(key_value.second);
                second_hash_value = second_hash_value * 31;
                return (first_hash_value + second_hash_value);
            }
        };
        using hash_table = standard_con::hash_table<hash_map_type_key, key_val_type, key_val, inbuilt_map_hash_functor>;
        hash_table instance_hash_map;

    public:
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator; // 单向迭代器
        hash_map() { ; }

        ~hash_map() = default;

        explicit hash_map(const key_val_type &key_value) { instance_hash_map.push(key_value); }

        hash_map(const hash_map &hash_map_data) { instance_hash_map = hash_map_data.instance_hash_map; }

        hash_map(hash_map &&hash_map_data) noexcept { instance_hash_map = std::move(hash_map_data.instance_hash_map); }

        bool push(const key_val_type &key_value) { return instance_hash_map.push(key_value); }

        bool pop(const key_val_type &key_value) { return instance_hash_map.pop(key_value); }

        iterator find(const key_val_type &key_value) { return instance_hash_map.find(key_value); }

        uint64_t size() { return instance_hash_map.size(); }

        [[nodiscard]] uint64_t size() const
        {
            return instance_hash_map.size();
        }

        [[nodiscard]] uint64_t capacity() const
        {
            return instance_hash_map.capacity();
        }

        bool empty() { return instance_hash_map.empty(); }

        iterator begin() { return instance_hash_map.begin(); }

        iterator end() { return instance_hash_map.end(); }

        const_iterator cbegin() { return instance_hash_map.cbegin(); }

        const_iterator cend() { return instance_hash_map.cend(); }

        iterator operator[](const key_val_type &key_value) { return instance_hash_map[key_value]; }

        hash_map(const std::initializer_list<key_val_type> &lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_hash_map.push(std::move(chained_values));
            }
        }
        explicit hash_map(key_val_type &&key_value) noexcept
        {
            instance_hash_map.push(std::forward<key_val_type>(key_value));
        }
        hash_map &operator=(const std::initializer_list<key_val_type> &lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_hash_map.push(std::move(chained_values));
            }
            return *this;
        }
        hash_map &operator=(hash_map &&hash_map_data) noexcept
        {
            if (this != &hash_map_data)
            {
                instance_hash_map = std::forward<hash_table>(hash_map_data.instance_hash_map);
            }
            return *this;
        }
        bool push(key_val_type &&key_value) noexcept
        {
            return instance_hash_map.push(std::forward<key_val_type>(key_value));
        }
        hash_map &operator=(const hash_map &hash_map_data)
        {
            if (this != &hash_map_data)
            {
                instance_hash_map = hash_map_data.instance_hash_map;
            }
            return *this;
        }
    };
}
namespace standard_con
{
    using map_container::hash_map;
    using map_container::tree_map;
}