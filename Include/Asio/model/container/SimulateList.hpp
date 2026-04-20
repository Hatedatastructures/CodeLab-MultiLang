#pragma once
#include "SimulateException.hpp"
#include "SimulateAlgorithm.hpp"
namespace list_container
{
    /*
            * @brief  #### `list` 类模板

            *   - 自定义双向链表容器类，类似 `std::list` 的功能实现
            *   - 支持高效的节点插入、删除操作，元素双向遍历
            *   - 采用哨兵节点设计，简化边界条件处理

            * 模板参数:

            * * - `list_type`: 链表中存储的元素类型

            * 内部结构:

            * * - `list_container_node`: 链表节点结构体
            *   - 包含前驱指针 `_prev`、后继指针 `_next` 和元素数据 `_data`
            *   - 支持拷贝构造和移动构造初始化
            *
            * * - `list_iterator`: 链表迭代器类
            *   - 支持解引用（`*`）、成员访问（`->`）、自增（`++`）、自减（`--`）等操作
            *   - 区分普通迭代器和常量迭代器（通过模板参数 `Ref` 和 `Ptr` 控制）
            *
            * * - `reverse_list_iterator`: 反向迭代器类
            *   - 基于正向迭代器实现，通过反向移动模拟正向遍历
            *   - 重载 `*`、`->`、`++`、`--` 等操作，行为与正向迭代器相反

            * 成员变量:

            * * - `_head`: 哨兵节点指针，作为链表的头节点（不存储实际数据）
            *   - 初始化时 `_prev` 和 `_next` 均指向自身，形成循环结构

            * 迭代器相关方法:

            * * - `begin()`: 返回指向第一个元素的迭代器
            *
            * * - `end()`: 返回指向哨兵节点的迭代器（表示末尾的下一个位置）
            *
            * * - `cbegin()`: 返回指向第一个元素的常量迭代器
            *
            * * - `cend()`: 返回指向哨兵节点的常量迭代器
            *
            * * - `rbegin()`: 返回指向最后一个元素的反向迭代器
            *
            * * - `rend()`: 返回指向哨兵节点的反向迭代器
            *
            * * - `rcbegin()`: 返回指向最后一个元素的常量反向迭代器
            *
            * * - `rcend()`: 返回指向哨兵节点的常量反向迭代器

            * 容量相关方法:

            * * - `size()`: 返回链表中元素的数量（遍历计数实现）
            *
            * * - `empty()`: 判断链表是否为空（哨兵节点的后继是否指向自身）
            *
            * * - `resize()`: 调整链表长度，不足时填充指定元素，超出时截断

            * 元素访问方法:

            * * - `front()`: 返回第一个元素的引用（支持读写和只读版本）
            *
            * * - `back()`: 返回最后一个元素的引用（支持读写和只读版本）

            * 构造函数:

            * * - 默认构造函数: 创建哨兵节点，初始化空链表
            *
            * * - 迭代器范围构造: 通过迭代器范围 `[first, last)` 初始化链表
            *
            * * - 初始化列表构造: 从元素初始化列表构造链表
            *
            * * - 拷贝构造函数: 深拷贝另一个链表的所有元素
            *
            * * - 移动构造函数: 接管另一个临时链表的资源，原链表重置为新哨兵节点

            * 析构函数:

            * * - 清空所有元素（调用 `clear()`），释放哨兵节点内存

            * 元素修改方法:

            * * - `push_back()`: 在链表末尾插入元素（支持拷贝和移动语义）
            *
            * * - `push_front()`: 在链表头部插入元素（支持拷贝和移动语义）
            *
            * * - `pop_back()`: 删除链表末尾的元素
            *
            * * - `pop_front()`: 删除链表头部的元素，返回下一个元素的迭代器
            *
            * * - `insert()`: 在指定迭代器位置插入元素（支持拷贝和移动语义），返回新元素的迭代器
            *
            * * - `erase()`: 删除指定迭代器位置的元素，返回下一个元素的迭代器
            *
            * * - `clear()`: 清空链表所有元素，仅保留哨兵节点
            *
            * * - `swap()`: 与另一个链表交换内部资源（哨兵节点指针）

            * 运算符重载:

            * * - `operator=`: 赋值运算符（支持拷贝赋值、初始化列表赋值、移动赋值）
            *
            * * - `operator+`: 链表拼接，返回包含两个链表所有元素的新链表
            *
            * * - `operator+=`: 链表拼接，将另一个链表的元素添加到当前链表末尾
            *
            * * - 友元 `operator<<`: 输出链表所有元素到流（元素间用空格分隔）

            * 特性:

            * * - 高效操作: 插入和删除操作时间复杂度为 O(1)（已知位置时）
            *
            * * - 双向遍历: 支持正向和反向迭代器，可双向遍历元素
            *
            * * - 迭代器稳定性: 插入操作不会导致迭代器失效，删除操作仅使被删除元素的迭代器失效
            *
            * * - 异常处理: 空迭代器插入、内存分配失败等情况会抛出 `fault` 异常
            *
            * * - 支持移动语义: 减少不必要的元素拷贝，提高性能

            * 注意事项:

            * * - 随机访问效率低: 访问第 n 个元素需遍历，时间复杂度为 O(n)
            *
            * * - `size()` 方法通过遍历实现，时间复杂度为 O(n)（非缓存实现）
            *
            * * - 移动构造/赋值后，原链表会被重置为新的空链表（包含哨兵节点）
            *
            * * - 迭代器不可直接进行算术运算（如 `it + 2`），仅支持自增/自减
            *
            * * - 哨兵节点不存储实际数据，`end()` 迭代器指向哨兵节点

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename list_type>
    class list
    {
        template <typename list_type_function_node>
        struct list_container_node
        {
            // 节点类
            list_container_node<list_type_function_node> *_prev;
            list_container_node<list_type_function_node> *_next;
            list_type_function_node _data;

            explicit list_container_node(const list_type_function_node &list_type_data = list_type_function_node())
                    : _prev(nullptr), _next(nullptr), _data(list_type_data)
            {
                // 列表初始化
            }
            explicit list_container_node(const list_type_function_node &&data)
                    : _prev(nullptr), _next(nullptr)
            {
                _data = data;
            }
        };
        template <typename list_node_type_iterator, typename Ref, typename Ptr>
        class list_iterator
        {
        public:
            // 迭代器类
            using container_node = list_container_node<list_node_type_iterator>;
            using iterator = list_iterator<list_node_type_iterator, list_node_type_iterator &, list_node_type_iterator *>;
            using reference = Ref;
            using pointer = Ptr;
            container_node *_node;
            list_iterator(container_node *node) noexcept
                    : _node(node)
            {
                ; // 拿一个指针来构造迭代器
            }
            Ref operator*() noexcept
            {
                // 返回该节点的自定义类型的数据
                return _node->_data;
            }
            list_iterator &operator++() noexcept
            {
                // 先加在用
                _node = _node->_next;
                return *this;
                // 返回类型名，如果为迭代器就会因为const 报错
            }
            list_iterator operator++(int) noexcept
            {
                // 先用在加
                list_iterator return_self(_node);
                _node = _node->_next;
                // 把本体指向下一个位置
                return return_self;
            }
            list_iterator &operator--() noexcept
            {
                _node = _node->_prev;
                return *this;
            }
            list_iterator operator--(int) noexcept
            {
                list_iterator return_self(_node);
                _node = _node->_prev;
                return return_self;
            }
            bool operator!=(const list_iterator &IteratorTemp) noexcept
            {
                // 比较两个指针及其上一个和下一个指针地址
                return _node != IteratorTemp._node;
            }
            Ptr operator->() noexcept
            {
                return &(_node->_data);
            }
        };
        template <typename iterator>
        class reverse_list_iterator
        {
            // 创建反向迭代器
            using Ref = typename iterator::reference;
            using Ptr = typename iterator::pointer;
            using const_reverse_list_iterator = reverse_list_iterator<iterator>;

        public:
            iterator _it;
            reverse_list_iterator(iterator it) noexcept
                    : _it(it)
            {
                ;
            }
            Ref &operator*() noexcept
            {
                // 因为反向迭代器起始位置在哨兵节点所以通过指向上一个来找到准确位置
                // 正好到rend位置停下来的时候已经遍历到rend位置
                iterator return_self(_it);
                --(return_self);
                return *return_self;
            }
            Ptr operator->() noexcept
            {
                // 两者函数差不多可直接调用
                return &(operator*());
            }
            reverse_list_iterator &operator++() noexcept
            {
                --_it;
                return *this;
            }
            reverse_list_iterator operator++(int) noexcept
            {
                reverse_list_iterator _temp(_it);
                --_it;
                return _temp;
            }
            reverse_list_iterator &operator--() noexcept
            {
                ++_it;
                return *this;
            }
            reverse_list_iterator operator--(int) noexcept
            {
                reverse_list_iterator _temp(_it);
                ++_it;
                return _temp;
            }
            bool operator!=(const const_reverse_list_iterator &Temp) noexcept
            {
                return _it != Temp._it;
            }
        };
        using container_node = list_container_node<list_type>;

        container_node *_head;
        //_head为哨兵位
        void create_head()
        {
            try
            {
                _head = new container_node;
                _head->_prev = _head;
                _head->_next = _head;
            }
            catch (const std::bad_alloc &process)
            {
                _head = nullptr;
                std::cerr << process.what() << std::endl;
                throw;
            }
        }

    public:
        using iterator = list_iterator<list_type, list_type &, list_type *>;
        using const_iterator = list_iterator<list_type, const list_type &, const list_type *>;

        // 拿正向迭代器构造反向迭代器，可以直接调用 iterator 已经重载的运算符和函数，相当于在封装一层类
        using reverse_iterator = reverse_list_iterator<iterator>;
        using reverse_const_iterator = reverse_list_iterator<const_iterator>;
        list() { create_head(); }
        ~list() noexcept
        {
            clear();
            delete _head;
            _head = nullptr;
        }
        list(iterator first, iterator last)
        {
            try
            {
                if (first._node == nullptr || last._node == nullptr)
                {
                    throw custom_exception::fault("传入迭代器参数为空", "list::list", __LINE__);
                }
                if (first == last)
                {
                    throw custom_exception::fault("传入迭代器参函数相同", "list::list", __LINE__);
                }
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
            create_head(); // 通过另一个list对象构建一个list
            // 已经创建一个哨兵节点
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }
        list(std::initializer_list<list_type> lightweight_container)
        {
            // 通过初始化列表构建一个list
            create_head();
            for (auto &chained_values : lightweight_container)
            {
                push_back(std::move(chained_values));
            }
        }
        list(const_iterator first, const_iterator last)
        {
            create_head();
            // 已经创建一个哨兵节点
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }
        list(const list<list_type> &list_data)
        {
            create_head();
            list<list_type> Temp(list_data.cbegin(), list_data.cend());
            swap(Temp);
        }
        list(list<list_type> &&list_data) noexcept
        {
            create_head(); // 移动构造
            _head = std::move(list_data._head);
            list_data._head = nullptr;
        }
        void swap(list_container::list<list_type> &swap_target) noexcept
        {
            standard_con::algorithm::swap(_head, swap_target._head);
        }
        [[nodiscard]] iterator begin() noexcept
        {
            return iterator(_head->_next);
        }

        [[nodiscard]] iterator end() noexcept
        {
            return iterator(_head);
        }

        [[nodiscard]] const_iterator cbegin() const noexcept
        {
            return const_iterator(_head->_next);
        }

        [[nodiscard]] const_iterator cend() const noexcept
        {
            return const_iterator(_head);
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return _head->_next == _head;
        }

        [[nodiscard]] reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(_head->_prev);
        }

        [[nodiscard]] reverse_iterator rend() noexcept
        {
            return reverse_iterator(_head);
        }

        [[nodiscard]] reverse_const_iterator rcbegin() const noexcept
        {
            return reverse_const_iterator(cend());
        }

        [[nodiscard]] reverse_const_iterator rcend() const noexcept
        {
            return reverse_const_iterator(cbegin());
        }

        [[nodiscard]] uint64_t size() const noexcept
        {
            container_node *current_node = _head->_next;
            uint64_t count = 0;
            while (current_node != _head)
            {
                count++;
                current_node = current_node->_next;
            }
            return count;
        }
        /*
        元素访问操作
        */
        const list_type &front() const noexcept
        {
            return _head->_next->_data;
        }

