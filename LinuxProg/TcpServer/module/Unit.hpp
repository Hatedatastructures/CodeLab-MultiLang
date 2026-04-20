#pragma once
#include <variant>
#include <atomic>
#include <algorithm>
#include <functional>
#include <memory>
#include <future>
#include <condition_variable>
#include <coroutine>
#include <typeinfo>
#include <any>
#include <string>
#include <utility>
#include <type_traits> 
#include <optional>
#include "Integration.hpp"

namespace internals
{
    namespace structure_u
    {
        /**
     * @class unit_ordinary
     * @brief #### 基本任务类 - 定义所有任务的通用接口
     * @tparam execute_function 任务执行函数类型,可以在上层自动推导来获取任务返回值类型
     * @warning #### 计算结果不可重复获取，创建任务对象通过工厂函数创建
     *
     * 核心功能： 线程安全的任务状态管理, 优先级设置和获取, 超时时间管理
     * ,任务取消机制, 执行时间统计
     * 
     * 被调用者：`worker`线程、`scheduler`调度器
     * 
     *  调用者：衍生任务类、`thread_pool`管理器
     */
        class unit_ordinary
        {
            friend class thread_pool;
        public:
            /**
       * @brief #### 标记任务开始执行（由`worker`线程调用）
       * @return `true` 标记成功，`false` 任务已被取消或超时
       */
            bool markRunning()
            {
                current_status expected = current_status::pending;
                if (_state.compare_exchange_strong(expected, current_status::running, std::memory_order_acq_rel))
                {
                    _startTime = std::chrono::steady_clock::now();
                    return true;
                }
                return false;
            }

            /**
       * @brief #### 标记任务完成（由`worker`线程调用）
       */
            void markCompleted()
            {
                _endTime = std::chrono::steady_clock::now();
                _state.store(current_status::completed, std::memory_order_release);
                std::lock_guard<std::mutex> lock(_stateMutex);
                _stateCv.notify_all();
            }

            /**
       * @brief #### 标记任务失败（由`worker`线程调用）
       */
            void markFailed()
            {
                _endTime = std::chrono::steady_clock::now();
                _state.store(current_status::failed, std::memory_order_release);
                std::lock_guard<std::mutex> lock(_stateMutex);
                _stateCv.notify_all();
            }

            /**
       * @brief #### 获取任务创建时间
       * @return 任务创建时间点
       */
            std::chrono::steady_clock::time_point getCreationTime() const
            {
                return _submitTime;
            }

            /**
       * @brief #### 获取任务执行函数
       * @return 任务执行函数的引用
       */
            const std::function<void()>& getExecution() const
            {
                return _ordinaryExecution;
            }

            /**
       * @brief #### 设置任务执行函数
       * @param execution 新的执行函数
       */
            void setExecution(const std::function<void()>& execution)
            {
                _ordinaryExecution = execution;
            }

        protected:

            constexpr std::string coverageString(const std::string &str) const
            {
                if (str.empty())
                {
                    return ("task_" + std::to_string(_identifier));
                }
                return str;
            }

        protected:

            std::uint64_t _identifier; // 任务唯一标识
            static std::atomic<std::uint64_t> _currentUniqueIdentifier; // 全局任务ID生成器

            std::function<void()> _ordinaryExecution;  // 任务执行函数

            std::string _taskName; // 任务名称
            std::atomic<current_status> _state{current_status::pending}; // 任务状态（原子操作保证线程安全）

            std::chrono::steady_clock::time_point _deadline; // 超时时间点
            std::chrono::steady_clock::time_point _endTime; // 结束执行时间
            std::chrono::steady_clock::time_point _startTime; // 开始执行时间
            std::chrono::steady_clock::time_point _submitTime; // 提交时间

            mutable std::mutex _stateMutex; // 状态变更互斥锁
            mutable std::condition_variable _stateCv; // 状态变更条件变量
            std::atomic<bool> _hasDeadline{false}; // 是否设置了超时时间

            std::atomic<std::int32_t> _priority; // 任务优先级

