#pragma once
#include "SimulateBase.hpp"
namespace set_container
{
    /**
   * @brief 基于红黑树实现的有序集合容器
   *
   * 该容器是一个有序关联容器，存储唯一的元素（无重复值），并根据元素的大小关系自动排序。
   *
   * 底层依赖红黑树（red_black_tree）实现，保证插入、删除、查找等操作的时间复杂度为 O(log n)。
   *
   * 元素具有唯一性，默认按升序排列（可通过自定义比较器修改排序规则）。
   *
   * 模板参数:
   *
   * * - `set_type`: 集合中元素的类型，既是存储的数据也是排序的键
   *
   * * - `comparators`: 元素的比较器类型，默认为 `standard_con::imitation_functions::less<set_type>`
   *
   *   - 定义元素的排序规则，返回 `true` 表示左操作数小于右操作数，决定元素的排列顺序
   *
   * 迭代器类型:
   * 继承自底层红黑树的迭代器，支持正向、反向遍历，以及常量版本：
   *
   * * - `iterator`: 正向迭代器，指向集合中的元素
   *
   * * - `const_iterator`: 常量正向迭代器，指向不可修改的元素
   *
   * * - `reverse_iterator`: 反向迭代器，按逆序遍历元素
   *
   * * - `const_reverse_iterator`: 常量反向迭代器，按逆序遍历不可修改的元素
   *
   * * - `set_iterator`: 插入操作返回类型，为 `standard_con::pair<iterator, bool>`，其中 `iterator` 指向插入位置（或已有元素），`bool` 表示是否插入成功
   */
    template <typename set_type, typename comparators = standard_con::less<set_type>>
    class tree_set
    {
        using key_val_type = set_type; // comparators 用户自定义比较器，用于比较两个元素的大小，方便存储
        struct key_val
        {
            const set_type &operator()(const key_val_type &key_value)
            {
                return key_value;
            }
        };
        using instance_rb = standard_con::red_black_tree<set_type, key_val_type, key_val, comparators>;
        instance_rb instance_tree_set;

    public:
        using iterator = typename instance_rb::iterator;
        using const_iterator = typename instance_rb::const_iterator;
        using reverse_iterator = typename instance_rb::reverse_iterator;
        using const_reverse_iterator = typename instance_rb::const_reverse_iterator;

