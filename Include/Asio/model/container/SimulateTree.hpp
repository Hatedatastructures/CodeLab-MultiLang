#pragma once
#include "SimulateStack.hpp"
#include "SimulateAlgorithm.hpp"
#include "SimulateUtility.hpp"
#include "SimulateException.hpp"
namespace tree_container
{
    /*
            * @brief  #### `binary_tree` 类模板

            *   - 自定义二叉搜索树（BST）容器，基于节点结构实现

            *   - 元素按特定规则排序（左子树元素 < 根节点元素 < 右子树元素）

            *   - 支持插入、删除、查找、遍历等基本操作，采用非递归方式实现遍历

            * 模板参数:

            * * - `binary_search_tree_type`: 树中存储的元素类型
            *
            * * - `container_imitate_function`: 比较器类型，默认为 `standard_con::imitation_functions::less<binary_search_tree_type>`
            *   - 用于定义元素间的大小关系，返回 `true`
            *   - 可自定义比较规则，改变树的排序逻辑（如改为大于则为右子树）


            * 构造函数:

            * * - 初始化列表构造: 从元素初始化列表构造二叉搜索树，依次插入每个元素
            *
            * * - 单元素构造: 用单个元素初始化根节点
            *
            * * - 移动构造函数: 接管另一个临时二叉搜索树的资源（根节点指针），原对象根节点置空
            *
            * * - 拷贝构造函数: 深拷贝另一个二叉搜索树，通过栈遍历复制每个节点，保持树结构一致

            * 析构函数:

            * * - 调用 `clear()` 方法释放所有节点资源

            * 主要操作方法:

            * * - `middle_order_traversal()`: 公开接口，调用内部中序遍历方法，打印树中元素（有序序列）
            *
            * * - `pre_order_traversal()`: 公开接口，调用内部前序遍历方法，打印树中元素
            *
            * * - `push(const binary_search_tree_type& binary_search_tree_type_data)`:
            *   插入元素，根据比较器规则确定插入位置（左小右大），不允许重复元素，返回插入成功与否
            *
            * * - `pop(const binary_search_tree_type& binary_search_tree_type_data)`:
            *   删除指定元素节点
            *
            * * - `size()`: 计算树中节点总数（常量和非常量版本），通过中序遍历计数
            *
            * * - `find(const binary_search_tree_type& find_node)`:
            *   查找指定元素，返回节点指针（找到）或 `nullptr`（未找到），基于BST特性高效查找（O(log n)）
            *
            * * - `insert(const binary_search_tree_type& existing_value, const binary_search_tree_type& new_value)`:
            *   在指定元素的右子树插入新节点，新节点左孩子指向原右子树，用于特定场景的插入

            * 运算符重载:

            * * - `operator=`: 拷贝赋值运算符，先清空当前树，再深拷贝目标树资源
            *
            * * - `operator= (binary_tree&& binary_search_tree_object)`:
            *   移动赋值运算符，接管目标树资源，原对象根节点置空

            * 特性:

            * * - 有序性: 中序遍历结果为有序序列，符合比较器定义的规则
            *
            * * - 高效操作: 插入、删除、查找平均时间复杂度为 O(log n)（平衡时），最坏 O(n)（失衡时）
            *
            * * - 非递归实现: 遍历、拷贝等操作均用栈实现非递归逻辑，避免递归深度过大导致栈溢出
            *
            * * - 可定制性: 通过自定义比较器改变排序规则（如传入 `greater` 实现右小左大）
            *
            * * - 异常安全: 关键操作（如拷贝构造、插入）包含异常处理，未找到元素时抛出 `fault`

            * 注意事项:

            * * - 元素类型要求: 需支持比较器的操作（如 `less` 要求支持 `<` 运算符）
            *
            * * - 重复元素: `push()` 不允许插入重复元素，会直接返回 `false`
            *
            * * - 树失衡风险: 未实现自平衡机制，极端情况下可能退化为链表（如有序插入），影响性能
            *
            * * - 移动操作后: 原对象根节点置空，不可再使用，避免悬空指针访问
            *
            * * - `insert()` 方法: 仅用于特定场景，可能破坏BST特性，需谨慎使用

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename binary_search_tree_type, typename container_imitate_function = standard_con::less<binary_search_tree_type>>
    class binary_tree
    {
    private:
        class binary_search_tree_type_node
        {
        public:
            // 节点类
            binary_search_tree_type_node *_left;
            binary_search_tree_type_node *_right;
            binary_search_tree_type _data;
            explicit binary_search_tree_type_node(const binary_search_tree_type &binary_search_tree_type_data = binary_search_tree_type())
                    : _left(nullptr), _right(nullptr), _data(binary_search_tree_type_data)
            {
                ;
            }
            ~binary_search_tree_type_node()
            {
                _left = nullptr;
                _right = nullptr;
            }
        };
        using container_node = binary_search_tree_type_node;
        container_node *_root;                      // 根节点
        container_imitate_function function_policy; // 仿函数对象
        void interior_middle_order_traversal(container_node *root_subtree_node)
        {
            // 内调中序遍历函数
            standard_con::stack<container_node *> interior_stack;
            while (root_subtree_node != nullptr || !interior_stack.empty())
            {
                while (root_subtree_node != nullptr)
                {
                    interior_stack.push(root_subtree_node);
                    // 压栈
                    root_subtree_node = root_subtree_node->_left;
                }
                // 访问栈顶节点
                root_subtree_node = interior_stack.top();
                // 弹出栈顶元素，刷新栈顶元素，栈顶元素会变成之前压入栈的节点的父节点

                interior_stack.pop();
                std::cout << root_subtree_node->_data << " ";
                // std::cout << &root_subtree_node->_data << " ";
                // 检查地址是不是值拷贝
                // 转向右子树
                root_subtree_node = root_subtree_node->_right;
            }
        }
        uint64_t interior_middle_order_traversal(container_node *root_subtree_node, uint64_t &node_counter)
        {
            standard_con::stack<container_node *> interior_stack;
            while (root_subtree_node != nullptr || !interior_stack.empty())
            {
                while (root_subtree_node != nullptr)
                {
                    interior_stack.push(root_subtree_node);
                    root_subtree_node = root_subtree_node->_left;
                }
                root_subtree_node = interior_stack.top();
                interior_stack.pop();
                node_counter++;
                root_subtree_node = root_subtree_node->_right;
            }
            return node_counter;
        }
        void interior_pre_order_traversal(container_node *root_subtree_node)
        {
            // 前序遍历，最外左子树全部压栈
            if (root_subtree_node == nullptr)
            {
                return;
            }
            container_node *reference_node = root_subtree_node;
            standard_con::stack<container_node *> interior_stack;
            interior_stack.push(reference_node);
            // 不能添加|| reference_node != nullptr ，因为最后一层循环后reference_node指针还是为真后面循环无意义，反之还会破环性质
            while (!interior_stack.empty())
            {
                reference_node = interior_stack.top();
                interior_stack.pop();

                std::cout << reference_node->_data << " ";
                if (reference_node->_right != nullptr)
                {
                    interior_stack.push(reference_node->_right);
                }
                if (reference_node->_left != nullptr)
                {
                    interior_stack.push(reference_node->_left);
                } // 修改逻辑错误，先压右子树再压左子树，因为这是栈
            }
        }
        void clear() noexcept
        {
            if (_root == nullptr)
            {
                return;
            }
            standard_con::stack<container_node *> resource_release_stack;
            resource_release_stack.push(_root);
            while (resource_release_stack.empty() == false)
            {
                container_node *pending_deletion_node = resource_release_stack.top();
                // 取出元素，把左右节点入进去
                resource_release_stack.pop();
                if (pending_deletion_node->_left != nullptr)
                {
                    resource_release_stack.push(pending_deletion_node->_left);
                }
                if (pending_deletion_node->_right != nullptr)
                {
                    resource_release_stack.push(pending_deletion_node->_right);
                }
                delete pending_deletion_node;
            }
            _root = nullptr;
        }

    public:
        ~binary_tree() noexcept { clear(); }
        // 构造函数，使用初始化列表来初始化二叉搜索树
        binary_tree(std::initializer_list<binary_search_tree_type> lightweight_container)
        {
            _root = nullptr;
            for (auto &chained_values : lightweight_container)
            {
                push(chained_values);
            }
        }
        explicit binary_tree(const binary_search_tree_type &bstt_node = binary_search_tree_type())
                : _root(nullptr)
        {
            _root = new container_node(bstt_node);
        }
        binary_tree(binary_tree &&binary_search_tree_object) noexcept
                : _root(nullptr), function_policy(binary_search_tree_object.function_policy)
        {
            _root = std::move(binary_search_tree_object._root);
            binary_search_tree_object._root = nullptr;
        }
        binary_tree(const binary_tree &binary_search_tree_object)
                : _root(nullptr), function_policy(binary_search_tree_object.function_policy)
        // 这个拷贝构造不需要传模板参数，因为模板参数是在编译时确定的，而不是在运行时确定的，对于仿函数，直接拿传进来的引用初始化就可以了
        {
            // 拷贝构造，时间复杂度为O(n)
            container_node *reference_node = binary_search_tree_object._root;
            if (reference_node == nullptr)
            {
                throw custom_exception::fault("拷贝构造失败二叉搜索树为空", "binary_tree", __LINE__);
            }
            standard_con::stack<standard_con::pair<container_node *, container_node **>> interior_stack;
            // 注意这里把本地_root类型传过去，是因为要对本地的_root进行操作，所以要传二级指针
            // 这里传引用也不行，这里的对象是动态变化的，所以传引用也不行
            // 如果是对全局的_ROOT进行操作，就传一级指针
            interior_stack.push(standard_con::pair<container_node *, container_node **>(reference_node, &_root));
            while (!interior_stack.empty())
            {
                auto pair_node = interior_stack.top();
                interior_stack.pop();
                *(pair_node.second) = new container_node(pair_node.first->_data);
                // container_node* _staic_temp_pair_second = *(pair_node.second);
                // if(pair_node.first->_left!= nullptr)
                // { //远古版本
                //     interior_stack.push(MY_Template::practicality::pair<container_node*,container_node**>
                //     (pair_node.first->_left,&_staic_temp_pair_second->_left));
                // }
                // if(pair_node.first->_right!= nullptr)
                // {
                //     interior_stack.push(MY_Template::practicality::pair<container_node*,container_node**>
                //     (pair_node.first->_right,&_staic_temp_pair_second->_right));
                // }
                // 移除临时变量，直接使用指针解引用
                if (pair_node.first->_right != nullptr)
                {
                    interior_stack.push(standard_con::pair<container_node *, container_node **>(pair_node.first->_right, &((*pair_node.second)->_right)));
                }
                if (pair_node.first->_left != nullptr)
                {
                    interior_stack.push(standard_con::pair<container_node *, container_node **>(pair_node.first->_left, &((*pair_node.second)->_left)));
                }
            }
        }
        void middle_order_traversal()
        {
            interior_middle_order_traversal(_root);
        }
        void pre_order_traversal()
        {
            interior_pre_order_traversal(_root);
        }
        bool push(const binary_search_tree_type &binary_search_tree_type_data)
        {
            if (_root == nullptr)
            {
                _root = new container_node(binary_search_tree_type_data);
                return true;
            }
            else
            {
                container_node *reference_node = _root;
                container_node *subtree_node = nullptr;
                while (reference_node != nullptr)
                {
                    subtree_node = reference_node;
                    if (!function_policy(binary_search_tree_type_data, reference_node->_data) &&
                            !function_policy(reference_node->_data, binary_search_tree_type_data))
                    {
                        // 改用仿函数特性，判断是否有重复元素,防止自定义类型没有重载==运算符
                        return false;
                    }
                    else if (function_policy(binary_search_tree_type_data, reference_node->_data))
                    {
                        reference_node = reference_node->_left;
                    }
                    else
                    {
                        reference_node = reference_node->_right;
                    }
                }
                // 新开节点链接
                auto *new_element_node = new container_node(binary_search_tree_type_data);
                // 链接节点
                if (function_policy(binary_search_tree_type_data, subtree_node->_data))
                {
                    subtree_node->_left = new_element_node;
                }
                else
                {
                    subtree_node->_right = new_element_node;
                }
                return true;
            }
        }
        binary_tree &pop(const binary_search_tree_type &binary_search_tree_type_data)
        {
            // 删除节点
            container_node *reference_node = _root;
            container_node *subtree_node = nullptr;
            while (reference_node != nullptr)
            {
                if (binary_search_tree_type_data == reference_node->_data)
                {
                    // 找到节点
                    if (reference_node->_left == nullptr)
                    {
                        // 左子树为空,下面判断要删除的节点是父节点的左子树还是右子树，防止多删和误删
                        if (subtree_node == nullptr)
                        {
                            // 当前节点是根节点，直接更新 _root
                            _root = reference_node->_right;
                        }
                        else
                        {
                            if (subtree_node->_left == reference_node)
                            {
                                // 根节点
                                subtree_node->_left = reference_node->_right;
                            }
                            else
                            {
                                // 非根节点
                                subtree_node->_right = reference_node->_right;
                            }
                        }
                        delete reference_node;
                        reference_node = nullptr;
                        return *this;
                    }
                    else if (reference_node->_right == nullptr)
                    {
                        if (subtree_node == nullptr)
                        {
                            // 防止当前节点是根节点，无法解引用，直接更新 _root
                            _root = reference_node->_left;
                        }
                        else
                        {
                            if (subtree_node->_left == reference_node)
                            {
                                subtree_node->_left = reference_node->_left;
                            }
                            else
                            {
                                subtree_node->_right = reference_node->_left;
                            }
                        }
                        delete reference_node;
                        reference_node = nullptr;
                        return *this;
                    }
                    else
                    {
                        // 左右子树都不为空，找右子树的最左节点
                        container_node *right_subtree_least_node = reference_node->_right;
                        container_node *subtree_parent_node = reference_node;
                        while (right_subtree_least_node->_left != nullptr)
                        {
                            subtree_parent_node = right_subtree_least_node;
                            right_subtree_least_node = right_subtree_least_node->_left;
                        }
                        // 找到最左节点
                        standard_con::algorithm::swap(reference_node->_data, right_subtree_least_node->_data);
                        // 因为右树最左节点已经被删，但是还需要把被删的上一节点的左子树指向被删节点的右子树，不管右子树有没有节点都要连接上
                        if (subtree_parent_node == reference_node)
                        {
                            // 说明右子树没有左子树最小节点就是右子树的第一个根，如同上面判断条件：要删除的根节点等于右子树最小节点的父亲节点
                            subtree_parent_node->_right = right_subtree_least_node->_right;
                            // 这俩交换指针指向位置就行，上面已经完成值的替换
                        }
                        else
                        {
                            // 情况2：说明要删除的数据的右子树的最左节点如果有数据，就把数据连接到右子树的最左节点的父亲节点的左子树指向最左子树的右子树
                            subtree_parent_node->_left = right_subtree_least_node->_right;
                        }
                        delete right_subtree_least_node;
                        right_subtree_least_node = nullptr;
                        return *this;
                    }
                }
                else if (function_policy(binary_search_tree_type_data, reference_node->_data))
                {
                    subtree_node = reference_node;
                    reference_node = reference_node->_left;
                }
                else
                {
                    subtree_node = reference_node;
                    reference_node = reference_node->_right;
                }
            }
            return *this;
        }
        uint64_t size()
        {
            uint64_t _size = 0;
            return interior_middle_order_traversal(_root, _size);
        }
        [[nodiscard]] uint64_t size() const
        {
            uint64_t node_number_counter = 0;
            return interior_middle_order_traversal(_root, node_number_counter);
        }
        container_node *find(const binary_search_tree_type &find_node)
        {
            // 查找函数
            container_node *reference_node = _root;
            while (reference_node != nullptr)
            {
                if (find_node == reference_node->_data)
                {
                    return reference_node;
                }
                else if (function_policy(find_node, reference_node->_data))
                {
                    reference_node = reference_node->_left;
                }
                else
                {
                    reference_node = reference_node->_right;
                }
            }
            return nullptr;
        }
        void insert(const binary_search_tree_type &existing_value, const binary_search_tree_type &new_value)
        {
            // 在existing_value后面插入new_value
            container_node *existing_value_node = find(existing_value);
            // 插入节点
            if (existing_value_node == nullptr)
            {
                throw custom_exception::fault("传入值未找到！", "insert::find", __LINE__);
            }
            else
            {
                auto *new_value_node = new container_node(new_value);
                new_value_node->_left = existing_value_node->_right;
                existing_value_node->_right = new_value_node;
            }
        }
        binary_tree &operator=(const binary_tree &binary_search_tree_object)
        {
            // 赋值运算符重载
            if (this != &binary_search_tree_object)
            {
                clear();
                function_policy = binary_search_tree_object.function_policy;
                binary_tree reference_node = binary_search_tree_object;
                standard_con::algorithm::swap(reference_node._root, _root);
            }
            return *this;
        }
        binary_tree &operator=(binary_tree &&binary_search_tree_object) noexcept
        {
            // 移动赋值运算符重载
            if (this != &binary_search_tree_object)
            {
                clear();
                function_policy = binary_search_tree_object.function_policy;
                _root = std::move(binary_search_tree_object._root);
                binary_search_tree_object._root = nullptr;
            }
            return *this;
        }
    };
    /*
            * @brief  #### `balance_tree` 类模板

            *   - 实现严格自平衡二叉搜索树（AVL树），通过维护平衡因子保证树的平衡性

            *   - 每个节点的左右子树高度差不超过1，支持高效的插入、删除和查找操作

            *   - 存储键值对数据，键用于排序，值用于存储关联信息

            * 模板参数:

            * * - `avl_tree_type_k`: 键的类型，用于节点排序和比较
            *
            * * - `avl_tree_type_v`: 值的类型，与键关联的存储数据
            *
            * * - `container_imitate_function`: 比较器类型，默认为 `standard_con::imitation_functions::less<avl_tree_type_k>`
            *   - 用于定义键的大小关系，返回 `true` 表示左操作数小于右操作数
            *   - 可自定义比较规则，改变树的排序逻辑
            *
            * * - `avl_tree_node_pair`: 键值对类型，默认为 `standard_con::pair<avl_tree_type_k, avl_tree_type_v>`
            *   - 用于存储节点中的键值对数据

            * 迭代器相关方法:

            * * - `begin()`: 返回指向最左节点的迭代器（中序遍历的第一个节点）
            *
            * * - `end()`: 返回指向 `nullptr` 的迭代器（表示遍历结束）
            *
            * * - `cbegin()`/`cend()`: 常量版本的起始和结束迭代器
            *
            * * - `rbegin()`: 返回指向最右节点的反向迭代器（中序遍历的最后一个节点）
            *
            * * - `rend()`: 返回指向 `nullptr` 的反向迭代器（表示反向遍历结束）
            *
            * * - `crbegin()`/`crend()`: 常量版本的反向起始和结束迭代器

            * 构造函数:

            * * - 默认构造函数: 初始化空树（根节点为 `nullptr`）
            *
            * * - 从键值构造: 用指定键和值初始化根节点
            *
            * * - 从键值对构造: 用指定键值对初始化根节点
            *
            * * - 拷贝构造函数: 深拷贝另一个AVL树，通过栈遍历复制每个节点（含平衡因子和父指针关系）
            *
            * * - 移动构造函数: 接管另一个临时AVL树的资源（根节点指针），原树根节点置空

            * 析构函数:

            * * - 调用 `clear()` 方法释放所有节点资源

            * 主要操作方法:

            * * - `push()`: 插入键值对节点（支持键值分离和键值对两种形式）
            *   - 插入后更新祖先节点的平衡因子，若平衡因子为±2则触发相应旋转操作
            *   - 返回 `true` 表示插入成功，`false` 表示键已存在（不允许重复键）
            *
            * * - `pop(const avl_tree_type_k& key_data)`: 删除指定键的节点
            *   - 处理三种删除情况（左子树空、右子树空、左右子树均非空），删除后更新平衡因子并可能触发旋转
            *   - 左右子树均非空时，用右子树最左节点（中序后继）替换当前节点，再删除后继节点
            *
            * * - `find(const avl_tree_type_k& key_data)`: 查找指定键的节点，返回节点指针（找到）或 `nullptr`（未找到）
            *   - 基于AVL树特性高效查找，时间复杂度为 O(log n)
            *
            * * - `size()`: 计算树中节点总数（常量和非常量版本），通过前序遍历计数
            *
            * * - `empty()`: 判断树是否为空（根节点为 `nullptr` 则返回 `true`）
            *
            * * - 遍历方法:
            *   - `pre_order_traversal()`: 前序遍历（根->左->右），打印节点数据
            *   - `middle_order_traversal()`: 中序遍历（左->根->右），打印节点数据（结果为有序序列）

            * 运算符重载:

            * * - `operator=`: 拷贝赋值运算符，先清空当前树，再通过交换资源实现深拷贝
            *
            * * - `operator= (balance_tree&& avl_tree_data)`: 移动赋值运算符，接管目标树资源，原树根节点置空

            * 特性:

            * * - 自平衡性: 通过平衡因子（±1、0）和旋转操作维持树的平衡性，确保插入、删除、查找的时间复杂度为 O(log n)
            *
            * * - 平衡因子维护: 插入/删除节点后，沿路径向上更新平衡因子，超过±1时触发旋转
            *
            * * - 非递归实现: 遍历、拷贝等操作均用栈实现非递归逻辑，避免递归深度过大导致栈溢出
            *
            * * - 迭代器支持: 提供正向和反向迭代器，支持范围for循环遍历，迭代器在旋转后仍保持有效
            *
            * * - 异常安全: 关键操作（如旋转、插入）包含异常处理，空指针传入时抛出 `fault`

            * 注意事项:

            * * - 键类型要求: 需支持比较器的操作（如 `less` 要求支持 `<` 运算符）
            *
            * * - 重复键处理: `push()` 不允许插入重复键，会直接返回 `false`
            *
            * * - 旋转对结构的影响: 旋转操作会改变节点的父子关系，但不影响中序遍历的有序性
            *
            * * - 迭代器有效性: 旋转和删除操作可能改变节点地址，但迭代器仍能正确指向目标节点
            *
            * * - 移动操作后: 原树资源被接管，不可再使用，避免悬空指针访问

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename avl_tree_type_k, typename avl_tree_type_v, typename container_imitate_function = standard_con::less<avl_tree_type_k>,
                        typename avl_tree_node_pair = standard_con::pair<avl_tree_type_k, avl_tree_type_v>>
    class balance_tree
    {
    private:
        class avl_tree_type_node
        {
        public:
            avl_tree_node_pair _data;

            avl_tree_type_node *_left;
            avl_tree_type_node *_right;
            avl_tree_type_node *_parent;
            // 平衡因子
            int _balance_factor;
            explicit avl_tree_type_node(const avl_tree_type_k &avl_tt_k_data = avl_tree_type_k(), const avl_tree_type_v &avl_tt_v_data = avl_tree_type_v())
                    : _data(avl_tt_k_data, avl_tt_v_data), _left(nullptr), _right(nullptr), _parent(nullptr), _balance_factor(0)
            {
                ;
            }
            explicit avl_tree_type_node(const avl_tree_node_pair &pair_type_data)
                    : _data(pair_type_data), _left(nullptr), _right(nullptr), _parent(nullptr), _balance_factor(0)
            {
                ;
            }
        };
        template <typename T, typename Ref, typename Ptr>
        class avl_tree_iterator
        {
        public:
            using iterator_node = avl_tree_type_node;
            using self = avl_tree_iterator<T, Ref, Ptr>;
            using pointer = Ptr;
            using reference = Ref;
            iterator_node *_node_iterator_ptr;
            explicit avl_tree_iterator(iterator_node *iterator_ptr_data)
                    : _node_iterator_ptr(iterator_ptr_data) { ; }

            Ptr operator->() { return &(_node_iterator_ptr->_data); }

            Ref &operator*() { return _node_iterator_ptr->_data; }

            bool operator!=(const self &another_iterator) { return _node_iterator_ptr != another_iterator._node_iterator_ptr; }

            bool operator==(const self &another_iterator) { return _node_iterator_ptr == another_iterator._node_iterator_ptr; }
            self &operator++()
            {
                if (_node_iterator_ptr->_right != nullptr)
                {
                    _node_iterator_ptr = _node_iterator_ptr->_right;
                    while (_node_iterator_ptr->_left != nullptr)
                    {
                        _node_iterator_ptr = _node_iterator_ptr->_left;
                    }
                }
                else
                {
                    container_node *_iterator_self_node = _node_iterator_ptr;
                    while (_iterator_self_node->_parent != nullptr && _iterator_self_node == _iterator_self_node->_parent->_right)
                    {
                        _iterator_self_node = _iterator_self_node->_parent;
                    }
                    _node_iterator_ptr = _iterator_self_node->_parent;
                }
                return *this;
            }
            self operator++(int)
            {
                self return_self = *this;
                ++(*this);
                return return_self;
            }
            self &operator--()
            {
                if (_node_iterator_ptr->_left != nullptr)
                {
                    _node_iterator_ptr = _node_iterator_ptr->_left;
                    while (_node_iterator_ptr->_right != nullptr)
                    {
                        _node_iterator_ptr = _node_iterator_ptr->_right;
                    }
                }
                else
                {
                    container_node *_iterator_self_node = _node_iterator_ptr;
                    while (_iterator_self_node->_parent != nullptr && _iterator_self_node == _iterator_self_node->_parent->_left)
                    {
                        _iterator_self_node = _iterator_self_node->_parent;
                    }
                    _node_iterator_ptr = _iterator_self_node->_parent;
                }
                return *this;
            }
            self operator--(int)
            {
                self return_self = *this;
                --(*this);
                return return_self;
            }
        };
        template <typename iterator>
        class avl_tree_reverse_iterator
        {
        public:
            using self = avl_tree_reverse_iterator<iterator>;
            iterator _it;
            using Ptr = typename iterator::pointer;
            using Ref = typename iterator::reference;
            explicit avl_tree_reverse_iterator(iterator iterator_data)
                    : _it(iterator_data) { ; }

            Ptr operator->() { return &(*this); }

            Ref &operator*() { return *_it; }

            bool operator!=(const self &another_iterator) { return _it != another_iterator._it; }

            bool operator==(const self &another_iterator) { return _it == another_iterator._it; }

            self &operator++()
            {
                --_it;
                return *this;
            }
            self operator++(int)
            {
                self return_self = *this;
                --(*this);
                return return_self;
            }
            self &operator--()
            {
                ++_it;
                return *this;
            }
            self operator--(int)
            {
                self return_self = *this;
                ++(*this);
                return return_self;
            }
        };
        using container_node = avl_tree_type_node;
        container_node *_root;

        container_imitate_function function_policy;
        void left_revolve(container_node *&subtree_node)
        {
            /*                                                                                                              左单旋情况：简化图
                    传进来的值是发现该树平衡性被破坏的节点地址                                                                     subtree_node(10)(当前平衡因子为-2，触发调整)
                    大致思想：因为这是左单旋，所以找传进来的父亲节点的右根节点来当调整节点                                                         \
                    然后把调整节点的左根节点赋值给传进来的父亲节点的右根节点 (刚才已经用节点保存过调整节点，所以这里直接赋值)，                   sub_tree_right_node(20)(当前平衡因子为-1，不触发调整)
                    再把父亲节点赋值给调整节点的左根节点，！！注意：在旋转的过程中还要处理每个调整节点的父亲节点的指向和平衡因子                      /         \
                    {                                                                                            sub_right_left_node(nullptr)  sub_tree_right_right_node(30)
                            container_node* sub_tree_right_node = subtree_node->_right;                                  /############             分割线             ############/
                            subtree_node->_right = sub_tree_right_node->_left;                                                          sub_tree_right_node(20)(当前平衡因子为0)
                            sub_tree_right_node->_left = subtree_node;                                                                        /       \
                            //错误写法：未同步调整父亲节点和判断调整节点的左根节点是否为空，以及全部需要调整节点的父亲指针的指针的指向        subtree_node(10)    sub_tree_right_right_node(30)
                    }                                                                                                                 \
                    if(subtree_node == nullptr || subtree_node->_right == nullptr)                                 sub_right_left_node(nullptr)
                    {
                            std::cout <<"left "<< "空指针"  <<std::endl;
                            return ;
                    }
            */
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
            subtree_node->_right = sub_right_left_node; // 因为调整后parent_node的位置是sub_right_left_node的位置
            if (sub_right_left_node)                    // 把parent_node的右根节点赋值给sub_right_left_node
            {
                sub_right_left_node->_parent = subtree_node;
                // 如果sub_right_left_node(调整节点的左根节点)不等于空，还需要调整sub_right_left_node它的父亲节点
            }
            sub_tree_right_node->_left = subtree_node;
            // 这里先保存一下parent_node的父亲地址，防止到下面else比较的时候丢失
            container_node *parent_node = subtree_node->_parent;
            subtree_node->_parent = sub_tree_right_node;
            // 更新parent_node节点指向正确的地址