        public:

            template <typename execute_function>
            unit_ordinary(execute_function&& function,const std::string &name = "", 
                weight priority = weight::normal)
            :_identifier(_currentUniqueIdentifier.fetch_add(1, std::memory_order_relaxed)),
            _ordinaryExecution(std::forward<execute_function>(function)),
            _taskName(coverageString(name)),_submitTime(std::chrono::steady_clock::now()),
            _priority(static_cast<std::int32_t>(priority)) {}

            virtual ~unit_ordinary() = default;

            unit_ordinary(const unit_ordinary&) = delete;

            unit_ordinary& operator=(const unit_ordinary&) = delete;

            unit_ordinary(unit_ordinary&&) = delete;

            unit_ordinary& operator=(unit_ordinary&&) = delete;

            /**
       * @brief #### 执行任务 - 虚函数，子类根据自身情况实现
       * @return 任务执行结果(`derivation`类型支持任意返回类型)
       * @throws `execution_exception` 任务执行异常
       *
       * 调用者：`worker`线程, 被调用者：衍生任务类
       */
            virtual derivation execute()
            {
                if(markRunning() == false)
                {
                    throw execution_exception("任务不在运行状态,检查任务",getIdentifier());
                }
                try
                {
                    _ordinaryExecution();
                    markCompleted();
                    return derivation();
                }
                catch (const std::exception& error)
                {
                    markFailed();
                    throw execution_exception(std::string(error.what()), getIdentifier());
                }
                catch(...)
                {
                    this->markFailed();
                    throw execution_exception("任务执行失败: 未知错误", getIdentifier());
                }
            }

            /**
       * @brief 检查任务是否有返回值
       * @return `true` 无返回值，`false` 有返回值
       */
            virtual bool isVoidTask() const noexcept 
            {
                return false;
            }

            /**
       * @brief #### 取消任务
       * @return `true` 取消成功，`false` 任务已开始执行无法取消
       *
       * 调用者：`scheduler`调度器、用户代码,依赖：原子状态变更、条件变量通知
       */
            virtual bool cancel()
            {
                current_status expected = current_status::pending;
                if (_state.compare_exchange_strong(expected, current_status::cancelled, std::memory_order_acq_rel))
                {
                    std::lock_guard<std::mutex> lock(_stateMutex);
                    _stateCv.notify_all();
                    return true;
                }
                return false;
            }

            /**
       * @brief #### 检查任务是否超时
       * @return `true` 未超时，`false` 超时
       *
       * 调用者：`scheduler`调度器,依赖：超时时间点比较
       */
            virtual bool is_timeout() const
            {
                return std::chrono::steady_clock::now() < _deadline;
            }

            /**
       * @brief #### 是否设置任务超时
       */
            bool hasDeadline() const noexcept
            {
                return _hasDeadline.load(std::memory_order_acquire);
            }

            /**
       * @brief #### 标记任务超时
       * @return `true` 标记成功，`false` 任务已开始执行
       *
       * 调用者：`scheduler`调度器
       */
            virtual bool markTimeout()
            {
                current_status expected = current_status::pending;
                if (_state.compare_exchange_strong(expected, current_status::timeout, std::memory_order_acq_rel))
                {
                    std::lock_guard<std::mutex> lock(_stateMutex);
                    _stateCv.notify_all();
                    return true;
                }
                return false;
            }

            /**
       * @brief #### 获取任务状态
       * @return 当前任务状态
       */
            current_status get_state() const noexcept
            {
                return _state.load(std::memory_order_acquire);
            }

            /**
       * @brief #### 获取任务优先级
       * @return 任务优先级值
       */
            std::int32_t getPriority() const noexcept
            {
                return _priority.load(std::memory_order_acquire);
            }

            /**
       * @brief #### 设置任务优先级
       * @param priority 新的优先级
       */
            void setPriority(weight priority) noexcept
            {
                _priority.store(static_cast<std::int32_t>(priority), std::memory_order_release);
            }

