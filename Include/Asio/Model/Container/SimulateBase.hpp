#pragma once
#include "SimulateStack.hpp"
#include "SimulateVector.hpp"
#include "SimulateAlgorithm.hpp"
#include "SimulateUtility.hpp"
#include "SimulateException.hpp"
#include "SimulateImitate.hpp"
#include "SimulateHash.hpp"
namespace base_container
{
    /*
            * @brief  #### `red_black_tree` 类模板

            *   - 实现红黑树（自平衡二叉搜索树），通过颜色规则和旋转操作维持平衡性

            *   - 满足五大特性：节点非红即黑、根节点为黑、叶子节点（NIL）为黑、红节点子节点为黑、任意节点到叶子的所有路径含相同黑节点数

            *   - 支持插入、删除、查找、遍历等操作，时间复杂度为 O(log n)

            * 模板参数:

            * * - `rb_tree_type_key`: 键的类型，用于节点排序的依据
            *
            * * - `rb_tree_type_value`: 节点存储的值类型（通常为键值对）
            *
            * * - `container_imitate_function_visit`: 访问器类型，用于从值中提取键（如从键值对中获取键）
            *
            * * - `container_imitate_function`: 比较器类型，默认为 `standard_con::imitation_functions::less<rb_tree_type_key>`
            *    定义键的大小关系，返回 `true` 表示左操作数小于右操作数


            * 迭代器相关方法:

            * * - `begin()`: 返回指向最左节点的迭代器（中序遍历第一个节点）
            *
            * * - `end()`: 返回指向 `nullptr` 的迭代器（遍历结束标记）
            *
            * * - `cbegin()`/`cend()`: 常量版本的起始和结束迭代器
            *
            * * - `rbegin()`: 返回指向最右节点的反向迭代器（中序遍历最后一个节点）
            *
            * * - `rend()`: 返回指向 `nullptr` 的反向迭代器（反向遍历结束标记）
            *
            * * - `crbegin()`/`crend()`: 常量版本的反向起始和结束迭代器

            * 构造函数:

            * * - 默认构造函数: 初始化空树（根节点为 `nullptr`）
            *
            * * - 从值构造: 用指定值初始化根节点，根节点颜色为黑
            *
            * * - 移动构造函数: 接管另一个临时红黑树的资源，原树根节点置空
            *
            * * - 拷贝构造函数: 深拷贝另一个红黑树，复制节点数据、颜色和父子关系

            * 析构函数:

            * * - 调用 `clear(_root)` 释放所有节点资源

            * 主要操作方法:

            * * - `push()`: 插入值节点（支持拷贝和移动语义）
            *    插入后节点默认为红色，通过变色和旋转调整以满足红黑树特性
            *    处理三种插入情况（叔叔节点为红则变色、叔叔为黑则单旋或双旋）
            *    返回 `pair<iterator, bool>`: 迭代器指向新节点（成功）或已有节点（失败），`bool` 表示是否插入成功
            *
            * * - `pop(const rb_tree_type_value& rb_tree_data)`: 删除指定值的节点
            *    处理三种删除情况（左子树空、右子树空、左右子树均非空）
            *    左右子树均非空时，用右子树最左节点（中序后继）替换当前节点，再删除后继节点
            *    删除后通过 `delete_adjust` 调整颜色和旋转，维持红黑树特性
            *    返回 `pair<iterator, bool>`: 迭代器为 `nullptr`，`bool` 表示是否删除成功
            *
            * * - `find(const rb_tree_type_value& val_data)`: 查找指定值的节点，返回迭代器（找到）或 `end()`（未找到）
            *
            * * - `size()`: 返回节点总数（常量和非常量版本）
            *
            * * - `empty()`: 判断树是否为空（根节点为 `nullptr` 则返回 `true`）
            *
            *
            *   - `middle_order_traversal()`: 中序遍历，打印节点数据（结果为有序序列）
            *
            *   - `pre_order_traversal()`: 前序遍历，打印节点数据

            * 运算符重载:

            * * - `operator=`: 拷贝赋值运算符，通过交换资源实现深拷贝
            *
            * * - `operator= (red_black_tree&& rb_tree_data)`: 移动赋值运算符，接管目标树资源，原树根节点置空
            *
            * * - `operator[](const rb_tree_type_value& rb_tree_data)`: 查找指定值的节点，返回迭代器

            * 特性:

            * * - 自平衡性: 通过颜色规则和旋转操作，确保树的高度为 O(log n)，所有操作均为 O(log n) 时间复杂度
            *
            * * - 红黑特性: 严格遵守五大红黑树特性，保证任意路径的黑节点数相同，避免失衡
            *
            * * - 迭代器有效性: 插入和删除操作可能改变结构，但迭代器仍能正确遍历
            *
            * * - 支持移动语义: 减少不必要的拷贝，提高插入和赋值效率

            * 注意事项:

            * * - 键唯一性: 不允许插入重复键，`push` 对重复键返回 `false`
            *
            * * - 颜色调整逻辑: 插入和删除后的调整是核心，需理解四种情况的变色和旋转规则
            *
            * * - 访问器作用: `container_imitate_function_visit` 必须能从值中提取键，否则比较逻辑失效
            *
            * * - 根节点颜色: 根节点始终为黑，插入和旋转后会强制根节点为黑

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename rb_tree_type_key, typename rb_tree_type_value, typename container_imitate_function_visit,
                        typename container_imitate_function = standard_con::less<rb_tree_type_key>>
    class red_black_tree
    {
    private:
        enum class rb_tree_color
        {
            red,
            black,
        };
        class rb_tree_node
        {
        public:
            rb_tree_type_value _data;
            rb_tree_node *_left;
            rb_tree_node *_right;
            rb_tree_node *_parent;
            rb_tree_color _color;
            explicit rb_tree_node(const rb_tree_type_value &val_data = rb_tree_type_value())
                    : _data(val_data), _left(nullptr), _right(nullptr), _parent(nullptr), _color(rb_tree_color::red)
            {
                ;
            }
            explicit rb_tree_node(rb_tree_type_value &&val_data) noexcept
                    : _data(std::move(val_data)), _left(nullptr), _right(nullptr), _parent(nullptr), _color(rb_tree_color::red)
            {
            }
        };
        template <typename T, typename Ref, typename Ptr>
        class rb_tree_iterator
        {
            using self = rb_tree_iterator<T, Ref, Ptr>;
            using iterator_node = rb_tree_node;
            iterator_node *_node_iterator_ptr;

        public:
            using reference = Ref;
            using pointer = Ptr;
            rb_tree_iterator(iterator_node *iterator_ptr_data)
                    : _node_iterator_ptr(iterator_ptr_data)
            {
                ;
            }
            rb_tree_iterator()
            {
                _node_iterator_ptr = nullptr;
            }
            Ref &operator*()
            {
                return _node_iterator_ptr->_data;
            }
            Ptr operator->()
            {
                return &(_node_iterator_ptr->_data);
            }
            self &operator++()
            {
                if (_node_iterator_ptr == nullptr)
                {
                    return *this;
                }
                if (_node_iterator_ptr->_right != nullptr)
                {
                    iterator_node *subtree_node = _node_iterator_ptr->_right;
                    while (subtree_node->_left != nullptr)
                    {
                        subtree_node = subtree_node->_left;
                    }
                    _node_iterator_ptr = subtree_node;
                }
                else
                {
                    // 代表右子树已经走完，需要向上遍历，继续向上找右子树，如果停下来，说明走完整棵树或者是走到根节点
                    iterator_node *last_parent_node = _node_iterator_ptr->_parent;
                    iterator_node *temporary_subtree_node = _node_iterator_ptr;
                    while (last_parent_node != nullptr && temporary_subtree_node == last_parent_node->_right)
                    {
                        temporary_subtree_node = last_parent_node;
                        last_parent_node = last_parent_node->_parent;
                    }
                    _node_iterator_ptr = last_parent_node;
                    // 如果跳出循环，说明走到了根节点，或者找到了右子树
                }
                return *this;
            }
            self operator++(int)
            {
                self previously_iterator = *this;
                ++(*this);
                return previously_iterator;
            }
            self &operator--()
            {
                if (_node_iterator_ptr->_left != nullptr)
                {
                    iterator_node *subtree_node = _node_iterator_ptr->_left;
                    while (subtree_node->_right != nullptr)
                    {
                        subtree_node = subtree_node->_right;
                    }
                    _node_iterator_ptr = subtree_node;
                }
                else
                {
                    iterator_node *last_parent_node = _node_iterator_ptr->_parent;
                    iterator_node *temporary_subtree_node = _node_iterator_ptr;
                    while (last_parent_node != nullptr && temporary_subtree_node == last_parent_node->_left)
                    {
                        temporary_subtree_node = last_parent_node;
                        last_parent_node = last_parent_node->_parent;
                    }
                    _node_iterator_ptr = last_parent_node;
                }
                return *this;
            }
            self operator--(int)
            {
                self previously_iterator = *this;
                --(*this);
                return previously_iterator;
            }
            bool operator==(const self &it_data) const
            {
                return _node_iterator_ptr == it_data._node_iterator_ptr;
            }
            bool operator!=(const self &it_data) const
            {
                return _node_iterator_ptr != it_data._node_iterator_ptr;
            }
        };
        template <typename iterator>
        class rb_tree_reverse_iterator
        {
            using self = rb_tree_reverse_iterator<iterator>;
            using Ref = typename iterator::reference;
            using Ptr = typename iterator::pointer;
            iterator _it;

        public:
            rb_tree_reverse_iterator(iterator it_data)
                    : _it(it_data)
            {
                ;
            }
            Ref &operator*()
            {
                return *_it;
            }
            Ptr operator->()
            {
                return &(*this);
            }
            // 前置自增：对应正向迭代器的自减
            rb_tree_reverse_iterator &operator++()
            {
                --_it;
                return *this;
            }
            rb_tree_reverse_iterator operator++(int)
            {
                auto previously_iterator = *this;
                --_it;
                return previously_iterator;
            }

            // 前置自减：对应正向迭代器的自增
            rb_tree_reverse_iterator &operator--()
            {
                ++_it;
                return *this;
            }
            rb_tree_reverse_iterator operator--(int)
            {
                auto previously_iterator = *this;
                ++_it;
                return previously_iterator;
            }

            // 比较运算符
            bool operator==(const rb_tree_reverse_iterator &other) const
            {
                return _it == other._it;
            }
            bool operator!=(const rb_tree_reverse_iterator &other) const
            {
                return _it != other._it;
            }
        };
        using container_node = rb_tree_node;
        container_node *_root;
        container_imitate_function_visit element;
        container_imitate_function function_policy;
        void left_revolve(container_node *subtree_node)
        {
            try
            {
                if (subtree_node == nullptr)
                {
                    throw custom_exception::fault("左单旋传进来的节点为空！", "left_revolve", __LINE__);
                }
            }
            catch (const custom_exception::fault &exception)
            {
                std::cerr << exception.what() << exception.function_name_get() << exception.line_number_get() << std::endl;
                throw;
            }
            container_node *sub_tree_right_node = subtree_node->_right;
            // container_node* sub_right_left_node = sub_tree_right_node->_left;
            container_node *sub_right_left_node = (sub_tree_right_node->_left) ? sub_tree_right_node->_left : nullptr;
            // 防止空指针解引用
            subtree_node->_right = sub_right_left_node;
            if (sub_right_left_node)
            {
                sub_right_left_node->_parent = subtree_node;
                // 如果Sub_right_left_temp(调整节点的左根节点)不等于空，还需要调整Sub_right_left_temp它的父亲节点
            }
            sub_tree_right_node->_left = subtree_node;
            // 这里先保存一下parent_temp_Node的父亲地址，防止到下面else比较的时候丢失
            container_node *parent_node = subtree_node->_parent;
            subtree_node->_parent = sub_tree_right_node;
            // 更新parent_temp_Node节点指向正确的地址

            if (_root == subtree_node)
            {
                // 如果要调整的节点是根根节点，直接把调整节点赋值给根节点，然后把调整节点的父亲节点置空
                _root = sub_tree_right_node;
                sub_tree_right_node->_parent = nullptr;
            }
            else
            {
                // 调整前parent_temp_Node是这个树的根现在是Sub_right_temp是这个树的根
                if (parent_node->_left == subtree_node)
                {
                    parent_node->_left = sub_tree_right_node;
                }
                else
                {
                    parent_node->_right = sub_tree_right_node;
                }
                sub_tree_right_node->_parent = parent_node;
            }
        }
        void right_revolve(container_node *subtree_node)
        {
            try
            {
                if (subtree_node == nullptr)
                {
                    throw custom_exception::fault("右单旋传进来的节点为空！", "right_revolve", __LINE__);
                }
            }
            catch (const custom_exception::fault &exception)
            {
                std::cerr << exception.what() << exception.function_name_get() << exception.line_number_get() << std::endl;
                throw;
            }
            container_node *sub_tree_left_node = subtree_node->_left;
            container_node *sub_left_right_node = (sub_tree_left_node->_right) ? sub_tree_left_node->_right : nullptr;
            // 防止空指针解引用
            subtree_node->_left = sub_left_right_node;
            if (sub_left_right_node)
            {
                sub_left_right_node->_parent = subtree_node;
            }
            sub_tree_left_node->_right = subtree_node;
            // 保存parent_temp_Node的父亲节点
            container_node *parent_node = subtree_node->_parent;
            subtree_node->_parent = sub_tree_left_node;

            if (_root == subtree_node)
            {
                _root = sub_tree_left_node;
                sub_tree_left_node->_parent = nullptr;
            }
            else
            {
                if (parent_node->_left == subtree_node)
                {
                    parent_node->_left = sub_tree_left_node;
                }
                else
                {
                    parent_node->_right = sub_tree_left_node;
                }
                sub_tree_left_node->_parent = parent_node;
            }
        }
        void clear(container_node *clear_node_ptr) noexcept
        {
            if (clear_node_ptr == nullptr)
            {
                return;
            }
            else
            {
                standard_con::stack<container_node *> resource_cleanup_stack;
                resource_cleanup_stack.push(clear_node_ptr);
                while (!resource_cleanup_stack.empty())
                {
                    clear_node_ptr = resource_cleanup_stack.top();
                    resource_cleanup_stack.pop();
                    if (clear_node_ptr->_right != nullptr)
                    {
                        resource_cleanup_stack.push(clear_node_ptr->_right);
                    }
                    if (clear_node_ptr->_left != nullptr)
                    {
                        resource_cleanup_stack.push(clear_node_ptr->_left);
                    }
                    delete clear_node_ptr;
                }
                _root = nullptr;
            }
        }
        void interior_middle_order_traversal(container_node *intermediate_traversal_node)
        {
            // 中序遍历函数
            if (intermediate_traversal_node == nullptr)
            {
                return;
            }
            standard_con::stack<container_node *> interior_stack;
            while (intermediate_traversal_node != nullptr || !interior_stack.empty())
            {
                while (intermediate_traversal_node != nullptr)
                {
                    interior_stack.push(intermediate_traversal_node);
                    intermediate_traversal_node = intermediate_traversal_node->_left;
                }
                intermediate_traversal_node = interior_stack.top();
                interior_stack.pop();
                std::cout << intermediate_traversal_node->_data << " ";
                // std::cout << intermediate_traversal_node->_color <<" ";
                intermediate_traversal_node = intermediate_traversal_node->_right;
            }
        }
        void interior_pre_order_traversal(container_node *preamble_traversal_node)
        {
            // 前序遍历，最外左子树全部压栈
            if (preamble_traversal_node == nullptr)
            {
                return;
            }
            container_node *reference_node = preamble_traversal_node;
            standard_con::stack<container_node *> interior_stack;
            interior_stack.push(reference_node);
            while (!interior_stack.empty())
            {
                reference_node = interior_stack.top();
                interior_stack.pop();

                std::cout << reference_node->_data << " ";
                // std::cout << reference_node->_color <<" ";
                // 修改逻辑错误，先压右子树再压左子树，因为这是栈
                if (reference_node->_right != nullptr)
                {
                    interior_stack.push(reference_node->_right);
                }
                if (reference_node->_left != nullptr)
                {
                    interior_stack.push(reference_node->_left);
                }
            }
        }
        static inline rb_tree_color get_color(container_node *current_node)
        {
            return current_node == nullptr ? rb_tree_color::black : current_node->_color;
        }
        static inline bool red_get(container_node *current_node)
        {
            return get_color(current_node) == rb_tree_color::red;
        }
        static inline bool black_get(container_node *current_node)
        {
            return get_color(current_node) == rb_tree_color::black;
        }
        [[nodiscard]] uint64_t _size() const
        {
            uint64_t size = 0;
            if (_root == nullptr)
            {
                return size;
            }
            container_node *reference_node = _root;
            standard_con::stack<container_node *> interior_stack;
            interior_stack.push(reference_node);
            while (!interior_stack.empty())
            {
                reference_node = interior_stack.top();
                interior_stack.pop();

                size++;
                if (reference_node->_right != nullptr)
                {
                    interior_stack.push(reference_node->_right);
                }
                if (reference_node->_left != nullptr)
                {
                    interior_stack.push(reference_node->_left);
                }
            }
            return size;
        }

    public:
        using iterator = rb_tree_iterator<rb_tree_type_value, rb_tree_type_value &, rb_tree_type_value *>;
        using const_iterator = rb_tree_iterator<rb_tree_type_value const, rb_tree_type_value const &, rb_tree_type_value const *>;

        using reverse_iterator = rb_tree_reverse_iterator<iterator>;
        using const_reverse_iterator = rb_tree_reverse_iterator<const_iterator>;

        using return_pair_value = standard_con::pair<iterator, bool>;
        red_black_tree()
        {
            _root = nullptr;
        }
        explicit red_black_tree(const rb_tree_type_value &rb_tree_data)
        {
            _root = new container_node(rb_tree_data);
            _root->_color = rb_tree_color::black;
        }
        explicit red_black_tree(rb_tree_type_value &&rb_tree_data) noexcept
        {
            _root = new container_node(std::forward<rb_tree_type_value>(rb_tree_data));
            _root->_color = rb_tree_color::black;
        }
        red_black_tree(red_black_tree &&rb_tree_data) noexcept
                : element(rb_tree_data.element), function_policy(rb_tree_data.function_policy)
        {
            _root = std::move(rb_tree_data._root);
            rb_tree_data._root = nullptr;
        }
        red_black_tree(const red_black_tree &rb_tree_data)
                : _root(nullptr), element(rb_tree_data.element), function_policy(rb_tree_data.function_policy)
        {
            if (rb_tree_data._root == nullptr)
            {
                _root = nullptr;
            }
            else
            {
                // 使用单栈，存储源节点和目标父节点（均为一级指针）
                standard_con::stack<standard_con::pair<container_node *, container_node *>> stack;

                // 创建根节点
                _root = new container_node(rb_tree_data._root->_data);
                _root->_color = rb_tree_data._root->_color;
                _root->_parent = nullptr; // 根节点的父节点为nullptr

                // 初始化栈，将根节点的子节点压入（注意：这里父节点是 _ROOT，一级指针）
                if (rb_tree_data._root->_right != nullptr)
                {
                    stack.push(standard_con::pair<container_node *, container_node *>(rb_tree_data._root->_right, _root));
                }
                if (rb_tree_data._root->_left != nullptr)
                {
                    stack.push(standard_con::pair<container_node *, container_node *>(rb_tree_data._root->_left, _root));
                }

                // 遍历并复制剩余节点
                while (!stack.empty())
                {
                    auto [first_node, parent_node] = stack.top();
                    stack.pop();

                    // 创建新节点并复制数据
                    auto *new_structure_node = new container_node(first_node->_data);
                    new_structure_node->_color = first_node->_color;

                    // 设置父节点关系（注意：parent_node 是一级指针）
                    new_structure_node->_parent = parent_node;

                    // 判断源节点在原树中是左子还是右子
                    bool isleft_child = false;
                    if (first_node->_parent != nullptr)
                    {
                        isleft_child = (first_node->_parent->_left == first_node);
                    }

                    // 将新节点链接到父节点的正确位置（注意：直接使用 parent_node）
                    if (isleft_child)
                    {
                        parent_node->_left = new_structure_node;
                    }
                    else
                    {
                        parent_node->_right = new_structure_node;
                    }

                    // 处理子节点（注意：压栈时父节点是 new_node，一级指针）
                    if (first_node->_right != nullptr)
                    {
                        stack.push(standard_con::pair<container_node *, container_node *>(first_node->_right, new_structure_node));
                    }
                    if (first_node->_left != nullptr)
                    {
                        stack.push(standard_con::pair<container_node *, container_node *>(first_node->_left, new_structure_node));
                    }
                }
            }
        }
        red_black_tree &operator=(const red_black_tree rb_tree_data)
        {
            if (this == &rb_tree_data)
            {
                return *this;
            }
            else
            {
                clear(_root);
                standard_con::algorithm::swap(rb_tree_data._root, _root);
                standard_con::algorithm::swap(rb_tree_data.element, element);
                standard_con::algorithm::swap(rb_tree_data.function_policy, function_policy);
                return *this;
            }
        }
        red_black_tree &operator=(red_black_tree &&rb_tree_data) noexcept
        {
            if (this != &rb_tree_data)
            {
                clear();
                function_policy = std::move(rb_tree_data.function_policy);
                element = std::move(rb_tree_data.element);
                _root = std::move(rb_tree_data._root);
                rb_tree_data._root = nullptr;
            }
            return *this;
        }
        ~red_black_tree() noexcept
        {
            clear(_root);
        }
        return_pair_value push(const rb_tree_type_value &value_data)
        {
            if (_root == nullptr)
            {
                _root = new container_node(value_data);
                _root->_color = rb_tree_color::black;
                return return_pair_value(iterator(_root), true);
            }
            else
            {
                container_node *reference_node = _root;
                container_node *parent_node = nullptr;
                while (reference_node != nullptr)
                {
                    parent_node = reference_node;
                    if (!function_policy(element(reference_node->_data), element(value_data)) &&
                            !function_policy(element(value_data), element(reference_node->_data)))
                    {
                        // 插入失败，找到相同的值，开始返回
                        return return_pair_value(iterator(reference_node), false);
                    }
                    else if (function_policy(element(reference_node->_data), element(value_data)))
                    {
                        reference_node = reference_node->_right;
                    }
                    else
                    {
                        reference_node = reference_node->_left;
                    }
                }
                // 找到插入位置
                reference_node = new container_node(value_data);
                if (function_policy(element(parent_node->_data), element(reference_node->_data)))
                {
                    parent_node->_right = reference_node;
                }
                else
                {
                    parent_node->_left = reference_node;
                }
                reference_node->_color = rb_tree_color::red;
                reference_node->_parent = parent_node;
                container_node *return_push_node = reference_node;
                // 保存节点
                // 开始调整，向上调整颜色节点
                while (parent_node != nullptr && parent_node->_color == rb_tree_color::red)
                {
                    container_node *grandfther_node = parent_node->_parent;
                    if (grandfther_node->_left == parent_node)
                    {
                        // 叔叔节点
                        //  std::cout << "push" <<" ";
                        container_node *uncle_node = grandfther_node->_right;
                        // 情况1：uncle存在，且为红
                        // 情况2: uncle不存在，那么_ROOT_Temp就是新增节点
                        // 情况3：uncle存在且为黑，说明_ROOT_Temp不是新增节点
                        if (uncle_node && uncle_node->_color == rb_tree_color::red)
                        {
                            // 情况1：
                            parent_node->_color = uncle_node->_color = rb_tree_color::black;
                            grandfther_node->_color = rb_tree_color::red;
                            // 颜色反转完成
                            reference_node = grandfther_node;
                            parent_node = reference_node->_parent;
                            // 向上调整,继续从红色节点开始
                        }
                        else
                        {
                            // 情况3：该情况双旋转单旋
                            if (reference_node == parent_node->_right)
                            {
                                left_revolve(parent_node);
                                standard_con::algorithm::swap(reference_node, parent_node);
                                // reference_node = parent_node;
                                // 折线调整，交换位置调整为情况2
                            }
                            // 情况2：直接单旋
                            right_revolve(grandfther_node);
                            grandfther_node->_color = rb_tree_color::red;
                            parent_node->_color = rb_tree_color::black;
                        }
                    }
                    else
                    {
                        container_node *uncle_node = grandfther_node->_left;
                        // 与上面相反
                        if (uncle_node && uncle_node->_color == rb_tree_color::red)
                        {
                            // 情况1：
                            parent_node->_color = uncle_node->_color = rb_tree_color::black;
                            grandfther_node->_color = rb_tree_color::red;
                            // 颜色反转完成
                            reference_node = grandfther_node;
                            parent_node = reference_node->_parent;
                        }
                        else
                        {
                            // 情况3：该情况双旋转单旋
                            if (reference_node == parent_node->_left)
                            {
                                right_revolve(parent_node);
                                standard_con::algorithm::swap(reference_node, parent_node);
                                // reference_node = parent_node;
                                // 交换指针转换为单旋
                            }
                            // 情况2：单旋
                            left_revolve(grandfther_node);
                            grandfther_node->_color = rb_tree_color::red;
                            parent_node->_color = rb_tree_color::black;
                        }
                    }
                }
                _root->_color = rb_tree_color::black;
                return return_pair_value(iterator(return_push_node), true);
            }
        }
        return_pair_value push(rb_tree_type_value &&value_data) noexcept
        {
            if (_root == nullptr)
            {
                _root = new container_node(std::forward<rb_tree_type_value>(value_data));
                _root->_color = rb_tree_color::black;
                return return_pair_value(iterator(_root), true);
            }
            else
            {
                container_node *reference_node = _root;
                container_node *parent_node = nullptr;
                while (reference_node != nullptr)
                {
                    parent_node = reference_node;
                    if (!function_policy(element(reference_node->_data), element(value_data)) &&
                            !function_policy(element(value_data), element(reference_node->_data)))
                    {
                        // 插入失败，找到相同的值，开始返回
                        return return_pair_value(iterator(reference_node), false);
                    }
                    else if (function_policy(element(reference_node->_data), element(value_data)))
                    {
                        reference_node = reference_node->_right;
                    }
                    else
                    {
                        reference_node = reference_node->_left;
                    }
                }
                // 找到插入位置
                reference_node = new container_node(std::forward<rb_tree_type_value>(value_data));
                if (function_policy(element(parent_node->_data), element(reference_node->_data)))
                {
                    parent_node->_right = reference_node;
                }
                else
                {
                    parent_node->_left = reference_node;
                }
                reference_node->_color = rb_tree_color::red;
                reference_node->_parent = parent_node;
                container_node *return_push_node = reference_node;
                // 保存节点
                // 开始调整，向上调整颜色节点
                while (parent_node != nullptr && parent_node->_color == rb_tree_color::red)
                {
                    container_node *grandfther_node = parent_node->_parent;
                    if (grandfther_node->_left == parent_node)
                    {
                        // 叔叔节点
                        container_node *uncle_node = grandfther_node->_right;
                        // 情况1：uncle存在，且为红
                        // 情况2: uncle不存在，那么_ROOT_Temp就是新增节点
                        // 情况3：uncle存在且为黑，说明_ROOT_Temp不是新增节点
                        if (uncle_node && uncle_node->_color == rb_tree_color::red)
                        {
                            // 情况1：
                            parent_node->_color = uncle_node->_color = rb_tree_color::black;
                            grandfther_node->_color = rb_tree_color::red;
                            // 颜色反转完成
                            reference_node = grandfther_node;
                            parent_node = reference_node->_parent;
                            // 向上调整,继续从红色节点开始
                        }
                        else
                        {
                            // 情况3：该情况双旋转单旋
                            if (reference_node == parent_node->_right)
                            {
                                left_revolve(parent_node);
                                standard_con::algorithm::swap(reference_node, parent_node);
                                // reference_node = parent_node;
                                // 折线调整，交换位置调整为情况2
                            }
                            // 情况2：直接单旋
                            right_revolve(grandfther_node);
                            grandfther_node->_color = rb_tree_color::red;
                            parent_node->_color = rb_tree_color::black;
                        }
                    }
                    else
                    {
                        container_node *uncle_node = grandfther_node->_left;
                        // 与上面相反
                        if (uncle_node && uncle_node->_color == rb_tree_color::red)
                        {
                            // 情况1：
                            parent_node->_color = uncle_node->_color = rb_tree_color::black;
                            grandfther_node->_color = rb_tree_color::red;
                            // 颜色反转完成
                            reference_node = grandfther_node;
                            parent_node = reference_node->_parent;
                        }
                        else
                        {
                            // 情况3：该情况双旋转单旋
                            if (reference_node == parent_node->_left)
                            {
                                right_revolve(parent_node);
                                standard_con::algorithm::swap(reference_node, parent_node);
                                // reference_node = parent_node;
                                // 交换指针转换为单旋
                            }
                            // 情况2：单旋
                            left_revolve(grandfther_node);
                            grandfther_node->_color = rb_tree_color::red;
                            parent_node->_color = rb_tree_color::black;
                        }
                    }
                }
                _root->_color = rb_tree_color::black;
                return return_pair_value(iterator(return_push_node), true);
            }
        }
        /*
        删除节点后，调整红黑树颜色，分左右子树来调整，每颗子树分为4种情况
        情况 1：兄弟节点为红色
                        将兄弟节点设为黑色。
                        将父节点设为红色。
                        对父节点进行旋转（左子树删除则左旋，右子树删除则右旋）。
                        更新兄弟节点为新的兄弟（旋转后父节点的新子节点）。

        情况 2：兄弟节点为黑色，且兄弟的两个子节点都是黑色
                        将兄弟节点设为红色。
                        当前节点（cur）上移至父节点（parent）。
                        若上移后的节点是红色，将其设为黑色并结束调整；否则继续循环。
        情况 3：兄弟节点为黑色，兄弟的内侧子节点为红色，外侧子节点为黑色
                        将兄弟节点的内侧子节点设为黑色。
                        将兄弟节点设为红色。
                        对兄弟节点进行反向旋转（左子树删除则右旋，右子树删除则左旋）。
                        更新兄弟节点为新的兄弟（旋转后父节点的子节点）。
        情况 4：兄弟节点为黑色，且兄弟的外侧子节点为红色，内侧子节点为任意颜色
                        将兄弟节点的颜色设为父节点的颜色。
                        将父节点设为黑色。
                        将兄弟的外侧子节点设为黑色。
                        对父节点进行旋转（左子树删除则右旋，右子树删除则左旋）。
                        结束调整。
        */
        void delete_adjust(container_node *current_node, container_node *parent)
        {
            // cur为被删节点的替代节点
            if (current_node == nullptr && parent == nullptr)
            {
                return;
            }
            while (current_node != _root && (current_node == nullptr || black_get(current_node)))
            {
                if (current_node == _root)
                {
                    break;
                }
                if (parent->_left == current_node)
                {
                    container_node *brother = parent->_right;
                    if (red_get(brother))
                    {
                        // 情况1：兄弟节点为红
                        brother->_color = rb_tree_color::black;
                        parent->_color = rb_tree_color::red;
                        left_revolve(parent);
                        // 调整后，兄弟节点为黑
                        // 继续向下调整
                        brother = parent->_right;
                    }
                    if ((brother != nullptr && black_get(brother)) && (brother->_left == nullptr || (brother->_left)) &&
                            (brother->_right == nullptr || black_get(brother->_right)))
                    {
                        // 情况2：兄弟节点为黑，且兄弟节点两个子节点都为黑
                        brother->_color = rb_tree_color::red;
                        current_node = parent;
                        parent = current_node->_parent;
                        if (current_node->_color == rb_tree_color::red)
                        {
                            current_node->_color = rb_tree_color::black;
                            break;
                        }
                    }
                    else if ((brother != nullptr && black_get(brother)) && (brother->_right == nullptr || black_get(brother->_right)) &&
                   (brother->_left != nullptr && red_get(brother->_left)))
                    {
                        // 情况3：兄弟节点为黑，兄弟节点左节点为红，右节点为黑
                        brother->_left->_color = rb_tree_color::black;
                        brother->_color = rb_tree_color::red;
                        right_revolve(brother);
                        // 调整后，兄弟节点为黑，兄弟节点右节点为红
                        // 继续向下调整
                        brother = parent->_right;
                    }
                    else if ((brother != nullptr || black_get(brother)) && (brother->_right != nullptr && red_get(brother->_right)))
                    {
                        // 情况4：兄弟节点为黑，兄弟节点右节点为红
                        brother->_color = parent->_color;
                        parent->_color = rb_tree_color::black;
                        brother->_right->_color = rb_tree_color::black;
                        left_revolve(parent);
                        current_node = _root;
                        parent = current_node->_parent;
                    }
                }
                else
                {
                    container_node *brother = parent->_left;
                    if (red_get(brother))
                    {
                        // 情况1：兄弟节点为红
                        brother->_color = rb_tree_color::black;
                        parent->_color = rb_tree_color::red;
                        right_revolve(parent);
                        // 调整后，兄弟节点为黑
                        brother = parent->_left;
                    }
                    if (brother != nullptr && black_get(brother) && (brother->_left == nullptr || black_get(brother->_left)) &&
                            (brother->_right == nullptr || black_get(brother->_right)))
                    {
                        // 情况2：兄弟节点为黑，且兄弟节点两个子节点都为黑
                        brother->_color = rb_tree_color::red;
                        current_node = parent;
                        parent = current_node->_parent;
                        if (current_node->_color == rb_tree_color::red)
                        {
                            current_node->_color = rb_tree_color::black;
                            break;
                        }
                    }
                    else if (brother != nullptr && black_get(brother) && (brother->_right != nullptr && red_get(brother->_right)) &&
                   (brother->_left == nullptr || black_get(brother->_left)))
                    {
                        // 情况3：兄弟节点为黑，兄弟节点左节点为红，右节点为黑
                        brother->_right->_color = rb_tree_color::black;
                        brother->_color = rb_tree_color::red;
                        left_revolve(brother);
                        // 调整后，兄弟节点为黑，兄弟节点右节点为红
                        // 继续向下调整
                        brother = parent->_left;
                    }
                    else if (brother != nullptr && black_get(brother) && brother->_left != nullptr && red_get(brother->_left))
                    {
                        // 情况4：兄弟节点为黑，兄弟节点右节点为红
                        brother->_color = parent->_color;
                        parent->_color = rb_tree_color::black;
                        brother->_left->_color = rb_tree_color::black;
                        right_revolve(parent);
                        current_node = _root;
                        parent = current_node->_parent;
                    }
                }
            }
            if (current_node != nullptr)
            {
                current_node->_color = rb_tree_color::black;
            }
        }
        return_pair_value pop(const rb_tree_type_value &rb_tree_data)
        {
            if (_root == nullptr)
            {
                return return_pair_value(iterator(nullptr), false);
            }
            else
            {
                container_node *reference_node = _root;
                container_node *parent_node = nullptr;
                container_node *adjust_node = nullptr;
                container_node *adjust_parent_node = nullptr;
                while (reference_node != nullptr)
                {
                    if (!function_policy(element(reference_node->_data), element(rb_tree_data)) && !function_policy(element(rb_tree_data), element(reference_node->_data)))
                    {
                        break;
                    }
                    // 防止父亲自赋值
                    parent_node = reference_node;
                    if (function_policy(element(reference_node->_data), element(rb_tree_data)))
                    {
                        reference_node = reference_node->_right;
                    }
                    else
                    {
                        reference_node = reference_node->_left;
                    }
                }
                if (reference_node == nullptr)
                {
                    return return_pair_value(iterator(nullptr), false);
                }
                // 找到位置开始删除
                rb_tree_color delete_color = reference_node->_color;
                if (reference_node->_left == nullptr)
                {
                    if (reference_node->_right != nullptr)
                    {
                        // 右节点有值
                        reference_node->_right->_parent = parent_node;
                    }
                    if (parent_node == nullptr)
                    {
                        // 根节点
                        _root = reference_node->_right;
                    }
                    else
                    {
                        // 不为空，代表要删除的数不是在根节点上
                        if (parent_node->_left == reference_node)
                        {
                            parent_node->_left = reference_node->_right;
                        }
                        else
                        {
                            parent_node->_right = reference_node->_right;
                        }
                    }
                    adjust_node = reference_node->_right;
                    adjust_parent_node = parent_node;
                    delete reference_node;
                    reference_node = nullptr;
                }
                else if (reference_node->_right == nullptr)
                {
                    if (reference_node->_left != nullptr)
                    {
                        reference_node->_left->_parent = parent_node;
                        // 链接父节点
                    }
                    if (parent_node == nullptr)
                    {
                        // 与上同理
                        _root = reference_node->_left;
                    }
                    else
                    {
                        if (parent_node->_left == reference_node)
                        {
                            parent_node->_left = reference_node->_left;
                        }
                        else
                        {
                            parent_node->_right = reference_node->_left;
                        }
                    }
                    adjust_node = reference_node->_left;
                    adjust_parent_node = parent_node;
                    delete reference_node;
                    reference_node = nullptr;
                }
                else if (reference_node->_right != nullptr && reference_node->_left != nullptr)
                {
                    container_node *right_subtree_smallest_node = reference_node->_right;
                    container_node *smallest_parent_node = reference_node;
                    while (right_subtree_smallest_node->_left != nullptr)
                    {
                        smallest_parent_node = right_subtree_smallest_node;
                        right_subtree_smallest_node = right_subtree_smallest_node->_left;
                    }
                    delete_color = right_subtree_smallest_node->_color;

                    // 交换数据 AND 交换颜色
                    standard_con::algorithm::swap(right_subtree_smallest_node->_data, reference_node->_data);
                    standard_con::algorithm::swap(right_subtree_smallest_node->_color, reference_node->_color);

                    // 然后正确地把后继节点的位置接到它父节点上：
                    if (smallest_parent_node->_left == right_subtree_smallest_node)
                    {
                        smallest_parent_node->_left = right_subtree_smallest_node->_right;
                    }
                    else
                    {
                        smallest_parent_node->_right = right_subtree_smallest_node->_right;
                    }
                    if (right_subtree_smallest_node->_right)
                    {
                        right_subtree_smallest_node->_right->_parent = smallest_parent_node;
                    }
                    adjust_node = right_subtree_smallest_node->_right;
                    adjust_parent_node = smallest_parent_node;

                    // 最后再 delete 那个后继节点
                    delete right_subtree_smallest_node;
                    right_subtree_smallest_node = nullptr;
                }
                // 更新颜色
                if (delete_color == rb_tree_color::black)
                {
                    // 删除红色节点不影响性质
                    delete_adjust(adjust_node, adjust_parent_node);
                }
                if (_root != nullptr)
                {
                    _root->_color = rb_tree_color::black;
                }
                return return_pair_value(iterator(nullptr), false);
            }
        }
        iterator find(const rb_tree_type_value &val_data)
        {
            if (_root == nullptr)
            {
                return iterator(nullptr);
            }
            else
            {
                container_node *root_find_node = _root;
                while (root_find_node != nullptr)
                {
                    if (!function_policy(element(root_find_node->_data), element(val_data)))
                    {
                        return iterator(root_find_node);
                    }
                    else if (function_policy(element(root_find_node->_data), element(val_data)))
                    {
                        root_find_node = root_find_node->_right;
                    }
                    else
                    {
                        root_find_node = root_find_node->_left;
                    }
                }
                return iterator(nullptr);
            }
        }
        uint64_t size()
        {
            return _size();
        }
        [[nodiscard]] uint64_t size() const
        {
            return _size();
        }
        bool empty()
        {
            return _root == nullptr;
        }
        void middle_order_traversal()
        {
            interior_middle_order_traversal(_root);
        }
        void pre_order_traversal()
        {
            interior_pre_order_traversal(_root);
        }
        iterator begin()
        {
            container_node *root_node = _root;
            while (root_node != nullptr && root_node->_left != nullptr)
            {
                root_node = root_node->_left;
            }
            return iterator(root_node);
        }

