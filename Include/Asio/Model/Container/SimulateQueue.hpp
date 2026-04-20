#pragma once
#include "SimulateList.hpp"
#include "SimulateVector.hpp"
#include "SimulateAlgorithm.hpp"
#include "SimulateException.hpp"
#include "SimulateImitate.hpp"
namespace queue_adapter
{
    /*
   * @brief  #### `queue` 类模板

    *   - 自定义队列容器适配器，遵循 FIFO（先进先出）原则

    *   - 基于底层容器实现（默认为 `standard_con::list`），提供队列的标准操作接口

   * 模板参数:

   * * - `queue_type`: 队列中存储的元素类型
   *
   * * - `list_based_queue`: 底层容器类型，默认为 `standard_con::list<queue_type>`
   *
   *   - 需支持 `push_back()`、`pop_front()`、`front()`、`back()`、`size()`、`empty()` 等操作

   * 构造函数:

   * * - 默认构造函数: 创建空队列
   *
   * * - 拷贝构造函数: 深拷贝另一个队列的所有元素
   *
   * * - 移动构造函数: 接管另一个临时队列的资源
   *
   * * - 初始化列表构造: 从元素初始化列表构造队列
   *
   * * - 单元素构造: 用单个元素初始化队列

   * 析构函数:

   * * - 默认行为，依赖底层容器的析构函数释放资源

   * 元素操作方法:

   * * - `push()`: 入队操作（支持拷贝和移动语义）
   *
   * * - `pop()`: 出队操作（移除队首元素，不返回值）
   *
   * * - `front()`: 返回队首元素的引用（非常量版本）
   *
   * * - `back()`: 返回队尾元素的引用（非常量版本）

   * 容量相关方法:

   * * - `size()`: 返回队列中元素的数量
   *
   * * - `empty()`: 判断队列是否为空

   * 赋值运算符:

   * * - `operator=`: 赋值运算符（支持拷贝赋值和移动赋值）

   * 特性:

   * * - 适配性: 可基于不同底层容器实现（如 list、deque 等）
   *
   * * - 移动语义: 支持高效的资源转移
   *
   * * - 异常安全: 依赖底层容器的异常安全保证

   * 注意事项:

   * * - `pop()` 不返回值: 如需获取队首元素，需先调用 `front()` 再调用 `pop()`
   *
   * * - 空队列操作: 在队列为空时调用 `front()`、`back()` 或 `pop()` 会返回一个类型默认构造的值

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
*/
    template <typename queue_type, typename list_based_queue = standard_con::list<queue_type>>
    class queue
    {
        list_based_queue list_object;

    public:
        ~queue() { ; }

        void push(queue_type &&queue_type_data)
        {
            list_object.push_back(std::forward<queue_type>(queue_type_data));
        }

        void push(const queue_type &queue_type_data)
        {
            list_object.push_back(queue_type_data);
        }

        void pop()
        {
            list_object.pop_front();
        }

        [[nodiscard]] uint64_t size() const noexcept
        {
            return list_object.size();
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return list_object.empty();
        }

        [[nodiscard]] queue_type &front() noexcept
        {
            return list_object.front();
        }

        [[nodiscard]] queue_type &back() noexcept
        {
            return list_object.back();
        }

        explicit queue(const queue<queue_type> &queue_data)
        {
            list_object = queue_data.list_object;
        }