            /**
       * @brief #### 设置任务优先级
       * @param priority 新的优先级值
       */
            void setPriority(std::int32_t priority) noexcept
            {
                _priority.store(priority, std::memory_order_release);
            }

            /**
       * @brief #### 获取任务ID
       * @return 任务唯一标识
       */
            virtual std::uint64_t getIdentifier() const noexcept
            {
                return _identifier;
            }

            /**
       * @brief #### 获取任务名称
       * @return 任务名称
       */
            const std::string& getTaskName() const noexcept
            {
                return _taskName;
            }

            /**
       * @brief #### 获取任务超时时间点
       */
            std::chrono::steady_clock::time_point getDeadline() const
            {
                return _deadline;
            }

            /**
       * @brief #### 设置超时时间
       * @param timeout 超时时长
       */
            template<typename rep, typename period>
            void setTimeout(const std::chrono::duration<rep, period>& timeout)
            {
                _deadline = std::chrono::steady_clock::now() + timeout;
                _hasDeadline.store(true, std::memory_order_release);
            }

            /**
       * @brief #### 设置绝对超时时间点
       * @param deadline 超时时间点
       */
            void setDeadline(const std::chrono::steady_clock::time_point& deadline)
            {
                _deadline = deadline;
                _hasDeadline.store(true, std::memory_order_release);
            }

            /**
       * @brief #### 获取任务提交时间
       * @return 提交时间点
       */
            std::chrono::steady_clock::time_point getSubmitTime() const noexcept
            {
                return _submitTime;
            }

            /**
       * @brief #### 获取任务执行时长
       * @return 执行时长（毫秒），如果任务未完成返回0
       */
            std::chrono::milliseconds getExecutionDuration() const
            {
                if (_startTime == std::chrono::steady_clock::time_point{} ||
                    _endTime == std::chrono::steady_clock::time_point{})
                    return std::chrono::milliseconds{0};
                return std::chrono::duration_cast<std::chrono::milliseconds>(_endTime - _startTime);
            }

            /**
       * @brief #### 获取任务等待时长
       * @return 等待时长（毫秒），从提交到开始执行
       */
            std::chrono::milliseconds getWaitDuration() const
            {
                if (_startTime == std::chrono::steady_clock::time_point{})
                    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _submitTime);
                return std::chrono::duration_cast<std::chrono::milliseconds>(_startTime - _submitTime);
            }

            /**
       * @brief #### 等待任务完成(超时)
       * @param timeout 等待超时时间
       * @return `true` 任务完成，`false` 等待超时
       */
            template<typename rep, typename period>
            bool waitFor(const std::chrono::duration<rep, period>& timeout) const
            {
                std::unique_lock<std::mutex> lock(_stateMutex);
                auto await_func = [this]()
                {
                    auto state = _state.load(std::memory_order_acquire);
                    return state == current_status::completed || state == current_status::cancelled 
                    || state == current_status::timeout || state == current_status::failed;
                };
                return _stateCv.waitFor(lock, timeout, await_func);
            }

            /**
       * @brief #### 等待任务完成（无超时）
       */
            virtual void wait() const
            {
                std::unique_lock<std::mutex> lock(_stateMutex);
                auto await_func = [this]()
                {
                    auto state = _state.load(std::memory_order_acquire);
                    return state == current_status::completed || state == current_status::cancelled 
                    || state == current_status::timeout || state == current_status::failed;
                };
                _stateCv.wait(lock, await_func);
            }

            /**
       * @brief #### 获取任务执行结果（阻塞等待）
       * @return 任务执行结果
       * @throws 任务执行过程中的异常
       */
            virtual derivation getResult()
            {
                return derivation{};
            }

            /**
       * @brief #### 检查结果是否就绪
       * @return `true` 结果就绪，`false` 结果未就绪
       */
            virtual bool isResultReady() const noexcept
            {
                return false;
            }

            constexpr bool operator<(const unit_ordinary& other) const noexcept
            {
                return this->getPriority() < other.getPriority();
            }

            constexpr bool operator>(const unit_ordinary& other) const noexcept
            {
                return this->getPriority() > other.getPriority();
            }
        };

