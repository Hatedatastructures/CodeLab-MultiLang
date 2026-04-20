#pragma once
#include <memory>
#include "SimulateException.hpp"
namespace standard_con
{
    /*
    * @brief  #### `pointer`智能指针命名空间

    *   ##### 智能指针模板类

    *   - `smart_ptr`: 管理动态分配的资源，确保资源自动释放

    *   - `shared_ptr`: 共享所有权的智能指针，允许多个智能指针共享同一资源

    *   - `unique_ptr`: 独占所有权的智能指针，确保资源仅由一个智能指针管理

    *   - `weak_ptr`: 弱引用的智能指针，不拥有资源所有权，用于避免循环引用
    */
    namespace pointer
    {
    }
}

namespace standard_con::pointer
{
    /*
   * @brief  #### `smart_ptr` 类

    *   - 智能指针，管理动态分配的资源，确保资源自动释放

    * 模板参数:

            - `smart_ptr_type`: 管理的对象类型

            - `deleter`: 定制删除器类型，默认为 `std::default_delete<smart_ptr_type>`

   * 构造函数:

   * * - 默认构造函数: 创建空智能指针

   * * - `explicit smart_ptr(const smart_ptr_type* ptr)`: 从原始指针构造，禁用隐式转换

   * 禁用的操作:

        - 拷贝构造、拷贝赋值: 不允许共享资源所有权

        - 移动赋值(右值引用赋值): 仅支持移动构造转移资源所有权

   * 提供的操作符:

   * * - `operator->()`: 访问管理对象的成员

   * * - `operator*()`: 解引用管理的对象

   * 资源管理:

   * * - 使用指定的删除器自动释放资源

   * * - 移动构造后原对象置空，避免双重释放

   * * - 析构时自动调用删除器释放资源

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

   * 线程安全性:

   * * - 不保证多线程环境下的安全性，操作时需外部同步

   * 注意事项:

   * * - 不要用同一个原始指针创建多个智能指针实例

   * * - 管理数组时需使用自定义删除器
    */
    template <typename smart_ptr_type, typename deleter = std::default_delete<smart_ptr_type>>
    class smart_ptr
    {
    private:
        smart_ptr_type *_ptr;
        deleter _deleter;
        using Ref = smart_ptr_type &;
        using Ptr = smart_ptr_type *;

    public:
        explicit smart_ptr(smart_ptr_type *ptr = nullptr)
                : _deleter(deleter()) { _ptr = ptr; }
        smart_ptr(const smart_ptr &) = delete;
        smart_ptr &operator=(const smart_ptr *&smart_ptr_data) = delete;
        smart_ptr(smart_ptr &&smart_ptr_data) noexcept
        {
            _ptr = smart_ptr_data._ptr;
            smart_ptr_data._ptr = nullptr;
        }
        smart_ptr &operator=(smart_ptr &&smart_ptr_data) noexcept
        {
            if (&smart_ptr_data != this)
            {
                _ptr = smart_ptr_data._ptr;
                smart_ptr_data._ptr = nullptr;
            }
            return *this;
        }
        ~smart_ptr() noexcept
        {
            if (_ptr != nullptr)
            {
                _deleter(_ptr);
                _ptr = nullptr;
            }
        }
        Ptr operator->() const noexcept
        {
            return _ptr;
        }
        Ref operator*() noexcept
        {
            return *(_ptr);
        }
    };
    /*
   * @brief  #### `unique_ptr` 类

    *   - 实现独占式资源管理的智能指针，确保同一资源仅被一个指针拥有

    *   - 资源在指针析构时自动释放，防止内存泄漏

   * 模板参数:

   * * - `unique_ptr_type`: 管理的对象类型

   * * - `deleter`: 资源释放器类型，默认为 `std::default_delete<unique_ptr_type>`

   * 构造函数:

   * * - `explicit unique_ptr(unique_ptr_type* ptr = nullptr)`: 从原始指针构造，禁用隐式转换

   * * - 移动构造函数: 转移资源所有权，原指针置空

   * 禁用的操作:

   * * - 拷贝构造、拷贝赋值: 禁止资源共享，确保独占性
   *
   * * - 拷贝赋值(常量右值引用): 防止意外的资源所有权转移

   * 提供的操作符:

   * * - `operator*()`: 解引用管理的对象

   * * - `operator->()`: 访问管理对象的成员

   * * - 移动赋值(右值引用赋值): 转移资源所有权

   * 关键方法:

   * * - `get_ptr()`: 返回原始指针（不释放所有权）


   * 资源管理:

   * * - 使用指定删除器自动释放资源

   * * - 移动操作后原指针置空，避免双重释放

   * * - 支持自定义删除器处理特殊资源（如文件句柄、网络连接）

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

   * 线程安全性:

   * * - 不保证多线程环境下的安全性，操作时需外部同步

   * 注意事项:

   * * - 不要用同一个原始指针创建多个智能指针实例

   * * - 管理数组时需使用自定义删除器

   * * - 避免存储由 `new[]` 分配的数组指针，除非使用匹配的删除器
    */
    template <typename unique_ptr_type, typename deleter = std::default_delete<unique_ptr_type>>
    class unique_ptr
    {
    private:
        unique_ptr_type *_ptr;
        deleter _deleter;
        using Ref = unique_ptr_type &;
        using Ptr = unique_ptr_type *;