        queue(queue<queue_type> &&queue_type_data) noexcept
        {
            // 移动构造
            list_object = std::forward<list_based_queue>(queue_type_data.list_object);
        }
        queue(std::initializer_list<queue_type> queue_type_data)
        {
            // 链式构造
            for (auto &chained_values : queue_type_data)
            {
                list_object.push_back(std::move(chained_values));
            }
        }
        explicit queue(const queue_type &queue_type_data)
        {
            list_object.push_back(queue_type_data);
        }
        queue() = default;
        queue &operator=(const queue<queue_type> &queue_data)
        {
            if (this != &queue_data)
            {
                list_object = queue_data.list_object;
            }
            return *this;
        }
        queue &operator=(queue<queue_type> &&queue_data) noexcept
        {
            if (this != &queue_data)
            {
                list_object = std::forward<list_based_queue>(queue_data.list_object);
            }
            return *this;
        }
    };
    /*
            * @brief  #### `priority_queue` 类模板

            *   - 自定义优先队列容器适配器，基于堆结构实现

            *   - 元素按优先级排序，优先级最高的元素始终位于队首

            *   - 提供堆的标准操作接口（插入、删除堆顶元素）

            * 模板参数:

            * * - `priority_queue_type`: 队列中存储的元素类型
            *
            * * - `container_imitate_function`: 比较器类型，默认为 `standard_con::imitation_functions::less<priority_queue_type>`
            *      用于定义元素优先级，返回 `true` 表示左操作数优先级低于右操作数，默认为大顶堆（优先级高的元素值大），可通过传入 `greater` 改为小顶堆
            *
            * * - `vector_based_priority_queue`: 底层容器类型，默认为 `standard_con::vector<priority_queue_type>`
            *
            * * *   - 需支持随机访问、尾部插入删除等操作

            * 构造函数:

            * * - 默认构造函数: 创建空优先队列
            *
            * * - 初始化列表构造: 从元素初始化列表构造优先队列
            *
            * * - 拷贝构造函数: 深拷贝另一个优先队列的所有元素和比较器
            *
            * * - 移动构造函数: 接管另一个临时优先队列的资源
            *
            * * - 单元素构造: 用单个元素初始化优先队列

            * 析构函数:

            * * - 显式调用底层容器的析构函数

            * 元素操作方法:

            * * - `push()`: 插入元素到优先队列，自动维护堆结构
            *
            * * - `pop()`: 删除堆顶元素（优先级最高的元素），自动维护堆结构
            *
            * * - `top()`: 返回堆顶元素的引用（优先级最高的元素）

            * 容量相关方法:

            * * - `empty()`: 判断队列是否为空
            *
            * * - `size()`: 返回队列中元素的数量

            * 赋值运算符:

            * * - `operator=`: 赋值运算符（支持拷贝赋值和移动赋值）

            * 特性:

            * * - 堆结构: 基于完全二叉树实现，确保 O(log n) 的插入和删除操作复杂度
            *
            * * - 可定制性: 通过自定义比较器实现不同的优先级规则
            *
            * * - 适配性: 可基于不同底层容器实现（如 vector、deque 等）
            *
            * * - 移动语义: 支持高效的资源转移

            * 注意事项:

            * * - `pop()` 不返回值: 如需获取堆顶元素，需先调用 `top()` 再调用 `pop()`
            *
            * * - 空队列操作: 在队列为空时调用 `top()` 或 `pop()` 会导致未定义行为
            *
            * * - 元素优先级: 比较器的选择直接影响元素的优先级顺序
            *
            *   - 使用 `less` 实现大顶堆（最大值优先）
            *
            *   - 使用 `greater` 实现小顶堆（最小值优先）

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md
    */
    template <typename priority_queue_type, typename container_imitate_function = standard_con::less<priority_queue_type>,
                        typename vector_based_priority_queue = standard_con::vector<priority_queue_type>>
    class priority_queue
    {
        // 创建容器对象
        vector_based_priority_queue vector_container_object;
        container_imitate_function function_policy; // 仿函数：数据类型比较器，可自定义
        // 仿函数对象