        std::atomic<std::uint64_t> unit_ordinary::_currentUniqueIdentifier{1};

        /**
     * @class unit_standard
     * @brief #### 标准任务类 - 支持异步结果获取
     * @tparam execute_function 任务类型模板
     *
     * 适用场景：需要获取执行结果的任务,计算密集型任务,数据处理和转换
     * 
     * 调用关系：继承自`unit_ordinary`,使用`std::promise`和`std::future`实现结果同步
     * 由`thread_pool::submit()`创建,由`worker`线程执行
     */
        template<typename execute_function, typename result = std::invoke_result_t<execute_function>>
        class unit_standard : public unit_ordinary
        {
            //对于和基类成员变量名相同的成员变量，在多态里默认隐藏，可以通过限定符来访问
        protected:

            std::promise<result> _promise; // 结果承诺
            std::future<result> _future; // 结果期望

            std::atomic<bool> _readyState{false};  // 结果是否就绪
            std::function<result()> _standardExecution;  // 任务执行函数

        public:

            unit_standard(execute_function&& function, const std::string &name = "", 
                weight priority = weight::normal)
            :unit_ordinary([](){},name,priority),_promise(), _future(_promise.getFuture()),
       _standardExecution(std::forward<execute_function>(function)) {}

            
            derivation execute() override
            {
                if(!this->markRunning())
                {
                    _promise.set_exception(std::make_exception_ptr(execution_exception("任务无法启动",getIdentifier())));
                    _readyState.store(true, std::memory_order_release);
                    throw execution_exception("任务无法启动", getIdentifier());
                }
                try
                {
                    if constexpr (std::is_void_v<result>)
                    {
                        this->_standardExecution();
                        _promise.set_value();
                        _readyState.store(true, std::memory_order_release);
                        this->markCompleted();
                        return derivation();
                    }
                    else
                    {
                        auto result_value = this->_standardExecution();
                        _promise.set_value(result_value);
                        _readyState.store(true, std::memory_order_release);
                        this->markCompleted();
                        return derivation(result_value);
                    }
                }
                catch (const std::exception& e)
                {
                    this->markFailed();
                    _promise.set_exception(std::current_exception());
                    _readyState.store(true, std::memory_order_release);
                    throw execution_exception("任务执行失败: " + std::string(e.what()), getIdentifier());
                }
                catch (...)
                {
                    _promise.set_exception(std::current_exception());
                    _readyState.store(true, std::memory_order_release);
                    this->markFailed();
                    throw execution_exception("任务执行失败: 未知错误", getIdentifier());
                }
            }

            bool isVoidTask() const noexcept override
            {
                return std::is_void_v<result>;
            }

            bool isResultReady() const noexcept override
            {
                return _readyState.load(std::memory_order_acquire);
            }

            /**
       * @brief #### 获取`future`对象
       * @return 关联的`future`对象
       * @note 结果只能获取一次，重复获取会抛出异常,后续结果通过工厂函数直接获取
       */
            std::future<result> getFuture()
            {
                return std::move(_future);
            }

            const std::future<result>& getFuture() const
            {
                return _future;
            }

            derivation getResult() override
            {
                try
                {
                    if constexpr (std::is_void_v<result>)
                    {
                        _future.get();
                        return derivation();
                    }
                    else
                    {
                        result value = _future.get(); 
                        return derivation(std::move(value));
                    }
                }
                catch (const std::exception& e)
                {
                    throw execution_exception("获取任务结果失败: " + std::string(e.what()), getIdentifier());
                }
                catch (...)
                {
                    throw execution_exception("获取任务结果失败: 未知错误", getIdentifier());
                }
            }
        };

        /**
     * @class unit_overtime
     * @brief #### 超时任务类 - 支持超时检查和处理
     *
     * 适用场景：有时间限制的任务, 网络请求和`IO`操作, 需要及时响应的任务
     *
     * 调用关系： 继承自`unit_standard`, 由`scheduler`定期检查超时, 支持超时回调处理
     */
        template<typename execute_function, typename timeout_function>
        class unit_overtime : public unit_standard<execute_function>
        {
        protected:

            std::atomic<bool> _timeoutHandled{false}; // 超时是否已处理
            std::decay_t<timeout_function> _timeoutCallback; // 超时回调函数

        public:

            template<typename func, typename rep, typename period>
            unit_overtime(func&& function, const std::chrono::duration<rep, period>& timeout,
       timeout_function&& timeout_callback, const std::string &name = "")
            :unit_standard<execute_function>(std::forward<func>(function),name),
       _timeoutCallback(std::forward<timeout_function>(timeout_callback))
            {  this->setTimeout(timeout);  }
            
            /**
       * @brief #### 标记任务超时（重写`unit_ordinary`类方法）
       * @return `true` 标记成功，`false` 任务已开始执行
       */
            bool markTimeout() override
            {
                if(unit_ordinary::markTimeout())
                {
                    handleTimeout();
                    return true;
                }
                return false;
            }

            /**
       * @brief #### 处理超时事件
       */
            void handleTimeout()
            {
                bool expected = false;
                if (_timeoutHandled.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
                {
                    if (_timeoutCallback)
                    {
                        try
                        {
                            _timeoutCallback();
                        }
                        catch (...) {}
                    }
                }
            }

            /**
       * @brief #### 设置超时回调函数
       * @param callback 超时回调函数
       */
            void setTimeoutCallback(timeout_function&& callback)
            {
                _timeoutCallback = std::forward<timeout_function>(callback);
            }

            /**
       * @brief #### 检查超时是否已处理
       * @return `true` 已处理，`false` 未处理
       */
            bool isTimeoutHandled() const noexcept      
            {
                return _timeoutHandled.load(std::memory_order_acquire);
            }
        };

        /**
     * @class task_depn
     * @brief #### 依赖任务类 - 支持任务间依赖关系
     *
     * 适用场景：需要按顺序执行的任务链, 数据流水线处理,复杂业务逻辑分解
     *
     * 调用关系：继承自`task_rslt`, 依赖其他任务的完成状态, 由`scheduler`检查依赖关系
     */
        template<typename execute_function, uint64_t MAX_CACHE_VALIDITY = 100ULL>
        class unit_reliance : public unit_standard<execute_function>
        {
        protected:

            mutable std::atomic<std::uint64_t> _lastCheckTime{0}; // 上次检查时间戳
            mutable std::atomic<bool> _dependenciesSatisfied{false}; // 依赖是否已满足（缓存）

            mutable std::mutex _dependencyMutex; // 依赖检查互斥锁
            mutable std::condition_variable _dependencyCv; // 依赖状态变更条件变量
            std::vector<std::shared_ptr<unit_ordinary>> _dependencyList; // 依赖任务列表

            static constexpr std::uint64_t CACHE_VALIDITY = MAX_CACHE_VALIDITY; // 缓存有效期（毫秒）

        protected:

            /**
                * @brief #### 获取当前时间戳（毫秒）
                * @return 当前时间戳
                */
            static std::uint64_t getCurrentTimeMs()
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::steady_clock::now().time_since_epoch()).count();
            }

            /**
                * @brief #### 不安全的依赖检查（假设已持有锁）
                * @return 如果所有依赖都已完成则返回`true`
                */
            bool areDependenciesSatisfiedUnsafe() const
            {
                auto satisfied_func = [](const std::shared_ptr<unit_ordinary>& dep) 
                {
                    return dep && dep->get_state() == current_status::completed;
                };
                return std::all_of(_dependencyList.begin(), _dependencyList.end(),satisfied_func);
            }
        public:
            unit_reliance(execute_function&& function, const std::vector<std::shared_ptr<unit_ordinary>>& dependencies = {},
       const std::string &name = "", weight priority = weight::normal)
            :unit_standard<execute_function>(std::forward<execute_function>(function),name,priority),
       _dependencyList(dependencies)
            {
                auto null_pointer_check = [](const std::shared_ptr<unit_ordinary>& ptr)
                {
                    return ptr != nullptr;
                };
                if(!_dependencyList.empty())
                {
                    _dependencyList.erase(std::remove_if(_dependencyList.begin(),_dependencyList.end(),
                    null_pointer_check),_dependencyList.end());
                }
            }