    public:
        explicit unique_ptr(unique_ptr_type *ptr = nullptr) noexcept
                : _deleter(deleter())
        {
            _ptr = ptr;
        }
        ~unique_ptr() noexcept
        {
            if (_ptr != nullptr)
            {
                _deleter(_ptr);
                _ptr = nullptr;
            }
        }
        Ref operator*() noexcept
        {
            return *(_ptr);
        }
        Ptr operator->() noexcept
        {
            return _ptr;
        }
        unique_ptr_type *get_ptr() const noexcept
        {
            return _ptr;
        }
        unique_ptr(const unique_ptr &unique_ptr_data) noexcept = delete;
        unique_ptr<unique_ptr_type> &operator=(const unique_ptr<unique_ptr_type, deleter> &unique_ptr_data) noexcept = delete;
        unique_ptr<unique_ptr_type> &operator=(unique_ptr<unique_ptr_type, deleter> &&unique_ptr_data) noexcept
        {
            if (&unique_ptr_data != this)
            {
                if (_ptr != nullptr)
                {
                    _deleter(_ptr);
                    _ptr = nullptr;
                }
                _ptr = unique_ptr_data._ptr;
                unique_ptr_data._ptr = nullptr;
            }
            return *this;
        }
        unique_ptr(unique_ptr &&unique_ptr_data) noexcept
        {
            _ptr = unique_ptr_data._ptr;
            unique_ptr_data._ptr = nullptr;
        }
    };
    /*
   * @brief  #### `shared_ptr` 类

    *   - 实现共享式资源管理的智能指针，通过引用计数实现资源的自动释放

    *   - 多个指针可共享同一资源，最后一个指针释放时资源才被销毁

   * 模板参数:

   * * - `shared_ptr_type`: 管理的对象类型

   * * - `deleter`: 资源释放器类型，默认为 `std::default_delete<shared_ptr_type>`

   * 构造函数:

   * * - `explicit shared_ptr(shared_ptr_type* ptr = nullptr)`: 从原始指针构造

   * * - 拷贝构造函数: 增加引用计数并共享资源

   * * - 移动构造函数: 转移资源所有权，原指针重置

   * 核心机制:

   * * - 引用计数(`shared_pcount`): 记录共享同一资源的指针数量

   * * - 线程安全: 使用互斥锁(`_pmutex`)保护引用计数操作

   * 提供的操作符:

   * * - `operator*()`: 解引用管理的对象

   * * - `operator->()`: 访问管理对象的成员

   * * - 拷贝赋值、移动赋值: 管理引用计数的增减

   * 关键方法:

   * * - `get_count()`: 返回当前引用计数

   * * - `get_ptr()`: 返回原始指针（不释放所有权）

   * 资源管理:

   * * - 最后一个指针释放时调用删除器销毁资源

   * * - 支持自定义删除器处理特殊资源（如文件句柄、网络连接）

   * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

   * 线程安全性:

   * * - 引用计数操作是线程安全的

   * * - 但不保证被管理对象的线程安全，需用户自行同步

   * 注意事项:

   * * - 避免循环引用（会导致内存泄漏，需配合 `weak_ptr` 使用）

   * * - 不要用同一个原始指针创建多个独立的 shared_ptr

   * * - 管理数组时需使用自定义删除器
    */
    template <typename shared_ptr_type, typename deleter = std::default_delete<shared_ptr_type>>
    class shared_ptr
    {
    private:
        shared_ptr_type *_ptr;
        deleter _deleter;
        int *_shared_count;
        std::mutex *_pmutex;
        using Ref = shared_ptr_type &;
        using ptr = shared_ptr_type *;
        void release() noexcept
        {
            _pmutex->lock();
            bool flag = false;
            if (--(*_shared_count) == 0 && _ptr != nullptr)
            {
                _deleter(_ptr);
                _ptr = nullptr;
                delete _shared_count;
                _shared_count = nullptr;
                flag = true;
            }
            _pmutex->unlock();
            if (flag == true)
            {
                delete _pmutex;
                _pmutex = nullptr;
            }
        }
        void assignment(shared_ptr &deliver_value) noexcept
        {
            _ptr = deliver_value._ptr;
            _shared_count = deliver_value._shared_count;
            _pmutex = deliver_value._pmutex;
            _deleter = deliver_value._deleter;
        }