        using set_iterator = standard_con::pair<iterator, bool>;
        tree_set &operator=(const tree_set &set_data)
        {
            if (this != &set_data)
            {
                instance_tree_set = set_data.instance_tree_set;
            }
            return *this;
        }
        tree_set &operator=(tree_set &&set_data) noexcept
        {
            if (this != &set_data)
            {
                instance_tree_set = std::move(set_data.instance_tree_set);
            }
            return *this;
        }
        tree_set(std::initializer_list<key_val_type> lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_tree_set.push(std::move(chained_values));
            }
        }
        tree_set &operator=(std::initializer_list<key_val_type> lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_tree_set.push(std::move(chained_values));
            }
            return *this;
        }
        set_iterator push(key_val_type &&set_data) noexcept
        {
            return instance_tree_set.push(std::forward<key_val_type>(set_data));
        }
        explicit tree_set(key_val_type &&set_type_data) noexcept
        {
            instance_tree_set.push(std::forward<key_val_type>(set_type_data));
        }

        tree_set() { ; }

        ~tree_set() = default;

        tree_set(const tree_set &set_data) { instance_tree_set = set_data.instance_tree_set; }

        tree_set(tree_set &&set_data) noexcept { instance_tree_set = std::move(set_data.instance_tree_set); }

        explicit tree_set(const key_val_type &set_type_data) { instance_tree_set.push(set_type_data); }

        set_iterator push(const key_val_type &set_type_data) { return instance_tree_set.push(set_type_data); }

        set_iterator pop(const key_val_type &set_type_data) { return instance_tree_set.pop(set_type_data); }

        iterator find(const key_val_type &set_type_data) { return instance_tree_set.find(set_type_data); }

        void middle_order_traversal() { instance_tree_set.middle_order_traversal(); }

        void pre_order_traversal() { instance_tree_set.pre_order_traversal(); }

        [[nodiscard]] uint64_t size() const
        {
            return instance_tree_set.size();
        }

        bool empty() { return instance_tree_set.empty(); }

        iterator begin() { return instance_tree_set.begin(); }

        iterator end() { return instance_tree_set.end(); }

        const_iterator cbegin() { return instance_tree_set.cbegin(); }

        const_iterator cend() { return instance_tree_set.cend(); }

        reverse_iterator rbegin() { return instance_tree_set.rbegin(); }

        reverse_iterator rend() { return instance_tree_set.rend(); }

        const_reverse_iterator crbegin() { return instance_tree_set.crbegin(); }

        const_reverse_iterator crend() { return instance_tree_set.crend(); }

        iterator operator[](const key_val_type &set_type_data) { return instance_tree_set[set_type_data]; }
    };
    /**
   * @brief 基于哈希表实现的无序集合容器
   *
   * 该容器是一个无序关联容器，存储唯一的元素（无重复值），通过哈希函数实现高效的插入、删除和查找操作。
   *
   * 底层依赖哈希表（hash_table）实现，元素的存储顺序与插入顺序一致（通过哈希表的全局链表维护），
   *
   * 平均时间复杂度为 O(1)，最坏情况为 O(n)（哈希冲突严重时）。
   *
   * 模板参数:
   *
   * * - `set_type_val`: 集合中存储的元素类型，既是数据也是哈希映射的键
   *
   * * - `external_hash_functions`: 外部哈希函数类型，默认为 `standard_con::imitation_functions::hash_imitation_functions`
   *
   *   - 用于计算元素的哈希值，影响元素在哈希表中的映射位置，可自定义以减少冲突
   *
   * 迭代器类型:
   * 继承自底层哈希表的迭代器，支持按插入顺序遍历，包含普通和常量版本：
   *
   * * - `iterator`: 正向迭代器，指向集合中的元素，支持按插入顺序遍历
   *
   * * - `const_iterator`: 常量正向迭代器，指向不可修改的元素
   */
    template <typename set_type_val, typename external_hash_functions = standard_con::hash_imitation_functions>
    class hash_set
    {
        using key_val_type = set_type_val;
        class inbuilt_set_hash_functor
        {
        private:
            external_hash_functions hash_functions_object;

        public:
            uint64_t operator()(const key_val_type &key_value)
            {
                return hash_functions_object(key_value) * 131;
            }
        };
        class key_val
        {
        public:
            const key_val_type &operator()(const key_val_type &key_value)
            {
                return key_value;
            }
        };
        using hash_table = standard_con::hash_table<set_type_val, key_val_type, key_val, inbuilt_set_hash_functor>;
        hash_table instance_hash_set;

    public:
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator;
        hash_set() { ; }

        explicit hash_set(const set_type_val &set_type_data)
        {
            instance_hash_set.push(set_type_data);
        }

        hash_set(const hash_set &hash_set_data) { instance_hash_set = hash_set_data.instance_hash_set; }

        ~hash_set() = default;

        hash_set(hash_set &&hash_set_data) noexcept { instance_hash_set = std::move(hash_set_data.instance_hash_set); }

        bool push(const key_val_type &set_type_data) { return instance_hash_set.push(set_type_data); }

        bool pop(const key_val_type &set_type_data) { return instance_hash_set.pop(set_type_data); }

        iterator find(const key_val_type &set_type_data) { return instance_hash_set.find(set_type_data); }

        uint64_t size() { return instance_hash_set.size(); }

        bool empty() { return instance_hash_set.empty(); }

        uint64_t capacity() { return instance_hash_set.capacity(); }

        [[nodiscard]] uint64_t size() const
        {
            return instance_hash_set.size();
        }

        [[nodiscard]] uint64_t capacity() const
        {
            return instance_hash_set.capacity();
        }

        iterator begin() { return instance_hash_set.begin(); }

        iterator end() { return instance_hash_set.end(); }

        const_iterator cbegin() { return instance_hash_set.cbegin(); }

        const_iterator cend() { return instance_hash_set.cend(); }

        iterator operator[](const key_val_type &set_type_data) { return instance_hash_set[set_type_data]; }

        explicit hash_set(set_type_val &&set_type_data) noexcept
        {
            instance_hash_set.push(std::forward<set_type_val>(set_type_data));
        }
        bool push(set_type_val &&set_type_data) noexcept
        {
            return instance_hash_set.push(std::forward<set_type_val>(set_type_data));
        }
        hash_set(std::initializer_list<key_val_type> lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_hash_set.push(std::move(chained_values));
            }
        }
        hash_set &operator=(const std::initializer_list<key_val_type> &lightweight_container)
        {
            for (auto &chained_values : lightweight_container)
            {
                instance_hash_set.push(std::move(chained_values));
            }
            return *this;
        }
        hash_set &operator=(const hash_set &hash_set_data) = default;
        hash_set &operator=(hash_set &&hash_set_data) noexcept
        {
            instance_hash_set = std::move(hash_set_data.instance_hash_set);
            return *this;
        }
    };
}
namespace standard_con
{
    using set_container::hash_set;
    using set_container::tree_set;
}