            unit_reliance(execute_function&& function, std::shared_ptr<unit_ordinary> dependency,
       const std::string &name = "", weight priority = weight::normal)
            :unit_standard<execute_function>(std::forward<execute_function>(function),name,priority)
            {
                _dependencyList.push_back(std::move(dependency));
            }

            /**
       * @brief #### 添加依赖任务
       * @param dependency 依赖的任务
       */
            void addDependency(std::shared_ptr<unit_ordinary> dependency)
            {
                std::lock_guard<std::mutex> lock(_dependencyMutex);
                if (dependency && this->unit_ordinary::get_state() == current_status::pending)
                {
                    _dependencyList.push_back(std::move(dependency));
                    // 重置缓存状态
                    _dependenciesSatisfied.store(false, std::memory_order_release);
                    _lastCheckTime.store(0, std::memory_order_release);
                }
            }

            /**
       * @brief #### 检查所有依赖是否完成
       * @return `true` 所有依赖已完成，`false` 存在未完成的依赖
       */
            bool areDependenciesSatisfied() const
            {
                // 检查缓存
                auto current_time = getCurrentTimeMs();
                auto last_check = _lastCheckTime.load(std::memory_order_acquire);
                
                if (_dependenciesSatisfied.load(std::memory_order_acquire) && 
                (current_time - last_check) < CACHE_VALIDITY)
                {
                    return true;
                }
                
                std::lock_guard<std::mutex> lock(_dependencyMutex);
                
                // 双重检查
                if (_dependenciesSatisfied.load(std::memory_order_acquire) && 
                (getCurrentTimeMs() - _lastCheckTime.load(std::memory_order_acquire)) < CACHE_VALIDITY)
                {
                    return true;
                }

                auto satisfied_func = [](const std::shared_ptr<unit_ordinary>& dep) 
                {
                    return dep && dep->get_state() == current_status::completed;
                };
                // 批量检查依赖
                bool all_satisfied = std::all_of(_dependencyList.begin(), _dependencyList.end(),satisfied_func);
                
                // 更新
                _dependenciesSatisfied.store(all_satisfied, std::memory_order_release);
                _lastCheckTime.store(getCurrentTimeMs(), std::memory_order_release);
                
                return all_satisfied;
            }

            /**
       * @brief #### 获取未完成的依赖任务
       * @return 未完成的依赖任务列表
       */
            std::vector<std::shared_ptr<unit_ordinary>> getPendingDependencies() const
            {
                std::lock_guard<std::mutex> lock(_dependencyMutex);
                std::vector<std::shared_ptr<unit_ordinary>> pending;
                pending.reserve(_dependencyList.size()); // 预分配内存
                auto get_pending_func = [](const std::shared_ptr<unit_ordinary>& dep) 
                {
                    if (!dep) return false;
                    auto state = dep->get_state();
                    return state != current_status::completed && state != current_status::cancelled && 
                    state != current_status::timeout && state != current_status::failed;
                };
                std::copy_if(_dependencyList.begin(), _dependencyList.end(), 
                std::back_inserter(pending),get_pending_func);
                
                return pending;
            }

            /**
       * @brief #### 获取依赖任务数量
       * @return 依赖任务数量
       */
            std::size_t getDependencyCount() const
            {
                std::lock_guard<std::mutex> lock(_dependencyMutex);
                return _dependencyList.size();
            }