        const list_type &back() const noexcept
        {
            return _head->_prev->_data;
        }
        list_type &front() noexcept
        {
            return _head->_next->_data;
        }

        list_type &back() noexcept
        {
            return _head->_prev->_data;
        }
        /*          插入删除操作          */
        void push_back(const list_type &push_back_data)
        {
            insert(end(), push_back_data);
        }

        void push_front(const list_type &push_front_data)
        {
            insert(begin(), push_front_data);
        }

        void push_back(list_type &&push_back_data)
        {
            insert(end(), std::forward<list_type>(push_back_data));
        }

        void push_front(list_type &&push_front_data)
        {
            insert(begin(), std::forward<list_type>(push_front_data));
        }

        void pop_back()
        {
            erase(--end());
        }

        iterator pop_front()
        {
            return erase(begin());
        }

        iterator insert(iterator iterator_position, const list_type &list_type_data)
        {
            try
            {
                if (iterator_position._node == nullptr)
                {
                    throw custom_exception::fault("传入迭代器参数为空", "list::insert", __LINE__);
                }
                auto *new_container_node(new container_node(list_type_data));
                // 开辟新节点
                container_node *iterator_current_node = iterator_position._node;
                // 保存pos位置的值
                new_container_node->_prev = iterator_current_node->_prev;
                new_container_node->_next = iterator_current_node;
                new_container_node->_prev->_next = new_container_node;
                iterator_current_node->_prev = new_container_node;
                return iterator(new_container_node);
            }
            catch (const std::bad_alloc &process)
            {
                std::cerr << process.what() << std::endl;
                throw;
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        iterator insert(iterator iterator_position, list_type &&list_type_data)
        {
            try
            {
                if (iterator_position._node == nullptr)
                {
                    throw custom_exception::fault("传入迭代器参数为空", "list::insert移动语义版本", __LINE__);
                }
                auto *new_container_node = new container_node(std::forward<list_type>(list_type_data));
                container_node *iterator_current_node = iterator_position._node;
                new_container_node->_prev = iterator_current_node->_prev;
                new_container_node->_next = iterator_current_node;
                new_container_node->_prev->_next = new_container_node;
                iterator_current_node->_prev = new_container_node;
                return iterator(new_container_node);
            }
            catch (const std::bad_alloc &process)
            {
                std::cerr << process.what() << "插入时内存不足" << std::endl;
                throw;
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        iterator erase(iterator iterator_position)
        {
            try
            {
                if (iterator_position._node == nullptr)
                {
                    throw custom_exception::fault("传入迭代器参数为空", "list::erase", __LINE__);
                }
                container_node *iterator_delete_node = iterator_position._node;  // 找到待删除的节点
                container_node *next_element_node = iterator_delete_node->_next; // 保存下一个节点的位置

                iterator_delete_node->_prev->_next = iterator_delete_node->_next; // 将该节点从链表中拆下来并删除
                iterator_delete_node->_next->_prev = iterator_delete_node->_prev;
                delete iterator_delete_node;

                return iterator(next_element_node);
            }
            catch (const custom_exception::fault &process)
            {
                std::cerr << process.what() << " " << process.function_name_get() << " " << process.line_number_get() << std::endl;
                throw;
            }
        }
        void resize(const uint64_t new_container_size, const list_type &list_type_data = list_type())
        {
            if (uint64_t container_size = size(); new_container_size <= container_size)
            {
                // 有效元素个数减少到new_container_size
                while (new_container_size < container_size)
                {
                    pop_back();
                    --container_size;
                }
            }
            else
            {
                while (container_size < new_container_size)
                {
                    push_back(list_type_data);
                    ++container_size;
                }
            }
        }
        void clear() noexcept
        {
            // 循环释放资源
            container_node *current_node = _head->_next;
            // 采用头删除
            while (current_node != _head)
            {
                _head->_next = current_node->_next;
                delete current_node;
                current_node = _head->_next;
            }
            _head->_next = _head->_prev = _head;
        }
        list &operator=(const list<list_type> &list_data) noexcept
        {
            // 拷贝赋值
            if (this != &list_data)
            {
                list<list_type> copy_list_object(list_data);
                swap(copy_list_object);
            }
            return *this;
        }
        list &operator=(std::initializer_list<list_type> lightweight_container)
        {
            clear();
            for (auto &chained_values : lightweight_container)
            {
                push_back(std::move(chained_values));
            }
            return *this;
        }
        list &operator=(list<list_type> &&list_data) noexcept
        {
            if (this != &list_data)
            {
                _head = std::move(list_data._head);
                list_data.create_head();
                // 防止移动之后类判空空指针
            }
            return *this;
        }
        list operator+(const list<list_type> &list_data)
        {
            list<list_type> return_list_object(cbegin(), cend());
            const_iterator start_position_iterator = list_data.cbegin();
            const_iterator end_position_iterator = list_data.cend();
            while (start_position_iterator != end_position_iterator)
            {
                return_list_object.push_back(*start_position_iterator);
                ++start_position_iterator;
            }
            return return_list_object;
        }
        list &operator+=(const list<list_type> &list_data)
        {
            const_iterator start_position_iterator = list_data.cbegin();
            const_iterator end_position_iterator = list_data.cend();
            while (start_position_iterator != end_position_iterator)
            {
                push_back(*start_position_iterator);
                ++start_position_iterator;
            }
            return *this;
        }
        template <typename const_list_output_templates>
        friend std::ostream &operator<<(std::ostream &list_ostream, const list<const_list_output_templates> &dynamic_arrays_data);
    };
    template <typename const_list_output_templates>
    std::ostream &operator<<(std::ostream &list_ostream, const list<const_list_output_templates> &dynamic_arrays_data)
    {
        typename list<const_list_output_templates>::const_iterator it = dynamic_arrays_data.cbegin();
        while (it != dynamic_arrays_data.cend())
        {
            list_ostream << *it << " ";
            ++it;
        }
        return list_ostream;
    }
}
namespace standard_con
{
    using list_container::list;
}