        static iterator end()
        {
            return iterator(nullptr);
        }
        const_iterator cbegin() const
        {
            container_node *croot_node = _root;
            while (croot_node != nullptr && croot_node->_left != nullptr)
            {
                croot_node = croot_node->_left;
            }
            return const_iterator(croot_node);
        }

        static const_iterator cend()
        {
            return const_iterator(nullptr);
        }
        reverse_iterator rbegin()
        {
            container_node *iterator_node = _root;
            while (iterator_node != nullptr && iterator_node->_right != nullptr)
            {
                iterator_node = iterator_node->_right;
            }
            return reverse_iterator(iterator_node);
        }

        static reverse_iterator rend()
        {
            return reverse_iterator(nullptr);
        }
        const_reverse_iterator crbegin() const
        {
            container_node *iterator_node = _root;
            while (iterator_node != nullptr && iterator_node->_right != nullptr)
            {
                iterator_node = iterator_node->_right;
            }
            return const_reverse_iterator(iterator_node);
        }

        static const_reverse_iterator crend()
        {
            return const_reverse_iterator(nullptr);
        }
        iterator operator[](const rb_tree_type_value &rb_tree_data)
        {
            return find(rb_tree_data);
        }
    };
    /*
            * @brief  #### `hash_table` 类模板

            *   - 自定义哈希表容器，采用链地址法解决哈希冲突

            *   - 支持按插入顺序遍历（通过全局双向链表维护），自动扩容机制，基于负载因子动态调整容量

            *   - 提供插入、删除、查找等基本操作，时间复杂度平均为 O(1)

            * 模板参数:

            * * - `hash_table_type_key`: 键的类型，用于哈希映射和比较的依据
            *
            * * - `hash_table_type_value`: 哈希表中存储的元素类型（通常为键值对）
            *
            * * - `container_imitate_function`: 仿函数类型，用于从值中提取键（如从键值对中获取键用于比较）
            *
            * * - `hash_function`: 哈希函数类型，默认为 `scl::imitation_functions::hash_imitation_functions`
            *   用于计算值的哈希值，决定元素在哈希桶中的位置

            * 迭代器相关方法:

            * * - `begin()`: 返回指向全局链表头节点的迭代器（按插入顺序的第一个元素）
            *
            * * - `cbegin()`: 返回指向全局链表头节点的常量迭代器
            *
            * * - `end()`: 返回指向 `nullptr` 的迭代器（遍历结束标记）
            *
            * * - `cend()`: 返回指向 `nullptr` 的常量迭代器

            * 构造函数:

            * * - 默认构造函数: 初始化空哈希表（容量10，负载因子7，大小0）
            *
            * * - 带容量构造: 用指定容量初始化哈希表
            *
            * * - 拷贝构造函数: 深拷贝另一个哈希表，复制所有节点数据、哈希桶结构和全局链表关系
            *
            * * - 移动构造函数: 接管另一个临时哈希表的资源（哈希桶、全局链表指针等），原表资源置空

            * 析构函数:

            * * - 遍历所有哈希桶，释放每个节点的内存，清空哈希表

            * 主要操作方法:

            * * - `push()`: 插入元素（支持拷贝和移动语义）

            *     返回 `true` 表示插入成功
            *
            * * - `pop(const hash_table_type_value& hash_table_value_data)`: 删除指定元素
            *
            *     释放节点内存，更新大小，返回 `true` 表示删除成功；未找到则返回 `false`
            *
            * * - `find(const hash_table_type_value& hash_table_value_data)`: 查找指定元素

            *    返回指向元素的迭代器（找到）或 `end()`（未找到）
            *
            * * - `operator[](const hash_table_type_key& key_value)`: 通过键查找元素
            *     计算键的哈希值找到对应桶，遍历桶内链表比较键，返回指向元素的迭代器（找到）或 `end()`（未找到）
            *
            *   - `size()`: 返回元素数量（常量和非常量版本）
            *
            *   - `empty()`: 判断哈希表是否为空（`_size == 0`）
            *
            *   - `capacity()`: 返回当前容量（常量和非常量版本）
            *
            *   - `change_load_factor(const uint64_t& new_load_factor)`: 修改负载因子（新值需 ≥1，返回修改是否成功）

            * 特性:

            * * - 链地址法: 通过链表处理哈希冲突，每个桶存储哈希值相同的元素
            *
            * * - 按插入顺序遍历: 通过全局双向链表维护插入顺序，迭代器按此顺序遍历
            *
            * * - 自动扩容: 基于负载因子动态调整容量（翻倍），避免哈希冲突过于频繁
            *
            * * - 支持移动语义: 减少插入和赋值时的拷贝开销，提高性能
            *
            * * - 唯一性: 不允许插入重复元素（通过仿函数比较键判断）

            * 注意事项:

            * * - 负载因子: 默认值为7（即负载率70%），可通过 `change_load_factor` 调整，值越小冲突越少但空间开销越大
            *
            * * - 扩容机制: 扩容时会重新哈希所有元素，可能导致迭代器失效
            *
            * * - 全局链表: 用于维持插入顺序，迭代器遍历依赖此链表，删除元素时需同步维护链表指针
            *
            * * - 仿函数要求: `container_imitate_function` 需能从值中提取键（如 `std::get<0>` 用于键值对），否则比较逻辑失效

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename hash_table_type_key, typename hash_table_type_value, typename container_imitate_function,
                        typename hash_function = standard_con::hash_imitation_functions>
    class hash_table
    {
        class hash_table_node
        {
        public:
            hash_table_type_value _data;
            hash_table_node *_next;
            hash_table_node *overall_list_prev;
            // 全局链表指针，方便按照插入的顺序有序遍历哈希表
            hash_table_node *overall_list_next;
            explicit hash_table_node(const hash_table_type_value &hash_table_value_data)
            {
                _data = hash_table_value_data;
                _next = nullptr;
                overall_list_prev = nullptr;
                overall_list_next = nullptr;
            }
            explicit hash_table_node(hash_table_type_value &&hash_table_value_data)
            {
                _data = std::move(hash_table_value_data);
                _next = nullptr;
                overall_list_prev = nullptr;
                overall_list_next = nullptr;
            }
        };
        using container_node = hash_table_node;
        container_imitate_function value_imitation_functions; // 仿函数

        uint64_t _size; // 哈希表大小

        uint64_t load_factor; // 负载因子

        uint64_t hash_capacity; // 哈希表容量

        standard_con::vector<container_node *> vector_hash_table; // 哈希表

        hash_function hash_function_object; // 哈希函数

        container_node *overall_list_before_node = nullptr; // 前一个数据

        container_node *overall_list_head_node = nullptr; // 全局头数据

        template <typename iterator_type_key, typename iterator_type_val>
        class hash_iterator
        {
            using iterator_node = container_node;
            using Ref = iterator_type_val &;
            using Ptr = iterator_type_val *;
            using self = hash_iterator<iterator_type_key, iterator_type_val>;
            iterator_node *hash_table_iterator_node;

        public:
            hash_iterator(iterator_node *iterator_ptr_node) { hash_table_iterator_node = iterator_ptr_node; }

            Ref operator*() { return hash_table_iterator_node->_data; }

            Ptr operator->() { return &hash_table_iterator_node->_data; }

            bool operator!=(const self &iterator_data) { return hash_table_iterator_node != iterator_data.hash_table_iterator_node; }

            bool operator==(const self &iterator_data) { return hash_table_iterator_node == iterator_data.hash_table_iterator_node; }

            self operator++(int)
            {
                self iterator_data = *this;
                hash_table_iterator_node = hash_table_iterator_node->overall_list_next;
                return iterator_data;
            }
            self operator++()
            {
                hash_table_iterator_node = hash_table_iterator_node->overall_list_next;
                return *this;
            }
            iterator_node *get_node()
            {
                return hash_table_iterator_node;
            }
        };
        void hash_chain_adjustment(container_node *&provisional_parent_node, container_node *&provisional_node, uint64_t &hash_map_location)
        {
            if (provisional_parent_node != nullptr)
            {
                // 父亲节点不为空，防止空指针错误
                provisional_parent_node->_next = provisional_node->_next;
            }
            else
            {
                // 父亲节点为空，直接将映射位置置空
                vector_hash_table[hash_map_location] = nullptr;
            }
        }

    public:
        using iterator = hash_iterator<hash_table_type_key, hash_table_type_value>;
        using const_iterator = hash_iterator<const hash_table_type_key, const hash_table_type_value>;
        hash_table()
        {
            _size = 0;
            load_factor = 7;
            hash_capacity = 10;
            vector_hash_table.resize(hash_capacity);
        }

        explicit hash_table(const uint64_t new_hash_table_capacity)
        {
            _size = 0;
            load_factor = 7;
            hash_capacity = new_hash_table_capacity;
            vector_hash_table.resize(hash_capacity);
        }
        hash_table(const hash_table &hash_table_data)
                : value_imitation_functions(hash_table_data.value_imitation_functions), _size(hash_table_data._size), load_factor(hash_table_data.load_factor),
                    hash_capacity(hash_table_data.hash_capacity), overall_list_before_node(nullptr),
                    overall_list_head_node(nullptr)
        {
            if (hash_capacity == 0)
            {
                return;
            }
            // 1. 分配同样大小的桶数组，所有桶初始为空
            vector_hash_table.resize(hash_capacity, nullptr);

            // 2. 遍历原表的每一个桶
            for (uint64_t new_hash_container_capacity = 0; new_hash_container_capacity < hash_capacity; ++new_hash_container_capacity)
            {
                container_node *src_bucket_node = hash_table_data.vector_hash_table[new_hash_container_capacity];
                // 桶内新表的尾节点（用于串联 _next）
                container_node *last_in_bucket = nullptr;

                // 逐节点深拷贝
                while (src_bucket_node)
                {
                    // 2.1 创建新节点并拷贝数据
                    auto *new_structure_node = new container_node(src_bucket_node->_data);
                    // 2.2 插入到“桶内部”链表
                    if (last_in_bucket != nullptr)
                    {
                        last_in_bucket->_next = new_structure_node;
                    }
                    else
                    {
                        vector_hash_table[new_hash_container_capacity] = new_structure_node;
                    }
                    last_in_bucket = new_structure_node;

                    // 2.3 插入到全局插入
                    if (overall_list_before_node != nullptr)
                    {
                        overall_list_before_node->overall_list_next = new_structure_node;
                        new_structure_node->overall_list_prev = overall_list_before_node;
                    }
                    else
                    {
                        // 第一个节点就是全局链表的头
                        overall_list_head_node = new_structure_node;
                    }
                    overall_list_before_node = new_structure_node;

                    // 继续下一个源节点
                    src_bucket_node = src_bucket_node->_next;
                }
            }
            if (overall_list_before_node != nullptr)
            {
                overall_list_before_node->overall_list_next = nullptr;
            }
        }
        hash_table(hash_table &&hash_table_data) noexcept
        {
            vector_hash_table = std::move(hash_table_data.vector_hash_table);
            _size = hash_table_data._size;
            load_factor = hash_table_data.load_factor;
            hash_capacity = hash_table_data.hash_capacity;
            hash_function_object = std::move(hash_table_data.hash_function_object);
            overall_list_before_node = std::move(hash_table_data.overall_list_before_node);
            overall_list_head_node = std::move(hash_table_data.overall_list_head_node);
            value_imitation_functions = std::move(hash_table_data.value_imitation_functions);
        }
        ~hash_table() noexcept
        {
            for (uint64_t i = 0; i < vector_hash_table.size(); ++i)
            {
                container_node *hash_bucket_delete = vector_hash_table[i];
                while (hash_bucket_delete != nullptr)
                {
                    container_node *hash_bucket_prev_node = hash_bucket_delete;
                    hash_bucket_delete = hash_bucket_delete->_next;
                    delete hash_bucket_prev_node;
                    hash_bucket_prev_node = nullptr;
                }
            }
        }
        bool change_load_factor(const uint64_t &new_load_factor) // 作用：改变负载因子大小
        {
            if (new_load_factor < 1)
            {
                return false;
            }
            load_factor = new_load_factor;
            return true;
        }
        iterator operator[](const hash_table_type_key &key_value)
        {
            if (_size == 0)
            {
                return iterator(nullptr);
            }
            else
            {
                uint64_t hash_value = value_imitation_functions(key_value);
                uint64_t hash_map_location = hash_value % hash_capacity;
                // 找到映射位置
                container_node *bucket_node = vector_hash_table[hash_map_location];
                while (bucket_node != nullptr)
                {
                    if (value_imitation_functions(bucket_node->_data) == value_imitation_functions(key_value))
                    {
                        return iterator(bucket_node);
                    }
                    bucket_node = bucket_node->_next;
                }
                return iterator(nullptr);
            }
        }
        iterator begin() { return iterator(overall_list_head_node); }

        const_iterator cbegin() const { return const_iterator(overall_list_head_node); }

        static iterator end()
        {
            return iterator(nullptr);
        }

        static const_iterator cend()
        {
            return const_iterator(nullptr);
        }

        uint64_t size() { return _size; }

        [[nodiscard]] uint64_t size() const
        {
            return _size;
        }

        [[nodiscard]] bool empty() const
        {
            return _size == 0;
        }

        uint64_t capacity() { return hash_capacity; }

        [[nodiscard]] uint64_t capacity() const
        {
            return hash_capacity;
        }

        bool push(const hash_table_type_value &hash_table_value_data)
        {
            if (find(hash_table_value_data).get_node() != nullptr)
            {
                return false;
            }
            // 判断扩容
            if (_size * 10 >= hash_capacity * load_factor)
            {
                // 扩容
                uint64_t new_container_capacity = (hash_capacity == 0 && vector_hash_table.empty()) ? 10 : hash_capacity * 2;
                // 新容量
                standard_con::vector<container_node *> new_vector_hash_table;
                new_vector_hash_table.resize(new_container_capacity, nullptr);
                uint64_t new_size = 0;
                // 重新映射,按照插入链表顺序
                container_node *regional_list_head_node = nullptr;            // 临时新哈希表全局头指针
                container_node *regional_list_previous_node = nullptr;        // 临时新哈希表全局上一个插入数据指针
                container_node *start_position_node = overall_list_head_node; // 全局链表头指针赋值
                while (start_position_node != nullptr)
                {
                    uint64_t new_mapping_value = hash_function_object(start_position_node->_data) % new_container_capacity;
                    // 重新计算映射值
                    container_node *hash_bucket_node = new_vector_hash_table[new_mapping_value];
                    if (hash_bucket_node == nullptr)
                    {
                        auto *new_mapping_data = new container_node(start_position_node->_data);
                        if (regional_list_head_node == nullptr)
                        {
                            new_mapping_data->overall_list_prev = nullptr;
                            new_mapping_data->overall_list_next = nullptr;
                            regional_list_head_node = regional_list_previous_node = new_mapping_data;
                        }
                        else
                        {
                            new_mapping_data->overall_list_prev = regional_list_previous_node;
                            regional_list_previous_node->overall_list_next = new_mapping_data;
                            regional_list_previous_node = new_mapping_data;
                        }
                        new_vector_hash_table[new_mapping_value] = new_mapping_data;
                        // 保存之前的遍历链表信息
                    }
                    else
                    {
                        auto *new_mapping_data = new container_node(start_position_node->_data);
                        if (regional_list_head_node == nullptr)
                        {
                            new_mapping_data->overall_list_prev = nullptr;
                            regional_list_head_node = regional_list_previous_node = new_mapping_data;
                        }
                        else
                        {
                            new_mapping_data->overall_list_prev = regional_list_previous_node;
                            regional_list_previous_node->overall_list_next = new_mapping_data;
                            regional_list_previous_node = new_mapping_data;
                        }
                        new_mapping_data->_next = hash_bucket_node;
                        new_vector_hash_table[new_mapping_value] = new_mapping_data;
                        // 头插节点
                    }
                    ++new_size;
                    start_position_node = start_position_node->overall_list_next;
                }
                // 释放旧哈希表
                for (uint64_t delete_traversal = 0; delete_traversal < vector_hash_table.size(); ++delete_traversal)
                {
                    container_node *hash_bucket_delete = vector_hash_table[delete_traversal];
                    while (hash_bucket_delete != nullptr)
                    {
                        container_node *hash_bucket_prev_node = hash_bucket_delete;
                        hash_bucket_delete = hash_bucket_delete->_next;
                        delete hash_bucket_prev_node;
                        hash_bucket_prev_node = nullptr;
                    }
                }
                _size = new_size;
                vector_hash_table.swap(new_vector_hash_table);
                hash_capacity = new_container_capacity;
                overall_list_head_node = regional_list_head_node;
                overall_list_before_node = regional_list_previous_node;
                // 重新映射,按照插入链表顺序
            }
            uint64_t hash_mapping_value = hash_function_object(hash_table_value_data);
            uint64_t hash_map_location = hash_mapping_value % hash_capacity;
            // 找到映射位置
            container_node *hash_bucket_node = vector_hash_table[hash_map_location];

            auto *new_mapping_data = new container_node(hash_table_value_data);
            new_mapping_data->_next = hash_bucket_node;
            vector_hash_table[hash_map_location] = new_mapping_data;
            if (_size == 0 && overall_list_head_node == nullptr)
            {
                new_mapping_data->overall_list_prev = nullptr;
                overall_list_head_node = overall_list_before_node = new_mapping_data;
            }
            else
            {
                new_mapping_data->overall_list_prev = overall_list_before_node;
                overall_list_before_node->overall_list_next = new_mapping_data;
                overall_list_before_node = new_mapping_data;
            }
            _size++;
            return true;
        }
        bool push(hash_table_type_value &&hash_table_value_data) noexcept
        {
            if (find(hash_table_value_data).get_node() != nullptr)
            {
                return false;
            }
            // 判断扩容
            if (_size * 10 >= hash_capacity * load_factor)
            {
                // 扩容
                uint64_t new_container_capacity = (hash_capacity == 0 && vector_hash_table.empty()) ? 10 : hash_capacity * 2;
                // 新容量
                standard_con::vector<container_node *> new_vector_hash_table;
                new_vector_hash_table.resize(new_container_capacity, nullptr);
                uint64_t new_size = 0;
                // 重新映射,按照插入链表顺序
                container_node *regional_list_head_node = nullptr;            // 临时新哈希表全局头指针
                container_node *regional_list_previous_node = nullptr;        // 临时新哈希表全局上一个插入数据指针
                container_node *start_position_node = overall_list_head_node; // 全局链表头指针赋值
                while (start_position_node != nullptr)
                {
                    uint64_t new_mapping_value = hash_function_object(start_position_node->_data) % new_container_capacity;
                    // 重新计算映射值
                    container_node *hash_bucket_node = new_vector_hash_table[new_mapping_value];
                    if (hash_bucket_node == nullptr)
                    {
                        auto *new_mapping_data = new container_node(std::forward<hash_table_type_value>(start_position_node->_data));
                        if (regional_list_head_node == nullptr)
                        {
                            new_mapping_data->overall_list_prev = nullptr;
                            new_mapping_data->overall_list_next = nullptr;
                            regional_list_head_node = regional_list_previous_node = new_mapping_data;
                        }
                        else
                        {
                            new_mapping_data->overall_list_prev = regional_list_previous_node;
                            regional_list_previous_node->overall_list_next = new_mapping_data;
                            regional_list_previous_node = new_mapping_data;
                        }
                        new_vector_hash_table[new_mapping_value] = new_mapping_data;
                        // 保存之前的遍历链表信息
                    }
                    else
                    {
                        auto *new_mapping_data = new container_node(std::forward<hash_table_type_value>(start_position_node->_data));
                        if (regional_list_head_node == nullptr)
                        {
                            new_mapping_data->overall_list_prev = nullptr;
                            regional_list_head_node = regional_list_previous_node = new_mapping_data;
                        }
                        else
                        {
                            new_mapping_data->overall_list_prev = regional_list_previous_node;
                            regional_list_previous_node->overall_list_next = new_mapping_data;
                            regional_list_previous_node = new_mapping_data;
                        }
                        new_mapping_data->_next = hash_bucket_node;
                        new_vector_hash_table[new_mapping_value] = new_mapping_data;
                        // 头插节点
                    }
                    ++new_size;
                    start_position_node = start_position_node->overall_list_next;
                }
                // 释放旧哈希表
                for (uint64_t delete_traversal = 0; delete_traversal < vector_hash_table.size(); ++delete_traversal)
                {
                    container_node *hash_bucket_delete = vector_hash_table[delete_traversal];
                    while (hash_bucket_delete != nullptr)
                    {
                        container_node *hash_bucket_prev_node = hash_bucket_delete;
                        hash_bucket_delete = hash_bucket_delete->_next;
                        delete hash_bucket_prev_node;
                        hash_bucket_prev_node = nullptr;
                    }
                }
                _size = new_size;
                vector_hash_table.swap(new_vector_hash_table);
                hash_capacity = new_container_capacity;
                overall_list_head_node = regional_list_head_node;
                overall_list_before_node = regional_list_previous_node;
                // 重新映射,按照插入链表顺序
            }
            uint64_t hash_mapping_value = hash_function_object(hash_table_value_data);
            uint64_t hash_map_location = hash_mapping_value % hash_capacity;
            // 找到映射位置
            container_node *hash_bucket_node = vector_hash_table[hash_map_location];

            auto *new_mapping_data = new container_node(std::forward<hash_table_type_value>(hash_table_value_data));
            new_mapping_data->_next = hash_bucket_node;
            vector_hash_table[hash_map_location] = new_mapping_data;
            if (_size == 0 && overall_list_head_node == nullptr)
            {
                new_mapping_data->overall_list_prev = nullptr;
                overall_list_head_node = overall_list_before_node = new_mapping_data;
            }
            else
            {
                new_mapping_data->overall_list_prev = overall_list_before_node;
                overall_list_before_node->overall_list_next = new_mapping_data;
                overall_list_before_node = new_mapping_data;
            }
            _size++;
            return true;
        }
        bool pop(const hash_table_type_value &hash_table_value_data)
        {
            // 空表判断
            if (find(hash_table_value_data).get_node() == nullptr)
            {
                return false;
            }
            uint64_t hash_mapping_value = hash_function_object(hash_table_value_data);
            uint64_t hash_map_location = hash_mapping_value % hash_capacity;
            // 找到映射位置
            container_node *hash_bucket_node = vector_hash_table[hash_map_location]; // 桶头节点赋值
            container_node *hash_bucket_parent_node = nullptr;                       // 保存上一个节点方便修改next指针的指向
            while (hash_bucket_node != nullptr)
            {
                // 找到位置
                if (value_imitation_functions(hash_bucket_node->_data) == value_imitation_functions(hash_table_value_data))
                {
                    if (overall_list_head_node == hash_bucket_node)
                    {
                        // 头节点删除情况
                        if (hash_bucket_node == overall_list_before_node)
                        {
                            // 只有一个节点
                            overall_list_head_node = overall_list_before_node = nullptr;
                            hash_chain_adjustment(hash_bucket_parent_node, hash_bucket_node, hash_map_location);
                        } // hash_chain_adjustment函数作用：检查hash_bucket_node节点是否哈希桶头结点，是则置空
                        else // 不是则将hash_bucket_node的_next节点赋值给hash_bucket_parent_node的_next指针，因为删除的是hash_bucket_node节点
                        {
                            // 是头节点，不是尾节点
                            hash_chain_adjustment(hash_bucket_parent_node, hash_bucket_node, hash_map_location);
                            overall_list_head_node = overall_list_head_node->overall_list_next;
                            overall_list_head_node->overall_list_prev = nullptr;
                        }
                    }
                    else if (hash_bucket_node == overall_list_before_node)
                    {
                        // 尾节点删除情况
                        hash_chain_adjustment(hash_bucket_parent_node, hash_bucket_node, hash_map_location);
                        overall_list_before_node = overall_list_before_node->overall_list_prev;
                        overall_list_before_node->overall_list_next = nullptr;
                    }
                    else
                    {
                        // 中间节点删除情况
                        hash_chain_adjustment(hash_bucket_parent_node, hash_bucket_node, hash_map_location);
                        hash_bucket_node->overall_list_prev->overall_list_next = hash_bucket_node->overall_list_next;
                        hash_bucket_node->overall_list_next->overall_list_prev = hash_bucket_node->overall_list_prev;
                    }
                    delete hash_bucket_node;
                    hash_bucket_node = nullptr;
                    --_size;
                    return true;
                }
                hash_bucket_parent_node = hash_bucket_node;
                hash_bucket_node = hash_bucket_node->_next;
                // 向下遍历
            }
            return false;
        }
        iterator find(const hash_table_type_value &hash_table_value_data)
        {
            if (_size == 0)
            {
                return iterator(nullptr);
            }
            else
            {
                uint64_t hash_mapping_value = hash_function_object(hash_table_value_data);
                uint64_t hash_map_location = hash_mapping_value % hash_capacity;
                // 找到映射位置
                container_node *hash_bucket_node = vector_hash_table[hash_map_location];
                while (hash_bucket_node != nullptr)
                {
                    if (value_imitation_functions(hash_bucket_node->_data) == value_imitation_functions(hash_table_value_data))
                    {
                        return iterator(hash_bucket_node);
                    }
                    hash_bucket_node = hash_bucket_node->_next;
                }
                return iterator(nullptr);
            }
        }
    };
    /**
   * @brief 位集合（BitSet）类实现
   *
   * 这是一个自定义的位集合类，使用`standard_con::vector<int>`存储位数据，
   *
   * 每个int（通常32位）可以存储32个布尔值（0或1）。
   *
   * 位集合可以高效地处理大量布尔值，常用于需要节省空间的场景，
   *
   * 如数据过滤、标记、集合运算等。
   *
   * 主要功能包括：
   *
   * - 设置指定位置的位（set）
   *
   * - 重置指定位置的位（reset）
   *
   * - 测试指定位置的位是否设置（test）
   *
   * - 调整大小（resize）
   *
   * - 拷贝构造和赋值操作
   */
    class bit_set
    {
        standard_con::vector<int> vector_bit_set;
        uint64_t _size;

    public:
        bit_set() : _size(0) { ; }
        explicit bit_set(const uint64_t &new_capacity)
        {
            _size = 0;
            vector_bit_set.resize((new_capacity / 32) + 1, 0);
            // 多开一个int的空间，防止不够
        }
        void resize(const uint64_t &new_capacity)
        {
            _size = 0;
            vector_bit_set.resize((new_capacity / 32) + 1, 0);
        }
        bit_set(const bit_set &bit_set_data)
        {
            vector_bit_set = bit_set_data.vector_bit_set;
            _size = bit_set_data._size;
        }
        bit_set &operator=(const bit_set &bit_set_data)
        {
            if (this != &bit_set_data)
            {
                vector_bit_set = bit_set_data.vector_bit_set;
                _size = bit_set_data._size;
            }
            return *this;
        }
        void set(const uint64_t &value_data)
        {
            // 把数映射到BitSet上的函数
            uint64_t mapping_bit = value_data / 32; // 定位到BitSet的位置在哪个int上
            uint64_t value_bit = value_data % 32;   // 定位到BitSet的位置在哪个int上的第几位
            vector_bit_set[mapping_bit] = vector_bit_set[mapping_bit] | (1 << value_bit);
            // 比较当前位置是否为1，若为1则不需要改变，若为0则需要改变，注意|只改变当前位不会改变其他位
            //|是两个条件满足一个条件就行，&是两个条件都满足才行
            // 其他位如果是1那么就还是1，如果是0那么就还是0，因为|是两个条件满足一个条件就行
            _size++;
        }
        void reset(const uint64_t &value_data)
        {
            // 删除映射的位置的函数
            uint64_t mapping_bit = value_data / 32;
            uint64_t value_bit = value_data % 32;
            vector_bit_set[mapping_bit] = vector_bit_set[mapping_bit] & (~(1 << value_bit));
            //&是两个条件都满足，~是取反，^是两个条件不同时满足
            // 1取反关键位是0其他位是1，这样就成功与掉，&要求是两个条件都需要满足
            // 其他位如果是1那么就不会改变原来的，如果是0那么还是为0，因为与是两个条件都需要满足
            _size--;
        }
        [[nodiscard]] uint64_t size() const
        {
            return _size;
        }
        bool test(const uint64_t &value_data)
        {
            if (_size == 0)
            {
                return false;
            }
            uint64_t mapping_bit = value_data / 32;
            uint64_t value_bit = value_data % 32;
            bool return_bit_set = vector_bit_set[mapping_bit] & (1 << value_bit);
            // 如果_BitSet[mapping_bit]里对应的位是1那么就返回true，否则返回false
            return return_bit_set;
        }
    };
}
namespace standard_con
{
    using base_container::bit_set;
    using base_container::hash_table;
    using base_container::red_black_tree;
}