            /**
       * @brief #### 等待所有依赖完成
       * @param timeout 等待超时时间
       * @return true 所有依赖完成，false 等待超时
       */
            template <typename rep, typename period>
            bool waitForDependencies(const std::chrono::duration<rep, period> &timeout) const
            {
                std::unique_lock<std::mutex> lock(_dependencyMutex);
                auto wait_function = [this]()
                {
                    return areDependenciesSatisfiedUnsafe();
                };
                return _dependencyCv.waitFor(lock, timeout, wait_function);
            }
        };
        /**
     * @brief #### 任务工厂函数 - 创建基本任务
     * @param func 任务执行函数
     * @param name 任务名称
     * @param priority 任务优先级
     * @return 智能指针
     */
        template<typename funcion_t>
        std::shared_ptr<unit_ordinary> makeUnitOrdinary(funcion_t&& func,
            weight priority = weight::normal,const std::string &name = "")
        {
            return std::make_shared<unit_ordinary>(std::forward<funcion_t>(func),name,priority);
        }

        /**
     * @brief #### 任务工厂函数 - 创建标准任务
     * @tparam function_t 任务执行函数类型
     * @param func 任务执行函数
     * @param name 任务名称
     * @param priority 任务优先级
     * @return 智能指针
     */
        template<typename function_t, typename result_t = std::invoke_result_t<function_t>>
        std::shared_ptr<unit_standard<function_t, result_t>> makeUnitStandard(function_t&& func,
            weight priority = weight::normal,const std::string &name = "")
        {
            return std::make_shared<unit_standard<function_t, result_t>>
            (std::forward<function_t>(func),name,priority);
        }

        /**
     * @brief #### 任务工厂函数 - 创建超时任务
     * @tparam function_t 任务执行函数类型
     * @tparam timeout_function 超时回调函数类型
     * @param func 任务执行函数
     * @param timeout 超时时间
     * @param timeout_callback 超时回调函数
     * @param name 任务名称
     * @return 智能指针
     */
        template<typename function_t, typename timeout_function, typename rep, typename period>
        std::shared_ptr<unit_overtime<function_t, timeout_function>> makeUnitOvertime(function_t&& func,
            const std::chrono::duration<rep, period>& timeout, timeout_function&& timeout_callback,
            const std::string &name = "")
        {
            return std::make_shared<unit_overtime<function_t, timeout_function>>
            (std::forward<function_t>(func),timeout,std::forward<timeout_function>(timeout_callback),name);
        }
        /**
     * @brief #### 任务工厂函数 - 创建依赖任务
     * @tparam execute_function 任务执行函数类型
     * @tparam MAX_CACHE_VALIDITY 任务缓存有效期（毫秒）
     * @param func 任务执行函数
     * @param dependencies 依赖任务列表
     * @param name 任务名称
     * @param priority 任务优先级
     * @return 智能指针
     */
        template<uint64_t MAX_CACHE_VALIDITY = 100ULL, typename function_t>
        std::shared_ptr<unit_reliance<function_t,MAX_CACHE_VALIDITY>> makeUnitReliance(
            function_t&& func,const std::vector<std::shared_ptr<unit_ordinary>>& dependencies = {},
            const std::string &name = "", weight priority = weight::normal)
        {
            return std::make_shared<unit_reliance<function_t,MAX_CACHE_VALIDITY>>
            (std::forward<function_t>(func),dependencies,name,priority);
        }
        template<uint64_t MAX_CACHE_VALIDITY = 100ULL, typename function_t>
        std::shared_ptr<unit_reliance<function_t,MAX_CACHE_VALIDITY>> makeUnitReliance
        (function_t&& func, std::shared_ptr<unit_ordinary> dependency,
            const std::string &name = "", weight priority = weight::normal)
        {
            return std::make_shared<unit_reliance<function_t,MAX_CACHE_VALIDITY>>
            (std::forward<function_t>(func),std::move(dependency),name,priority);
        }
    }
}

namespace pool 
{
    using internals::structure_u::unit_ordinary;
    using internals::structure_u::unit_standard;
    using internals::structure_u::unit_overtime;
    using internals::structure_u::unit_reliance;

    using internals::structure_u::makeUnitOrdinary;
    using internals::structure_u::makeUnitStandard;
    using internals::structure_u::makeUnitOvertime;
    using internals::structure_u::makeUnitReliance;
}