        void priority_queue_adjust_upwards(int adjust_upwards_child) noexcept
        {
            // 向上调整算法
            int adjust_upwards_parent = (adjust_upwards_child - 1) / 2;
            while (adjust_upwards_child > 0)
            {
                if (function_policy(vector_container_object[adjust_upwards_parent], vector_container_object[adjust_upwards_child]))
                {
                    standard_con::algorithm::swap(vector_container_object[adjust_upwards_parent], vector_container_object[adjust_upwards_child]);
                    adjust_upwards_child = adjust_upwards_parent;
                    adjust_upwards_parent = (adjust_upwards_child - 1) / 2;
                }
                else
                {
                    break;
                }
            }
        }
        void priority_queue_adjust_downwards(int adjust_downwards_parent = 0) noexcept
        {
            int adjust_downwards_child = (adjust_downwards_parent * 2) + 1;
            while (adjust_downwards_child < static_cast<int>(vector_container_object.size()))
            {
                int adjust_downwards_left = adjust_downwards_child;
                int adjust_downwards_right = adjust_downwards_left + 1;
                if (adjust_downwards_right < static_cast<int>(vector_container_object.size()) &&
                        function_policy(vector_container_object[adjust_downwards_left], vector_container_object[adjust_downwards_right]))
                {
                    // 大堆找出左右节点哪个孩子大
                    adjust_downwards_child = adjust_downwards_right;
                }
                if (function_policy(vector_container_object[adjust_downwards_parent], vector_container_object[adjust_downwards_child]))
                {
                    // 建大堆把小的换下去，建小堆把大的换下去
                    standard_con::algorithm::swap(vector_container_object[adjust_downwards_parent], vector_container_object[adjust_downwards_child]);

                    // 换完之后如果是大堆，则父亲节点是较大的值，需要更新孩子节点继续向下找比孩子节点大的值，如果有继续交换
                    adjust_downwards_parent = adjust_downwards_child;
                    adjust_downwards_child = (adjust_downwards_parent * 2) + 1;
                }
                else
                {
                    break;
                }
            }
        }

    public:
        ~priority_queue() noexcept
        {
            vector_container_object.~vector();
        }
        void push(const priority_queue_type &prioity_queue_type_data)
        {
            vector_container_object.push_back(prioity_queue_type_data);
            priority_queue_adjust_upwards(static_cast<int>(vector_container_object.size() - 1));
        }
        priority_queue_type &top() noexcept
        {
            return vector_container_object.front();
        }
        bool empty() noexcept
        {
            return vector_container_object.empty();
        }
        uint64_t size() noexcept
        {
            return vector_container_object.size();
        }
        void pop()
        {
            standard_con::algorithm::swap(vector_container_object[0], vector_container_object[vector_container_object.size() - static_cast<uint64_t>(1)]);
            vector_container_object.pop_back();
            priority_queue_adjust_downwards();
        }
        priority_queue()
        {
            ;
        }
        priority_queue(std::initializer_list<priority_queue_type> lightweight_container)
        {
            // 通过初始化列表构建一个list
            for (auto &chained_values : lightweight_container)
            {
                push(std::move(chained_values));
            }
        }
        priority_queue(const priority_queue &priority_queue_data)
        {
            // 拷贝构造
            vector_container_object = priority_queue_data.vector_container_object;
        }
        priority_queue(priority_queue &&priority_queue_data) noexcept
                : function_policy(priority_queue_data.function_policy)
        {
            // 移动构造
            vector_container_object = std::move(priority_queue_data.vector_container_object);
        }
        explicit priority_queue(const priority_queue_type &priority_queue_type_data)
        {
            vector_container_object.push_back(priority_queue_type_data);
            priority_queue_adjust_upwards((static_cast<int>(vector_container_object.size() - 1)));
        }
        priority_queue &operator=(priority_queue &&priority_queue_data) noexcept
        {
            // 移动赋值
            if (this != &priority_queue_data)
            {
                vector_container_object = std::move(priority_queue_data.vector_container_object);
                function_policy = priority_queue_data.function_policy;
            }
            return *this;
        }
        priority_queue &operator=(const priority_queue &priority_queue_data)
        {
            // 拷贝赋值
            if (this != &priority_queue_data)
            {
                vector_container_object = priority_queue_data.vector_container_object;
                function_policy = priority_queue_data.function_policy;
            }
            return *this;
        }
    };
}
namespace standard_con
{
    using queue_adapter::priority_queue;
    using queue_adapter::queue;
}