            if (_root == subtree_node)
            {
                // 如果要调整的节点是根节点，直接把调整节点赋值给根节点，然后把调整节点的父亲节点置空
                _root = sub_tree_right_node;
                sub_tree_right_node->_parent = nullptr;
            }
            else
            {
                // 调整前parent_node是这个树的根现在是sub_tree_right_node是这个树的根
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
            subtree_node->_balance_factor = sub_tree_right_node->_balance_factor = 0;
        }

        void right_revolve(container_node *&subtree_node)
        {
            // 思路同左单旋思路差不多,实现相反，图略
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
            subtree_node->_balance_factor = sub_tree_left_node->_balance_factor = 0;
        }
        void right_left_revolve(container_node *&subtree_node)
        {
            try
            {
                if (subtree_node == nullptr)
                {
                    throw custom_exception::fault("右左双旋传进来的节点为空！", "right_left_revolve", __LINE__);
                }
            }
            catch (const custom_exception::fault &exception)
            {
                std::cerr << exception.what() << exception.function_name_get() << exception.line_number_get() << std::endl;
                throw;
            }
            container_node *sub_tree_right_node = subtree_node->_right;
            container_node *sub_right_left_node = sub_tree_right_node->_left;
            int separate_balance_factor = sub_right_left_node->_balance_factor;

            right_revolve(subtree_node->_right);
            // 右旋
            left_revolve(subtree_node);
            // 左旋
            if (separate_balance_factor == -1)
            {
                subtree_node->_balance_factor = 0;
                sub_tree_right_node->_balance_factor = 1;
                sub_right_left_node->_balance_factor = 0;
            }
            else if (separate_balance_factor == 1)
            {
                subtree_node->_balance_factor = -1;
                sub_tree_right_node->_balance_factor = 0;
                sub_right_left_node->_balance_factor = 0;
            }
            else
            {
                subtree_node->_balance_factor = 0;
                sub_tree_right_node->_balance_factor = 0;
                sub_right_left_node->_balance_factor = 0;
            }
        }
        void left_right_revolve(container_node *&subtree_node)
        {
            try
            {
                if (subtree_node == nullptr)
                {
                    throw custom_exception::fault("左右单旋传进来的节点为空！", "left_right_revolve", __LINE__);
                }
            }
            catch (const custom_exception::fault &exception)
            {
                std::cerr << exception.what() << exception.function_name_get() << exception.line_number_get() << std::endl;
                throw;
            }
            container_node *sub_tree_left_node = subtree_node->_left;
            container_node *sub_left_right_node = sub_tree_left_node->_right;
            int separate_balance_factor = sub_left_right_node->_balance_factor;

            left_revolve(subtree_node->_left);
            // 左旋
            right_revolve(subtree_node);
            // 右旋
            if (separate_balance_factor == -1)
            {
                subtree_node->_balance_factor = 0;
                sub_tree_left_node->_balance_factor = 1;
                sub_left_right_node->_balance_factor = 0;
            }
            else if (separate_balance_factor == 1)
            {
                subtree_node->_balance_factor = -1;
                sub_tree_left_node->_balance_factor = 0;
                sub_left_right_node->_balance_factor = 0;
            }
            else
            {
                subtree_node->_balance_factor = 0;
                sub_tree_left_node->_balance_factor = 0;
                sub_left_right_node->_balance_factor = 0;
            }
        }
        void clear() noexcept
        {
            // 清空所有资源
            if (_root == nullptr)
            {
                return;
            }
            else
            {
                standard_con::stack<container_node *> interior_stack;
                // 前序释放
                interior_stack.push(_root);
                while (!interior_stack.empty())
                {
                    container_node *delete_data_node = interior_stack.top();
                    interior_stack.pop();
                    if (delete_data_node->_left != nullptr)
                    {
                        interior_stack.push(delete_data_node->_left);
                    }
                    if (delete_data_node->_right != nullptr)
                    {
                        interior_stack.push(delete_data_node->_right);
                    }
                    delete delete_data_node;
                    delete_data_node = nullptr;
                }
                _root = nullptr;
            }
        }
        // 测试函数
        void interior_pre_order_traversal(container_node *root_subtree_node)
        {
            // 前序遍历，最外左子树全部压栈
            if (root_subtree_node == nullptr)
            {
                return;
            }
            container_node *reference_node = root_subtree_node;
            standard_con::stack<container_node *> interior_stack;
            interior_stack.push(reference_node);
            // 不能添加|| reference_node != nullptr ，因为最后一层循环后_Pre_order_traversal_test还是为真后面循环无意义，反之还会破环性质
            while (!interior_stack.empty())
            {
                reference_node = interior_stack.top();
                interior_stack.pop();

                std::cout << reference_node->_data << " ";
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
        void interior_middle_order_traversal(container_node *root_subtree_node)
        {
            // 中序遍历函数
            standard_con::stack<container_node *> interior_stack;
            while (root_subtree_node != nullptr || !interior_stack.empty())
            {
                while (root_subtree_node != nullptr)
                {
                    interior_stack.push(root_subtree_node);
                    // 压栈
                    root_subtree_node = root_subtree_node->_left;
                }
                // 访问栈顶节点
                root_subtree_node = interior_stack.top();
                // 弹出栈顶元素，刷新栈顶元素，栈顶元素会变成之前压入栈的节点的父节点

                interior_stack.pop();
                std::cout << root_subtree_node->_data << " ";
                // std::cout << &root_subtree_node->_data << " ";
                // 检查地址是不是值拷贝
                // 转向右子树
                root_subtree_node = root_subtree_node->_right;
            }
        }
        uint64_t _size()
        {
            uint64_t avl_tree_node_counters = 0;
            if (_root == nullptr)
            {
                return avl_tree_node_counters;
            }
            else
            {
                container_node *reference_node = _root;
                standard_con::stack<container_node *> interior_stack;
                interior_stack.push(reference_node);
                while (!interior_stack.empty())
                {
                    reference_node = interior_stack.top();
                    interior_stack.pop();

                    avl_tree_node_counters++;

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
            return avl_tree_node_counters;
        }

    public:
        using iterator = avl_tree_iterator<avl_tree_node_pair, avl_tree_node_pair &, avl_tree_node_pair *>;
        using const_iterator = avl_tree_iterator<avl_tree_node_pair, const avl_tree_node_pair &, const avl_tree_node_pair *>;

        using reverse_iterator = avl_tree_reverse_iterator<iterator>;
        using const_reverse_iterator = avl_tree_reverse_iterator<const_iterator>;

        iterator begin()
        {
            container_node *start_position_node = _root;
            while (start_position_node != nullptr && start_position_node->_left != nullptr)
            {
                start_position_node = start_position_node->_left;
            }
            return iterator(start_position_node);
        }

        static iterator end()
        {
            return iterator(nullptr);
        }
        const_iterator cbegin() const
        {
            container_node *start_position_node = _root;
            while (start_position_node != nullptr && start_position_node->_left != nullptr)
            {
                start_position_node = start_position_node->_left;
            }
            return const_iterator(start_position_node);
        }

        static const_iterator cend()
        {
            return const_iterator(nullptr);
        }
        reverse_iterator rbegin()
        {
            container_node *reverse_start_position = _root;
            while (reverse_start_position != nullptr && reverse_start_position->_right != nullptr)
            {
                reverse_start_position = reverse_start_position->_right;
            }
            return reverse_iterator(iterator(reverse_start_position));
        }

        static reverse_iterator rend()
        {
            return reverse_iterator(iterator(nullptr));
        }
        const_reverse_iterator crbegin() const
        {
            container_node *reverse_start_position = _root;
            while (reverse_start_position != nullptr && reverse_start_position->_right != nullptr)
            {
                reverse_start_position = reverse_start_position->_right;
            }
            return const_reverse_iterator(const_iterator(reverse_start_position));
        }

        static const_reverse_iterator crend()
        {
            return const_reverse_iterator(const_iterator(nullptr));
        }
        bool empty()
        {
            return _root == nullptr;
        }
        balance_tree()
        {
            _root = nullptr;
        }
        explicit balance_tree(const avl_tree_type_k &key_data, const avl_tree_type_v &val_data = avl_tree_type_v(),
                                                                                container_imitate_function com_value = container_imitate_function())
                : _root(nullptr), function_policy(com_value)
        {
            _root = new container_node(key_data, val_data);
        }
        explicit balance_tree(const avl_tree_node_pair &pair_type_data,
                                                                                container_imitate_function com_value = container_imitate_function())
                : _root(nullptr), function_policy(com_value)
        {
            _root = new container_node(pair_type_data.first, pair_type_data.second);
        }
        balance_tree(const balance_tree &avl_tree_data)
                : _root(nullptr), function_policy(avl_tree_data.function_policy)
        {
            if (avl_tree_data._root == nullptr)
            {
                return;
            }

            // 使用单栈，存储源节点和目标父节点（均为一级指针）
            standard_con::stack<standard_con::pair<container_node *, container_node *>> stack;

            // 创建根节点
            _root = new container_node(avl_tree_data._root->_data);
            _root->_balance_factor = avl_tree_data._root->_balance_factor;
            _root->_parent = nullptr; // 根节点的父节点为nullptr

            // 初始化栈，将根节点的子节点压入（注意：这里父节点是 _ROOT，一级指针）
            if (avl_tree_data._root->_right != nullptr)
            {
                stack.push(standard_con::pair<container_node *, container_node *>(avl_tree_data._root->_right, _root));
            }
            if (avl_tree_data._root->_left != nullptr)
            {
                stack.push(standard_con::pair<container_node *, container_node *>(avl_tree_data._root->_left, _root));
            }

            // 遍历并复制剩余节点
            while (!stack.empty())
            {
                auto [first_node, second_node] = stack.top();
                stack.pop();

                // 创建新节点并复制数据
                auto *new_structure_node = new container_node(first_node->_data);
                new_structure_node->_balance_factor = first_node->_balance_factor;

                // 设置父节点关系（注意：second_node 是一级指针）
                new_structure_node->_parent = second_node;

                // 判断源节点在原树中是左子还是右子
                bool isleft_child = false;
                if (first_node->_parent != nullptr)
                {
                    isleft_child = (first_node->_parent->_left == first_node);
                }

                // 将新节点链接到父节点的正确位置（注意：直接使用 second_node）
                if (isleft_child)
                {
                    second_node->_left = new_structure_node;
                }
                else
                {
                    second_node->_right = new_structure_node;
                }

                // 处理子节点（注意：压栈时父节点是 new_structure_node，一级指针）
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
        balance_tree(balance_tree &&avl_tree_data) noexcept
                : _root(nullptr), function_policy(avl_tree_data.function_policy)
        {
            _root = std::move(avl_tree_data._root);
            avl_tree_data._root = nullptr;
        }
        balance_tree &operator=(balance_tree &&avl_tree_data) noexcept
        {
            if (this != &avl_tree_data)
            {
                clear();
                _root = std::move(avl_tree_data._root);
                function_policy = std::move(avl_tree_data.function_policy);
                avl_tree_data._root = nullptr;
            }
            return *this;
        }
        balance_tree &operator=(const balance_tree avl_tree_data)
        {
            clear();
            if (&avl_tree_data == this)
            {
                return *this;
            }
            if (avl_tree_data._root == nullptr)
            {
                return *this;
            }
            standard_con::algorithm::swap(function_policy, avl_tree_data.function_policy);
            standard_con::algorithm::swap(_root, avl_tree_data._root);
            return *this;
        }
        ~balance_tree() noexcept
        {
            clear();
        }

        [[nodiscard]] uint64_t size() const
        {
            return _size();
        }

        [[nodiscard]] uint64_t size()
        {
            return _size();
        }

        void pre_order_traversal()
        {
            interior_pre_order_traversal(_root);
        }

        void middle_order_traversal()
        {
            interior_middle_order_traversal(_root);
        }

        bool push(const avl_tree_type_k &key_data, const avl_tree_type_v &val_data = avl_tree_type_v())
        {
            // 插入
            if (_root == nullptr)
            {
                _root = new container_node(key_data, val_data);
                return true;
            }
            else
            {
                container_node *reference_node = _root;
                container_node *parent_node = nullptr;
                while (reference_node)
                {
                    parent_node = reference_node;
                    if (!function_policy(key_data, reference_node->_data.first) && !function_policy(reference_node->_data.first, key_data))
                    {
                        return false;
                    }
                    else if (function_policy(key_data, reference_node->_data.first))
                    {
                        reference_node = reference_node->_left;
                    }
                    else
                    {
                        reference_node = reference_node->_right;
                    }
                }
                reference_node = new container_node(key_data, val_data);
                if (function_policy(key_data, parent_node->_data.first))
                {
                    parent_node->_left = reference_node;
                }
                else
                {
                    parent_node->_right = reference_node;
                }
                reference_node->_parent = parent_node;

                container_node *adjust_reference_node = reference_node;
                container_node *adjust_reference_parent_node = parent_node;

                while (adjust_reference_parent_node)
                {
                    if (adjust_reference_parent_node->_left == adjust_reference_node)
                    {
                        --adjust_reference_parent_node->_balance_factor;
                    }
                    else
                    {
                        ++adjust_reference_parent_node->_balance_factor;
                    }

                    if (adjust_reference_parent_node->_balance_factor == 0)
                    {
                        break;
                    }
                    else if (adjust_reference_parent_node->_balance_factor == 1 || adjust_reference_parent_node->_balance_factor == -1)
                    {
                        adjust_reference_node = adjust_reference_parent_node;
                        adjust_reference_parent_node = adjust_reference_parent_node->_parent;
                    }
                    else if (adjust_reference_parent_node->_balance_factor == 2 || adjust_reference_parent_node->_balance_factor == -2)
                    {
                        if (adjust_reference_parent_node->_balance_factor == 2)
                        {
                            if (adjust_reference_node->_balance_factor == 1)
                            {
                                left_revolve(adjust_reference_parent_node);
                            }
                            else
                            {
                                right_left_revolve(adjust_reference_parent_node);
                            }
                        }
                        if (adjust_reference_parent_node->_balance_factor == -2)
                        {
                            if (adjust_reference_node->_balance_factor == -1)
                            {
                                right_revolve(adjust_reference_parent_node);
                            }
                            else
                            {
                                left_right_revolve(adjust_reference_parent_node);
                            }
                        }
                        adjust_reference_node = adjust_reference_parent_node;
                        adjust_reference_parent_node = adjust_reference_parent_node->_parent;
                    }
                }
            }
            return true;
        }
        bool push(const avl_tree_node_pair &pair_type_data)
        {
            // AVL树左子树比右子树高，则他俩的根节点的平衡因子为1，反之为-1，也就是说左加一，右减一，如果根节点为2和-2就要需要调整了
            if (_root == nullptr)
            {
                _root = new container_node(pair_type_data.first, pair_type_data.second);
                return true;
            }
            else
            {
                container_node *reference_node = _root;
                container_node *parent_node = nullptr;
                while (reference_node != nullptr)
                {
                    parent_node = reference_node;
                    // 找到first该在的节点
                    if (!function_policy(pair_type_data.first, reference_node->_data.first) && !function_policy(reference_node->_data.first, pair_type_data.first))
                    {
                        // 不允许重复插入
                        return false;
                    }
                    else if (function_policy(pair_type_data.first, reference_node->_data.first))
                    {
                        reference_node = reference_node->_left;
                    }
                    else
                    {
                        reference_node = reference_node->_right;
                    }
                }
                reference_node = new container_node(pair_type_data);
                if (function_policy(pair_type_data.first, parent_node->_data.first))
                {
                    parent_node->_left = reference_node;
                    // 三叉链表，注意父亲节点指向
                }
                else
                {
                    parent_node->_right = reference_node;
                }
                reference_node->_parent = parent_node;
                container_node *adjust_reference_node = reference_node;
                container_node *adjust_reference_parent_node = parent_node;
                // 更新平衡因子
                while (adjust_reference_parent_node)
                {
                    // 更新到根节点跳出
                    if (adjust_reference_node == adjust_reference_parent_node->_right)
                    {
                        ++adjust_reference_parent_node->_balance_factor;
                    }
                    else
                    {
                        --adjust_reference_parent_node->_balance_factor;
                    }

                    if (adjust_reference_parent_node->_balance_factor == 0)
                    {
                        // 平衡因子为0，无需平衡
                        break;
                    }
                    else if (adjust_reference_parent_node->_balance_factor == 1 || adjust_reference_parent_node->_balance_factor == -1)
                    {
                        adjust_reference_node = adjust_reference_parent_node;
                        adjust_reference_parent_node = adjust_reference_parent_node->_parent;
                        // 向上更新，直到找到0或-2或2
                    }
                    else if (adjust_reference_parent_node->_balance_factor == 2 || adjust_reference_parent_node->_balance_factor == -2)
                    {
                        // 平衡因子为2或者-2，需要平衡
                        if (adjust_reference_parent_node->_balance_factor == 2)
                        {
                            if (adjust_reference_node->_balance_factor == 1)
                            {
                                // L，说明_ROOT_Temp_test是_ROOT_Temp_test_parent的左子节点，线形
                                left_revolve(adjust_reference_parent_node);
                            }
                            else
                            {
                                // RL，证明_ROOT_Temp_test是_ROOT_Temp_test_parent的右子节点，在AVL树抽象图上就是折线型的
                                right_left_revolve(adjust_reference_parent_node);
                            }
                        }
                        else if (adjust_reference_parent_node->_balance_factor == -2)
                        {
                            if (adjust_reference_node->_balance_factor == -1)
                            {
                                // R，说明_ROOT_Temp_test是_ROOT_Temp_test_parent的右子节点，线形
                                right_revolve(adjust_reference_parent_node);
                            }
                            else
                            {
                                // LR，和上同理
                                left_right_revolve(adjust_reference_parent_node);
                            }
                        }
                        // 旋转后继续向上调整，因为旋转后父节点的平衡因子可能发生变化，每个旋转的节点都可以当作一个子树，子树旋转后，父节点平衡因子可能发生变化
                        adjust_reference_node = adjust_reference_parent_node;
                        adjust_reference_parent_node = adjust_reference_parent_node->_parent;
                        // 对于双旋的情况，相同方向先调整该节点，再调整整体
                    }
                }
            }
            return true;
        }
        container_node *find(const avl_tree_type_k &key_data)
        {
            container_node *reference_node = _root;
            while (reference_node != nullptr)
            {
                if (reference_node->_data.first == key_data)
                {
                    break;
                }
                else if (function_policy(reference_node->_data.first, key_data))
                {
                    reference_node = reference_node->_right;
                }
                else
                {
                    reference_node = reference_node->_left;
                }
            }
            return reference_node;
        }
        balance_tree &pop(const avl_tree_type_k &key_data)
        {
            if (_root == nullptr)
            {
                return *this;
            }
            container_node *reference_node = _root;
            container_node *parent_node = nullptr;

            // 查找要删除的节点
            while (reference_node != nullptr)
            {
                if (!function_policy(key_data, reference_node->_data.first) && !function_policy(reference_node->_data.first, key_data))
                {
                    break;
                }
                parent_node = reference_node;
                if (function_policy(reference_node->_data.first, key_data))
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
                return *this; // 没有找到
            }

            // 记录被删除节点是其父节点的左子树还是右子树
            bool isleft_child = (parent_node != nullptr) && (parent_node->_left == reference_node);

            // 三种情况：左空，右空，左右都不空
            if (reference_node->_left == nullptr)
            {
                container_node *child = reference_node->_right;
                if (child != nullptr)
                {
                    child->_parent = parent_node;
                }

                if (parent_node == nullptr)
                {
                    _root = child;
                }
                else
                {
                    if (isleft_child)
                    {
                        parent_node->_left = child;
                    }
                    else
                    {
                        parent_node->_right = child;
                    }
                }
                delete reference_node;
                reference_node = nullptr;
            }
            else if (reference_node->_right == nullptr)
            {
                container_node *child = reference_node->_left;
                if (child != nullptr)
                {
                    child->_parent = parent_node;
                }

                if (parent_node == nullptr)
                {
                    _root = child;
                }
                else
                {
                    if (isleft_child)
                    {
                        parent_node->_left = child;
                    }
                    else
                    {
                        parent_node->_right = child;
                    }
                }
                delete reference_node;
                reference_node = nullptr;
            }
            else // 左右子树都不为空
            {
                // 找右子树最左节点
                container_node *right_subtree_smallest_node = reference_node->_right;
                container_node *smallest_parent_node = reference_node;

                while (right_subtree_smallest_node->_left != nullptr)
                {
                    smallest_parent_node = right_subtree_smallest_node;
                    right_subtree_smallest_node = right_subtree_smallest_node->_left;
                }

                // 交换数据
                standard_con::algorithm::swap(right_subtree_smallest_node->_data, reference_node->_data);

                // 更新要删除的节点信息
                reference_node = right_subtree_smallest_node;
                parent_node = smallest_parent_node;
                isleft_child = (parent_node->_left == reference_node);

                // 删除右子树最左节点
                container_node *child = reference_node->_right;
                if (child != nullptr)
                {
                    child->_parent = parent_node;
                }

                if (parent_node->_left == reference_node)
                {
                    parent_node->_left = child;
                }
                else
                {
                    parent_node->_right = child;
                }

                delete reference_node;
                reference_node = nullptr;
            }

            // 更新平衡因子
            container_node *current = parent_node;
            while (current != nullptr)
            {
                // 根据之前记录的子树关系更新平衡因子
                if (isleft_child)
                {
                    ++current->_balance_factor;
                }
                else
                {
                    --current->_balance_factor;
                }

                // 平衡因子调整逻辑
                if (current->_balance_factor == 0)
                {
                    // 高度未变，不需要继续调整
                    break;
                }
                else if (current->_balance_factor == 1 || current->_balance_factor == -1)
                {
                    // 高度变化，但不需要旋转，继续向上调整
                    isleft_child = (current->_parent != nullptr) && (current->_parent->_left == current);
                    current = current->_parent;
                }
                else if (current->_balance_factor == 2 || current->_balance_factor == -2)
                {
                    // 需要旋转调整
                    if (current->_balance_factor == 2)
                    {
                        container_node *child = current->_right;
                        if (child->_balance_factor == 1)
                        {
                            left_revolve(current);
                        }
                        else
                        {
                            right_left_revolve(current);
                        }
                    }
                    else // current->_balance_factor == -2
                    {
                        container_node *child = current->_left;
                        if (child->_balance_factor == -1)
                        {
                            right_revolve(current);
                        }
                        else
                        {
                            left_right_revolve(current);
                        }
                    }

                    // 旋转后继续向上调整
                    isleft_child = (current->_parent != nullptr) && (current->_parent->_left == current);
                    current = current->_parent;
                }
            }
            return *this;
        }
    };
}
namespace standard_con
{
    using tree_container::balance_tree;
    using tree_container::binary_tree;
}