    public:
        explicit shared_ptr(shared_ptr_type *ptr = nullptr)
                : _deleter(deleter())
        {
            _ptr = ptr;
            _shared_count = new int(1);
            _pmutex = new std::mutex;
        }
        shared_ptr(shared_ptr &shared_ptr_data) noexcept
        {
            assignment(shared_ptr_data);
            _pmutex->lock();
            (*_shared_count)++;
            _pmutex->unlock();
        }
        shared_ptr(shared_ptr &&shared_ptr_data) noexcept
        {
            assignment(shared_ptr_data);
            shared_ptr_data._ptr = nullptr;
            shared_ptr_data._shared_count = new int(1);
            shared_ptr_data._pmutex = new std::mutex;
        }
        ~shared_ptr() noexcept
        {
            release();
        }
        shared_ptr<shared_ptr_type> &operator=(const shared_ptr &shared_ptr_data) noexcept
        {
            if (&shared_ptr_data != this)
            {
                if (_ptr != shared_ptr_data._ptr)
                {
                    release();
                    assignment(shared_ptr_data);
                    _pmutex->lock();
                    (*_shared_count)++;
                    _pmutex->unlock();
                }
            }
            return *this;
        }
        shared_ptr<shared_ptr_type> &operator=(shared_ptr &&shared_ptr_data) noexcept
        {
            if (&shared_ptr_data != this)
            {
                release();
                assignment(shared_ptr_data);
                shared_ptr_data._ptr = nullptr;
                shared_ptr_data._shared_count = new int(1);
                shared_ptr_data._pmutex = new std::mutex;
            }
            return *this;
        }
        [[nodiscard]] int get_count() const noexcept
        {
            return _shared_count ? *_shared_count : -1;
        }
        Ref operator*() noexcept
        {
            return *(_ptr);
        }
        ptr operator->() noexcept
        {
            return _ptr;
        }
        ptr get_ptr() const noexcept
        {
            return _ptr;
        }
    };
    /*
            * @brief  #### `weak_ptr` 类

            *   - 弱引用智能指针，用于观察 `shared_ptr` 管理的资源但不增加引用计数

            *   - 解决 `shared_ptr` 的循环引用问题，避免资源无法释放的内存泄漏

            * 模板参数:

            * * - `weak_ptr_type`: 观察的对象类型，需与关联的 `shared_ptr` 管理的类型一致

            * 构造函数:

            * * - 默认构造函数: 初始化空弱指针，不关联任何资源

            * * - `explicit weak_ptr(const shared_ptr<weak_ptr_type>& weak_ptr_data)`: 从 `shared_ptr` 构造，关联其管理的资源（不增加引用计数）

            * * - 拷贝构造函数: 复制另一个弱指针的关联关系（共享观察同一资源）

            * * - 移动构造函数: 转移另一个弱指针的关联关系，原指针置空

            * * - 禁用从 `shared_ptr` 右值构造: 避免关联临时 `shared_ptr` 导致的悬空引用

            * 赋值运算符:

            * * - 拷贝赋值: 复制另一个弱指针的关联关系
            *
            * * - 移动赋值: 转移另一个弱指针的关联关系，原指针置空
            *
            * * - 从 `shared_ptr` 赋值: 重新关联到 `shared_ptr` 管理的资源

            * 核心方法:

            * * - `expired() const noexcept`: 检查关联的资源是否已被释放（引用计数为 0 时返回 `true`）

            * * - `get_count() const noexcept`: 返回关联资源的当前引用计数（未关联时返回 -1）

            * * - `operator*()`: 解引用观察的对象（需先通过 `expired()` 确认资源有效）
            *
            * * - `operator->()`: 访问观察对象的成员（需先确认资源有效）

            * 资源管理:

            * * - 不拥有资源所有权，不影响 `shared_ptr` 的引用计数
            *
            * * - 析构时仅重置内部指针，不涉及资源释放
            *
            * * - 当关联的 `shared_ptr` 引用计数归 0 时，弱指针自动变为无效状态

            * 详细请参考 https://github.com/Hatedatastructures/Custom-libraries/blob/main/template_container.md

            * 注意事项:

            * * - `weak_ptr` 创建的指针的生命周期要小于 `shared_ptr`，否则会出现悬空指针，导致资源提前释放
            *
            * * - 使用 `operator*()` 或 `operator->()` 前必须通过 `expired()` 检查资源有效性，否则可能访问已释放内存
            *
            * * - 不能直接通过 `weak_ptr` 管理资源生命周期，需配合 `shared_ptr` 使用
            *
            * * - 适用于观察者模式、缓存管理等需临时访问资源但不延长其生命周期的场景
            *
            * * - 线程安全性：内部指针操作非线程安全，多线程环境下需外部同步
    */
    template <typename weak_ptr_type>
    class weak_ptr
    {
    private:
        weak_ptr_type *_ptr;
        pointer::shared_ptr<weak_ptr_type> *_shared_ptr;
        using Ref = weak_ptr_type &;
        using ptr = weak_ptr_type *;

