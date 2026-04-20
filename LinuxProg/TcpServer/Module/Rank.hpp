#pragma once
#include "Unit.hpp"
#include "Integration.hpp"
#include <set>
#include <queue>
#include <deque>
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <typeinfo>
#include <shared_mutex>
#include <unordered_map>

#define parameter_discard(parameter) (void)(parameter)
#define macro_statement throw operation_exception("The current derived class has not overridden the function.")

namespace internals
{
    namespace structure_r {}
}
namespace internals::structure_r
{
    using namespace internals::structure_u;
    using safety_unit_pointer = std::shared_ptr<unit_ordinary>;

    using internals_clk    = std::chrono::system_clock;
    using internals_time_t = std::chrono::system_clock::time_point;
    using internals_time   = std::shared_ptr<internals_time_t>;

    /**
   * @brief 任务队列基类
   * @details 任务队列基类，定义了任务队列的基本接口，以及任务队列的基本属性。
   * @warning 该类需重载内部函数版本来消除运行时异常
   */
    class rank_ordinary
    {
    protected:

    // 计算执行单元默认超时时间点
    internals_time internalCalculationDeadline()
    {
        if(!_unitTimeLimit)
        {
            return nullptr;
        }
        internals_time_t now_time = std::chrono::system_clock::now() + _defaultFunctionTimeout;
        return std::make_shared<internals_time_t>(now_time);
    }

    protected:

        std::atomic<bool> _closed{false}; //关闭标识
        std::atomic<bool> _unitTimeLimit{false}; //执行单元时间限制
        std::atomic<std::size_t> _maxStorageCapacity{0}; //最大队列大小
        std::chrono::milliseconds _defaultFunctionTimeout{1000}; //默认延时时间 