    public:
        weak_ptr() noexcept
                : _ptr(nullptr), _shared_ptr(nullptr) {}
        explicit weak_ptr(shared_ptr<weak_ptr_type> &weak_ptr_data) noexcept
                : _ptr(weak_ptr_data.get_ptr()), _shared_ptr(&weak_ptr_data) {}
        weak_ptr(const weak_ptr &weak_ptr_data) noexcept
                : _ptr(weak_ptr_data._ptr), _shared_ptr(weak_ptr_data._shared_ptr) {}
        weak_ptr(weak_ptr &&weak_ptr_data) noexcept
                : _ptr(weak_ptr_data._ptr), _shared_ptr(weak_ptr_data._shared_ptr)
        {
            weak_ptr_data._ptr = nullptr;
            weak_ptr_data._shared_ptr = nullptr;
        }
        weak_ptr(shared_ptr<weak_ptr_type> &&shared_ptr_data) = delete;
        weak_ptr &operator=(const weak_ptr &weak_ptr_data) noexcept
        {
            if (&weak_ptr_data != this)
            {
                _ptr = weak_ptr_data._ptr;
                _shared_ptr = weak_ptr_data._shared_ptr;
            }
            return *this;
        }
        weak_ptr &operator=(weak_ptr &&weak_ptr_data) noexcept
        {
            if (&weak_ptr_data != this)
            {
                _ptr = weak_ptr_data._ptr;
                _shared_ptr = weak_ptr_data._shared_ptr;
                weak_ptr_data._ptr = nullptr;
                weak_ptr_data._shared_ptr = nullptr;
            }
            return *this;
        }
        weak_ptr &operator=(const shared_ptr<weak_ptr_type> &shared_ptr_data) noexcept
        {
            _ptr = shared_ptr_data.get_ptr();
            _shared_ptr = &shared_ptr_data;
            return *this;
        }
        ~weak_ptr() noexcept
        {
            if (_shared_ptr == nullptr)
            {
                delete _ptr;
            }
            _ptr = nullptr;
            _shared_ptr = nullptr;
        }

        Ref operator*() noexcept
        {
            return *(_ptr);
        }
        ptr operator->() noexcept
        {
            return _ptr;
        }
        bool expired() const noexcept
        {
            return _shared_ptr->get_count() == 0;
        }
        int get_count() const noexcept
        {
            return _shared_ptr ? _shared_ptr->get_count() : -1;
        }
    };
}