    protected:
        // 内部推送任务接口
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode, 
        internals_time deadline  = nullptr)
        {
            parameter_discard(pointer);  parameter_discard(mode);
            parameter_discard(deadline); macro_statement;
            return false;
        }
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode)
        {
            parameter_discard(pointer); parameter_discard(mode); macro_statement;
            return false;
        }
        // 内部批量推送任务接口
        virtual std::size_t internalPushBatch(std::vector<safety_unit_pointer>&& pointers,
       backpressure mode)
        {
            parameter_discard(pointers); parameter_discard(mode); macro_statement;
            return std::size_t(0);
        }
        // 内部弹出任务接口
        virtual safety_unit_pointer internalPop()
        {
            macro_statement;
            return nullptr;
        }
        // 内部批量弹出任务接口
        virtual std::vector<safety_unit_pointer> internalPopBatch(const std::size_t count)
        {
            parameter_discard(count); macro_statement;
            return {};
        }
        // 内部尝试弹出任务接口
        virtual safety_unit_pointer internalTryPop()
        {
            macro_statement;
            return nullptr;
        }
        // 内部尝试弹出任务接口（带超时）
        virtual safety_unit_pointer internalTryPop_for(const std::chrono::milliseconds& timeout)
        {
            parameter_discard(timeout); macro_statement;
            return nullptr;
        }
        // 内部获取队列大小接口
        virtual std::size_t internalSize() const
        {
            macro_statement;
            return 0;
        }
        // 内部判断队列是否为空接口
        virtual bool internalEmpty() const
        {
            macro_statement;
            return true;
        }
        // 内部清空队列接口
        virtual void internalClear()
        {
            macro_statement;
            return;
        }
        // 内部关闭接口
        virtual void internalClose()
        {
            macro_statement;
            return;
        }
        // 内部获取子队列数量接口
        virtual std::size_t internalGetSubQueueCount() const
        {
            macro_statement;
            return 0;
        }
        // 内部获取延迟执行单元数量接口
        virtual std::size_t internalGetDelayUintCount() const
        {
            macro_statement;
            return 0;
        }
        // 内部获取调度策略接口
        virtual rank_strategy internalStrategy() const
        {
            macro_statement;
            return rank_strategy::fifo;
        }
    public:
        rank_ordinary(const std::size_t size) :_maxStorageCapacity(size) {} 

        virtual ~rank_ordinary() = default;

        rank_strategy strategy() const 
        { 
            return internalStrategy(); 
        }

        bool push(safety_unit_pointer pointer, backpressure mode = backpressure::block) 
        {
            if(strategy() == rank_strategy::delay) 
            {
                return internalPush(std::move(pointer), mode, internalCalculationDeadline());
            }
            return internalPush(std::move(pointer), mode,nullptr);
        }

        bool push(safety_unit_pointer pointer, std::chrono::system_clock::time_point deadline,
        backpressure mode = backpressure::block)
        {
            internals_time time_point = std::make_shared<std::chrono::system_clock::time_point>(deadline);
            return internalPush(std::move(pointer), mode, time_point);
        }

        std::size_t push_batch(std::vector<safety_unit_pointer> pointers, backpressure mode = backpressure::block)
        {
            return internalPushBatch(std::move(pointers), mode);
        }

        safety_unit_pointer pop()
        {
            return internalPop();
        }

        std::vector<safety_unit_pointer> pop_batch(const std::size_t count)
        {
            return internalPopBatch(count);
        }

        safety_unit_pointer try_pop()
        {
            return internalTryPop();
        }
        template<typename rep, typename period>
        safety_unit_pointer try_pop_for(const std::chrono::duration<rep, period>& timeout)
        {
            return internalTryPop_for(convert_time::to_milliseconds(timeout));
        }

        std::size_t size() const
        {
            return internalSize();
        }

        bool empty() const 
        { 
            return internalEmpty(); 
        }

        void clear() 
        {
            internalClear();
        }
        // 关闭提交，拒绝新任务提交
        virtual void close() 
        {
            internalClose();
        }
        
        bool closed() const 
        { 
            return _closed.load(std::memory_order_acquire); 
        }
        bool setMaxSize(const std::size_t max_size)
        {
            _maxStorageCapacity.store(max_size, std::memory_order_relaxed);
            return true;
        }
        std::size_t getMaxSize()const  
        {
            return _maxStorageCapacity.load();
        }

        std::size_t getSubQueueCount()  const 
        { 
            return internalGetSubQueueCount(); 
        }

        std::size_t getDelayUintCount() const 
        { 
            return internalGetDelayUintCount(); 
        }

    };
    /**
   * @brief 标准任务队列
   * @details 线程安全的标准任务队列，支持阻塞、覆盖、异常三种背压策略
   * @note 底层容器为`std::deque`
   */
    class rank_standard : public rank_ordinary
    {
    protected:

        std::deque<safety_unit_pointer> _rankUnitStandard;

        std::condition_variable_any _judgeFullCv;
        std::condition_variable_any _judgeEmptyCv;

        mutable std::shared_mutex _rankStandardMutex;

    public:
        explicit rank_standard(std::size_t max_size = 0) : rank_ordinary(max_size) {}

        virtual ~rank_standard() = default;

    private:
        bool enqueueWithBackpressure(safety_unit_pointer pointer, backpressure mode)
        {
            std::size_t current_size = 0;
            
            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);
            current_size = _rankUnitStandard.size();
            
            if((_maxStorageCapacity != 0 && current_size >= _maxStorageCapacity) == false)
            {
                _rankUnitStandard.push_back(std::move(pointer));
                lock.unlock();
                _judgeEmptyCv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rankUnitStandard.size() < this->_maxStorageCapacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judgeFullCv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rankUnitStandard.push_back(std::move(pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                {
                    if(!_rankUnitStandard.empty())  _rankUnitStandard.pop_back();
                    _rankUnitStandard.push_back(std::move(pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::exception:
                    lock.unlock();
                    throw operation_exception("The queue is full, please check the overflow policy.");
                case backpressure::drop:
                    lock.unlock();
                    return false;
                default:
                    lock.unlock();
                    throw operation_exception("Unknown backpressure mode.");
            }
        }
    protected:
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            return enqueueWithBackpressure(std::move(pointer), mode);
        }
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode, 
        internals_time timeout_pointer) override
        {
            internals_time_t now_time = std::chrono::system_clock::now();
            if(!timeout_pointer || now_time < *timeout_pointer)
            {
                return internalPush(std::move(pointer), mode);
            }
            return false;
        }
        virtual std::size_t internalPushBatch(std::vector<safety_unit_pointer>&& pointers, 
            backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointers.empty()) throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit_pointers : pointers)
            {
                if (internalPush(std::move(unit_pointers), mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter;
        }
        virtual safety_unit_pointer internalPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);
            auto  check_units_func = [this]()
            {
                return !this->_rankUnitStandard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judgeEmptyCv.wait(lock, check_units_func);
            if(_closed.load(std::memory_order_acquire) && _rankUnitStandard.empty()) return nullptr;
            safety_unit_pointer pointer = std::move(_rankUnitStandard.front());
            _rankUnitStandard.pop_front();
            _judgeFullCv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internalPopBatch(const std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointers;

            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);
            pointers.reserve(count);
            auto  popup_func = [this]()
            {
                return !this->_rankUnitStandard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judgeEmptyCv.wait(lock, popup_func);
            if(_closed.load(std::memory_order_acquire) && this->_rankUnitStandard.empty()) return pointers;
            const std::size_t safety_count = std::min(count, _rankUnitStandard.size());
            auto last_iterator = std::next(_rankUnitStandard.begin(), safety_count);
            auto first = std::make_move_iterator(_rankUnitStandard.begin());
            auto last  = std::make_move_iterator(last_iterator);
            pointers.assign(first, last);
            _rankUnitStandard.erase(_rankUnitStandard.begin(), last_iterator);
            lock.unlock();
            if(count > safety_count)
            {
                //log funtion
            }
            if (safety_count > 0) _judgeFullCv.notify_one();
            return pointers;
        }
        virtual safety_unit_pointer internalTryPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);

            if(_rankUnitStandard.empty()) return nullptr;
            auto pointer = std::move(_rankUnitStandard.front());
            _rankUnitStandard.pop_front();

            _judgeFullCv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internalTryPop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);
            auto  popup_func = [this]()
            {
                return !this->_rankUnitStandard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judgeEmptyCv.wait_for(lock, timeout, popup_func))
            {
                if(_closed.load(std::memory_order_acquire) && _rankUnitStandard.empty()) return nullptr;

                auto pointer = std::move(_rankUnitStandard.front());
                _rankUnitStandard.pop_front();
                lock.unlock();
                _judgeFullCv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internalSize()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankStandardMutex);
            return _rankUnitStandard.size();
        }
        virtual bool internalEmpty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankStandardMutex);
            return _rankUnitStandard.empty();
        }
        virtual void internalClear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankStandardMutex);
            _closed.store(false, std::memory_order_release);
            _maxStorageCapacity.store(0, std::memory_order_release);
            _rankUnitStandard.clear();
        }
        virtual void internalClose() override
        {
            _closed.store(true, std::memory_order_release);
            _judgeEmptyCv.notify_all();
            _judgeFullCv.notify_all();
        }
        virtual rank_strategy internalStrategy()const override
        {
            return rank_strategy::fifo;
        }
        virtual std::size_t internalGetSubQueueCount()const override
        {
            return 0;
        }
        virtual std::size_t internalGetDelayUintCount()const override
        {
            return 0;
        }
    };
    /**
   * @brief 优先级队列
   * @details 优先级队列，根据优先级排序，优先级高的先出队
   * @note 底层容器为`std::multiset`
   */
    class rank_priority : public rank_ordinary
    {
    public:
        explicit rank_priority(std::size_t max_size = 0) : rank_ordinary(max_size) {}
        
        virtual ~rank_priority() = default;
        
    protected:
        class comparator
        {
        public:
            bool operator()(const safety_unit_pointer& first, const safety_unit_pointer& second) const
            {
                return first->get_priority() < second->get_priority();
            }
        };
    protected: 
        std::multiset<safety_unit_pointer,comparator> _rankUnitPriority;

        std::condition_variable_any _judgeEmptyCv;
        std::condition_variable_any _judgeFullCv;

        mutable std::shared_mutex _rankPriorityMutex;
    private:
        bool enqueueWithBackpressure(safety_unit_pointer pointer, backpressure mode)
        {
            std::size_t current_size = 0;
            
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            current_size = _rankUnitPriority.size();
            
            if((_maxStorageCapacity != 0 && current_size >= _maxStorageCapacity) == false)
            {
                _rankUnitPriority.insert(std::move(pointer));
                lock.unlock();
                _judgeEmptyCv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rankUnitPriority.size() < this->_maxStorageCapacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judgeFullCv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rankUnitPriority.insert(std::move(pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                { 
                    if(!_rankUnitPriority.empty())
                    {
                        auto replace_iterator = std::prev(_rankUnitPriority.end());
                        _rankUnitPriority.erase(replace_iterator);
                    }
                    _rankUnitPriority.insert(std::move(pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::exception:
                    lock.unlock();
                    throw operation_exception("The queue is full, please check the overflow policy.");
                case backpressure::drop:
                    lock.unlock();
                    return false;
                default:
                    lock.unlock();
                    throw operation_exception("Unknown backpressure mode.");
            }
        }
    protected:
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            return enqueueWithBackpressure(std::move(pointer), mode);
        }
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode, 
        internals_time timeout_pointer) override
        {
            internals_time_t now_time = std::chrono::system_clock::now();
            if(!timeout_pointer || now_time < *timeout_pointer)
            {
                return internalPush(std::move(pointer), mode);
            }
            return false;
        }
        virtual std::size_t internalPushBatch(std::vector<safety_unit_pointer>&& pointers, 
            backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointers.empty()) throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit_pointers : pointers)
            {
                if (internalPush(std::move(unit_pointers), mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter;
        }
        virtual safety_unit_pointer internalPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            auto check_units_func = [this]()
            {
                return !this->_rankUnitPriority.empty() || this->_closed.load(std::memory_order_acquire);
            }; 
            _judgeEmptyCv.wait(lock, check_units_func);
            if(_closed.load(std::memory_order_acquire) && _rankUnitPriority.empty()) return nullptr;
            
            auto it = _rankUnitPriority.begin();
            if(it == _rankUnitPriority.end()) return nullptr;
            
            safety_unit_pointer pointer = *it;
            _rankUnitPriority.erase(it);
            lock.unlock();
            _judgeFullCv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internalPopBatch(const std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointers;
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            auto popup_func = [this]()
            {
                return !this->_rankUnitPriority.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judgeEmptyCv.wait(lock, popup_func);
            if(_closed.load(std::memory_order_acquire) && _rankUnitPriority.empty()) return pointers;
            const std::size_t safety_count = std::min(count, _rankUnitPriority.size());
            pointers.reserve(safety_count);
            for(std::size_t i = 0; i < safety_count; ++i)
            {
                safety_unit_pointer high_level_value = const_cast<safety_unit_pointer&>(*_rankUnitPriority.begin());
                safety_unit_pointer pointer = std::move(high_level_value);
                pointers.push_back(std::move(pointer));
                _rankUnitPriority.erase(_rankUnitPriority.begin());
            }
            lock.unlock();
            if(count > safety_count)
            {
                //log funtion
            }
            if (safety_count > 0) _judgeFullCv.notify_one();
            return pointers;
        }
        virtual safety_unit_pointer internalTryPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            if(_rankUnitPriority.empty()) return nullptr;
            
            auto it = _rankUnitPriority.begin();
            if(it == _rankUnitPriority.end()) return nullptr;
            
            safety_unit_pointer pointer = *it;
            _rankUnitPriority.erase(it);
            _judgeFullCv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internalTryPop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            auto  popup_func = [this]()
            {
                return !this->_rankUnitPriority.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judgeEmptyCv.wait_for(lock, timeout, popup_func))
            {
                auto it = _rankUnitPriority.begin();
                if(it == _rankUnitPriority.end()) return nullptr;
                
                safety_unit_pointer pointer = *it;
                _rankUnitPriority.erase(it);

                lock.unlock();
                _judgeFullCv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internalSize()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankPriorityMutex);
            return _rankUnitPriority.size();
        }
        virtual bool internalEmpty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankPriorityMutex);
            return _rankUnitPriority.empty();
        }
        virtual void internalClear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankPriorityMutex);
            _closed.store(false, std::memory_order_release);
            _maxStorageCapacity.store(0, std::memory_order_release);
            _rankUnitPriority.clear();
        }
        virtual void internalClose() override
        {
            _closed.store(true, std::memory_order_release);
            _maxStorageCapacity.store(0, std::memory_order_release);
            _judgeEmptyCv.notify_all();
            _judgeFullCv.notify_all();
        }
        virtual rank_strategy internalStrategy()const override
        {
            return rank_strategy::priority;
        }
        virtual std::size_t internalGetSubQueueCount()const override
        {
            return 0;
        }
        virtual std::size_t internalGetDelayUintCount()const override
        {
            return 0;
        }
    };
    /**
   * @brief 延迟队列
   */
    class rank_deferred : public rank_ordinary
    {
    protected:
        class delay_unit
        {
        public:
            safety_unit_pointer _safetyUnitPointer;
            internals_time_t _delayTime;
            delay_unit(safety_unit_pointer safety_unit_pointer,internals_time_t delay_time = internals_clk::now())
            :_safetyUnitPointer(std::move(safety_unit_pointer)),_delayTime(delay_time) {}
        };
        struct comparator
        {
            bool operator()(const std::shared_ptr<delay_unit>& first, const std::shared_ptr<delay_unit>& second)const
            {
                return first->_delayTime > second->_delayTime;
            }
        };
    protected:
        std::jthread _backgroundDetection;

        std::condition_variable_any _judgeEmptyCv;
        std::condition_variable_any _judgeFullCv;

        mutable std::shared_mutex _rankDeferredMutex; 
        std::multiset <std::shared_ptr<delay_unit>,comparator> _rankUnitDeferred;
    private:
        bool enqueueWithBackpressure(std::shared_ptr<delay_unit> struct_pointer, backpressure mode)
        {
            if(struct_pointer == nullptr) 
                throw operation_exception("The incoming pointer is null, please check the parameters passed from the upper layer.");

            std::size_t current_size = 0;
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            current_size = _rankUnitDeferred.size();
            if((_maxStorageCapacity != 0 && current_size >= _maxStorageCapacity) == false)
            {
                _rankUnitDeferred.insert(std::move(struct_pointer));
                lock.unlock();
                _judgeEmptyCv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rankUnitDeferred.size() < this->_maxStorageCapacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judgeFullCv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rankUnitDeferred.insert(std::move(struct_pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                {
                    
                    if(!_rankUnitDeferred.empty())
                    {
                        auto replace_iterator = std::prev(_rankUnitDeferred.end());
                        _rankUnitDeferred.erase(replace_iterator);
                    }
                    _rankUnitDeferred.insert(std::move(struct_pointer));
                    lock.unlock();
                    _judgeEmptyCv.notify_one();
                    return true;
                }
                case backpressure::exception:
                    throw operation_exception("The queue is full, please check the overflow policy.");
                case backpressure::drop:
                    return false;
                default:
                    throw operation_exception("Unknown backpressure mode.");
            }
        }
        void backgroundDetection()
        {
            // 后台检测线程
            while (!_closed.load(std::memory_order_acquire))
            {
                bool has_expired = false;
                std::chrono::system_clock::time_point next_check_time;
                
                {
                    std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
                    if (!_rankUnitDeferred.empty())
                    {
                        auto now = std::chrono::system_clock::now();
                        auto earliest_task = *_rankUnitDeferred.begin();
                        
                        if (earliest_task->_delayTime <= now)
                        {
                            has_expired = true;
                        }
                        else
                        {
                            next_check_time = earliest_task->_delayTime;
                        }
                    }
                }
                
                if (has_expired)
                {
                    _judgeEmptyCv.notify_one();          // 有元素到期，叫醒消费者
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                else if (!_rankUnitDeferred.empty())
                {
                    // 智能等待：等待到下一个任务到期时间，但最多等待10ms
                    auto now = std::chrono::system_clock::now();
                    auto wait_time = std::min(
                        std::chrono::duration_cast<std::chrono::milliseconds>(next_check_time - now),
                        std::chrono::milliseconds(10)
                    );
                    if (wait_time > std::chrono::milliseconds(0))
                    {
                        std::this_thread::sleep_for(wait_time);
                    }
                    else
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
                else
                {
                    // 队列为空时等待更长时间
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    public:
        rank_deferred() = default;
        rank_deferred(std::size_t max_storage_capacity = 0) :rank_ordinary(max_storage_capacity)
        {
            _backgroundDetection = std::jthread(&rank_deferred::backgroundDetection, this);
        }
        ~rank_deferred()
        {
            internalClose();
            if(_backgroundDetection.joinable())
                _backgroundDetection.join();
        }
    protected:
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            std::shared_ptr<delay_unit> small_unit = std::make_shared<delay_unit>(std::move(pointer));
            return enqueueWithBackpressure(small_unit, mode);
        }
        virtual bool internalPush(safety_unit_pointer pointer, backpressure mode, 
            internals_time delay_time) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            std::shared_ptr<delay_unit> small_unit = std::make_shared<delay_unit>(std::move(pointer), *delay_time);
            return enqueueWithBackpressure(small_unit, mode);
        }
        virtual std::size_t internalPushBatch(std::vector<safety_unit_pointer>&& pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointer.empty())  throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit : pointer)
            {
                if (internalPush(unit, mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter; 
        }
        virtual safety_unit_pointer internalPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            if(_rankUnitDeferred.empty() && _closed.load(std::memory_order_acquire)) return nullptr;
            _judgeEmptyCv.wait(lock);
            auto it = _rankUnitDeferred.begin();
            safety_unit_pointer pointer = (*it)->_safetyUnitPointer;
            _rankUnitDeferred.erase(it);
            lock.unlock();
            _judgeFullCv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internalPopBatch(std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointer;
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            const std::size_t safety_count = std::min(count, _rankUnitDeferred.size());
            for(std::size_t i = 0; i < safety_count; i++)
            {
                if(_rankUnitDeferred.empty()) break;
                if((*_rankUnitDeferred.begin())->_delayTime <= internals_clk::now())
                {
                    auto& delay_ptr = const_cast<std::shared_ptr<delay_unit>&>(*_rankUnitDeferred.begin());
                    pointer.push_back(std::move(delay_ptr->_safetyUnitPointer));
                    _rankUnitDeferred.erase(_rankUnitDeferred.begin());
                }
                else
                {
                    _judgeEmptyCv.wait(lock);
                }
            }
            lock.unlock();
            if (safety_count > 0) _judgeFullCv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internalTryPop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            if(_rankUnitDeferred.empty()) return nullptr;
            auto it = _rankUnitDeferred.begin();
            if((*it)->_delayTime < internals_clk::now())
            {
                safety_unit_pointer pointer = (*it)->_safetyUnitPointer;
                _rankUnitDeferred.erase(it);
                lock.unlock();
                _judgeFullCv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual safety_unit_pointer internalTryPop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            auto  popup_func = [this]()
            {
                return !this->_rankUnitDeferred.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judgeEmptyCv.wait_for(lock, timeout, popup_func))
            {
                auto it = _rankUnitDeferred.begin();
                safety_unit_pointer pointer = (*it)->_safetyUnitPointer;
                _rankUnitDeferred.erase(it);

                lock.unlock();
                _judgeFullCv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internalSize()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankDeferredMutex);
            return _rankUnitDeferred.size();
        }
        virtual bool internalEmpty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rankDeferredMutex);
            return _rankUnitDeferred.empty();
        }
        virtual void internalClear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rankDeferredMutex);
            _closed.store(false, std::memory_order_release);
            _maxStorageCapacity.store(0, std::memory_order_release);
            _rankUnitDeferred.clear();
        }
        virtual void internalClose() override
        {
            _closed.store(true, std::memory_order_release);
            _maxStorageCapacity.store(0, std::memory_order_release);
            _judgeEmptyCv.notify_all();
            _judgeFullCv.notify_all();
        }
        virtual rank_strategy internalStrategy()const override
        {
            return rank_strategy::delay;
        }
        virtual std::size_t internalGetSubQueueCount()const override
        {
            return 0;
        }
        virtual std::size_t internalGetDelayUintCount()const override
        {
            return 0;
        }
    };
    /**
   * @brief 任务队列工厂函数 - 创建`FIFO`队列
   * @param max_capacity 最大队列容量
   * @return 队列智能指针
   */
    inline std::shared_ptr<rank_standard> make_rank_standard(std::size_t max_capacity = 0)
    {
        return std::make_shared<rank_standard>(max_capacity);
    }
    /**
   * @brief 任务队列工厂函数 - 创建优先级队列
   * @param max_capacity 最大队列容量
   * @return 队列智能指针
   */
    inline std::shared_ptr<rank_priority> make_rank_priority(std::size_t max_capacity = 0)
    {
        return std::make_shared<rank_priority>(max_capacity);
    }
    /**
   * @brief 任务队列工厂函数 - 创建延迟队列
   * @param max_capacity 最大队列容量
   * @return 队列智能指针
   */
    inline std::shared_ptr<rank_deferred> make_rank_deferred(std::size_t max_capacity = 0)
    {
        return std::make_shared<rank_deferred>(max_capacity);
    }
    /**
   * @brief 任务队列工厂函数 - 根据策略创建队列
   * @param strategy 队列策略
   * @param max_capacity 最大队列容量
   * @return 队列智能指针
   */
    inline std::shared_ptr<rank_ordinary> make_rank(rank_strategy strategy, std::size_t max_capacity = 0)
    {
        switch(strategy)
        {
            case rank_strategy::fifo:
                return make_rank_standard(max_capacity);
            case rank_strategy::priority:
                return make_rank_priority(max_capacity);
            case rank_strategy::delay:
                return make_rank_deferred(max_capacity);
            default:
                return make_rank_standard(max_capacity);
        }
    }
}
namespace pool 
{
    using internals::structure_r::make_rank;

    using internals::structure_r::make_rank_standard;
    using internals::structure_r::make_rank_priority;
    using internals::structure_r::make_rank_deferred;

    using internals::structure_r::rank_ordinary;
    using internals::structure_r::rank_priority;
    using internals::structure_r::rank_deferred;
    using internals::structure_r::rank_standard;
}