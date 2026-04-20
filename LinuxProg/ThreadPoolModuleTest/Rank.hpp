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
    internals_time internal_calculation_deadline()
    {
        if(!_unit_time_limit)
        {
            return nullptr;
        }
        internals_time_t now_time = std::chrono::system_clock::now() + _default_function_timeout;
        return std::make_shared<internals_time_t>(now_time);
    }

    protected:

        std::atomic<bool> _closed{false}; //关闭标识
        std::atomic<bool> _unit_time_limit{false}; //执行单元时间限制
        std::atomic<std::size_t> _max_storage_capacity{0}; //最大队列大小
        std::chrono::milliseconds _default_function_timeout{1000}; //默认延时时间 

    protected:
        // 内部推送任务接口
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode, 
        internals_time deadline  = nullptr)
        {
            parameter_discard(pointer);  parameter_discard(mode);
            parameter_discard(deadline); macro_statement;
            return false;
        }
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode)
        {
            parameter_discard(pointer); parameter_discard(mode); macro_statement;
            return false;
        }
        // 内部批量推送任务接口
        virtual std::size_t internal_push_batch(std::vector<safety_unit_pointer>&& pointers,
       backpressure mode)
        {
            parameter_discard(pointers); parameter_discard(mode); macro_statement;
            return std::size_t(0);
        }
        // 内部弹出任务接口
        virtual safety_unit_pointer internal_pop()
        {
            macro_statement;
            return nullptr;
        }
        // 内部批量弹出任务接口
        virtual std::vector<safety_unit_pointer> internal_pop_batch(const std::size_t count)
        {
            parameter_discard(count); macro_statement;
            return {};
        }
        // 内部尝试弹出任务接口
        virtual safety_unit_pointer internal_try_pop()
        {
            macro_statement;
            return nullptr;
        }
        // 内部尝试弹出任务接口（带超时）
        virtual safety_unit_pointer internal_try_pop_for(const std::chrono::milliseconds& timeout)
        {
            parameter_discard(timeout); macro_statement;
            return nullptr;
        }
        // 内部获取队列大小接口
        virtual std::size_t internal_size() const
        {
            macro_statement;
            return 0;
        }
        // 内部判断队列是否为空接口
        virtual bool internal_empty() const
        {
            macro_statement;
            return true;
        }
        // 内部清空队列接口
        virtual void internal_clear()
        {
            macro_statement;
            return;
        }
        // 内部关闭接口
        virtual void internal_close()
        {
            macro_statement;
            return;
        }
        // 内部获取子队列数量接口
        virtual std::size_t internal_get_sub_queue_count() const
        {
            macro_statement;
            return 0;
        }
        // 内部获取延迟执行单元数量接口
        virtual std::size_t internal_get_delay_uint_count() const
        {
            macro_statement;
            return 0;
        }
        // 内部获取调度策略接口
        virtual rank_strategy internal_strategy() const
        {
            macro_statement;
            return rank_strategy::fifo;
        }
    public:
        rank_ordinary(const std::size_t size) :_max_storage_capacity(size) {} 

        virtual ~rank_ordinary() = default;

        rank_strategy strategy() const 
        { 
            return internal_strategy(); 
        }

        bool push(safety_unit_pointer pointer, backpressure mode = backpressure::block) 
        {
            if(strategy() == rank_strategy::delay) 
            {
                return internal_push(std::move(pointer), mode, internal_calculation_deadline());
            }
            return internal_push(std::move(pointer), mode,nullptr);
        }

        bool push(safety_unit_pointer pointer, std::chrono::system_clock::time_point deadline,
        backpressure mode = backpressure::block)
        {
            internals_time time_point = std::make_shared<std::chrono::system_clock::time_point>(deadline);
            return internal_push(std::move(pointer), mode, time_point);
        }

        std::size_t push_batch(std::vector<safety_unit_pointer> pointers, backpressure mode = backpressure::block)
        {
            return internal_push_batch(std::move(pointers), mode);
        }

        safety_unit_pointer pop()
        {
            return internal_pop();
        }

        std::vector<safety_unit_pointer> pop_batch(const std::size_t count)
        {
            return internal_pop_batch(count);
        }

        safety_unit_pointer try_pop()
        {
            return internal_try_pop();
        }
        template<typename rep, typename period>
        safety_unit_pointer try_pop_for(const std::chrono::duration<rep, period>& timeout)
        {
            return internal_try_pop_for(convert_time::to_milliseconds(timeout));
        }

        std::size_t size() const
        {
            return internal_size();
        }

        bool empty() const 
        { 
            return internal_empty(); 
        }

        void clear() 
        {
            internal_clear();
        }
        // 关闭提交，拒绝新任务提交
        virtual void close() 
        {
            internal_close();
        }
        
        bool closed() const 
        { 
            return _closed.load(std::memory_order_acquire); 
        }
        bool set_max_size(const std::size_t max_size)
        {
            _max_storage_capacity.store(max_size, std::memory_order_relaxed);
            return true;
        }
        std::size_t get_max_size()const  
        {
            return _max_storage_capacity.load();
        }

        std::size_t get_sub_queue_count()  const 
        { 
            return internal_get_sub_queue_count(); 
        }

        std::size_t get_delay_uint_count() const 
        { 
            return internal_get_delay_uint_count(); 
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

        std::deque<safety_unit_pointer> _rank_unit_standard;

        std::condition_variable_any _judge_full_cv;
        std::condition_variable_any _judge_empty_cv;

        mutable std::shared_mutex _rank_standard_mutex;

    public:
        explicit rank_standard(std::size_t max_size = 0) : rank_ordinary(max_size) {}

        virtual ~rank_standard() = default;

    private:
        bool enqueue_with_backpressure(safety_unit_pointer pointer, backpressure mode)
        {
            std::size_t current_size = 0;
            
            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);
            current_size = _rank_unit_standard.size();
            
            if((_max_storage_capacity != 0 && current_size >= _max_storage_capacity) == false)
            {
                _rank_unit_standard.push_back(std::move(pointer));
                lock.unlock();
                _judge_empty_cv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rank_unit_standard.size() < this->_max_storage_capacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judge_full_cv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rank_unit_standard.push_back(std::move(pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                {
                    if(!_rank_unit_standard.empty())  _rank_unit_standard.pop_back();
                    _rank_unit_standard.push_back(std::move(pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
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
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            return enqueue_with_backpressure(std::move(pointer), mode);
        }
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode, 
        internals_time timeout_pointer) override
        {
            internals_time_t now_time = std::chrono::system_clock::now();
            if(!timeout_pointer || now_time < *timeout_pointer)
            {
                return internal_push(std::move(pointer), mode);
            }
            return false;
        }
        virtual std::size_t internal_push_batch(std::vector<safety_unit_pointer>&& pointers, 
            backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointers.empty()) throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit_pointers : pointers)
            {
                if (internal_push(std::move(unit_pointers), mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter;
        }
        virtual safety_unit_pointer internal_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);
            auto  check_units_func = [this]()
            {
                return !this->_rank_unit_standard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judge_empty_cv.wait(lock, check_units_func);
            if(_closed.load(std::memory_order_acquire) && _rank_unit_standard.empty()) return nullptr;
            safety_unit_pointer pointer = std::move(_rank_unit_standard.front());
            _rank_unit_standard.pop_front();
            _judge_full_cv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internal_pop_batch(const std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointers;

            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);
            pointers.reserve(count);
            auto  popup_func = [this]()
            {
                return !this->_rank_unit_standard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judge_empty_cv.wait(lock, popup_func);
            if(_closed.load(std::memory_order_acquire) && this->_rank_unit_standard.empty()) return pointers;
            const std::size_t safety_count = std::min(count, _rank_unit_standard.size());
            auto last_iterator = std::next(_rank_unit_standard.begin(), safety_count);
            auto first = std::make_move_iterator(_rank_unit_standard.begin());
            auto last  = std::make_move_iterator(last_iterator);
            pointers.assign(first, last);
            _rank_unit_standard.erase(_rank_unit_standard.begin(), last_iterator);
            lock.unlock();
            if(count > safety_count)
            {
                //log funtion
            }
            if (safety_count > 0) _judge_full_cv.notify_one();
            return pointers;
        }
        virtual safety_unit_pointer internal_try_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);

            if(_rank_unit_standard.empty()) return nullptr;
            auto pointer = std::move(_rank_unit_standard.front());
            _rank_unit_standard.pop_front();

            _judge_full_cv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internal_try_pop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);
            auto  popup_func = [this]()
            {
                return !this->_rank_unit_standard.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judge_empty_cv.wait_for(lock, timeout, popup_func))
            {
                if(_closed.load(std::memory_order_acquire) && _rank_unit_standard.empty()) return nullptr;

                auto pointer = std::move(_rank_unit_standard.front());
                _rank_unit_standard.pop_front();
                lock.unlock();
                _judge_full_cv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internal_size()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_standard_mutex);
            return _rank_unit_standard.size();
        }
        virtual bool internal_empty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_standard_mutex);
            return _rank_unit_standard.empty();
        }
        virtual void internal_clear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_standard_mutex);
            _closed.store(false, std::memory_order_release);
            _max_storage_capacity.store(0, std::memory_order_release);
            _rank_unit_standard.clear();
        }
        virtual void internal_close() override
        {
            _closed.store(true, std::memory_order_release);
            _judge_empty_cv.notify_all();
            _judge_full_cv.notify_all();
        }
        virtual rank_strategy internal_strategy()const override
        {
            return rank_strategy::fifo;
        }
        virtual std::size_t internal_get_sub_queue_count()const override
        {
            return 0;
        }
        virtual std::size_t internal_get_delay_uint_count()const override
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
        std::multiset<safety_unit_pointer,comparator> _rank_unit_priority;

        std::condition_variable_any _judge_empty_cv;
        std::condition_variable_any _judge_full_cv;

        mutable std::shared_mutex _rank_priority_mutex;
    private:
        bool enqueue_with_backpressure(safety_unit_pointer pointer, backpressure mode)
        {
            std::size_t current_size = 0;
            
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            current_size = _rank_unit_priority.size();
            
            if((_max_storage_capacity != 0 && current_size >= _max_storage_capacity) == false)
            {
                _rank_unit_priority.insert(std::move(pointer));
                lock.unlock();
                _judge_empty_cv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rank_unit_priority.size() < this->_max_storage_capacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judge_full_cv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rank_unit_priority.insert(std::move(pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                { 
                    if(!_rank_unit_priority.empty())
                    {
                        auto replace_iterator = std::prev(_rank_unit_priority.end());
                        _rank_unit_priority.erase(replace_iterator);
                    }
                    _rank_unit_priority.insert(std::move(pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
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
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            return enqueue_with_backpressure(std::move(pointer), mode);
        }
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode, 
        internals_time timeout_pointer) override
        {
            internals_time_t now_time = std::chrono::system_clock::now();
            if(!timeout_pointer || now_time < *timeout_pointer)
            {
                return internal_push(std::move(pointer), mode);
            }
            return false;
        }
        virtual std::size_t internal_push_batch(std::vector<safety_unit_pointer>&& pointers, 
            backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointers.empty()) throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit_pointers : pointers)
            {
                if (internal_push(std::move(unit_pointers), mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter;
        }
        virtual safety_unit_pointer internal_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            auto check_units_func = [this]()
            {
                return !this->_rank_unit_priority.empty() || this->_closed.load(std::memory_order_acquire);
            }; 
            _judge_empty_cv.wait(lock, check_units_func);
            if(_closed.load(std::memory_order_acquire) && _rank_unit_priority.empty()) return nullptr;
            
            auto it = _rank_unit_priority.begin();
            if(it == _rank_unit_priority.end()) return nullptr;
            
            safety_unit_pointer pointer = *it;
            _rank_unit_priority.erase(it);
            lock.unlock();
            _judge_full_cv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internal_pop_batch(const std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointers;
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            auto popup_func = [this]()
            {
                return !this->_rank_unit_priority.empty() || this->_closed.load(std::memory_order_acquire);
            };
            _judge_empty_cv.wait(lock, popup_func);
            if(_closed.load(std::memory_order_acquire) && _rank_unit_priority.empty()) return pointers;
            const std::size_t safety_count = std::min(count, _rank_unit_priority.size());
            pointers.reserve(safety_count);
            for(std::size_t i = 0; i < safety_count; ++i)
            {
                safety_unit_pointer high_level_value = const_cast<safety_unit_pointer&>(*_rank_unit_priority.begin());
                safety_unit_pointer pointer = std::move(high_level_value);
                pointers.push_back(std::move(pointer));
                _rank_unit_priority.erase(_rank_unit_priority.begin());
            }
            lock.unlock();
            if(count > safety_count)
            {
                //log funtion
            }
            if (safety_count > 0) _judge_full_cv.notify_one();
            return pointers;
        }
        virtual safety_unit_pointer internal_try_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            if(_rank_unit_priority.empty()) return nullptr;
            
            auto it = _rank_unit_priority.begin();
            if(it == _rank_unit_priority.end()) return nullptr;
            
            safety_unit_pointer pointer = *it;
            _rank_unit_priority.erase(it);
            _judge_full_cv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internal_try_pop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            auto  popup_func = [this]()
            {
                return !this->_rank_unit_priority.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judge_empty_cv.wait_for(lock, timeout, popup_func))
            {
                auto it = _rank_unit_priority.begin();
                if(it == _rank_unit_priority.end()) return nullptr;
                
                safety_unit_pointer pointer = *it;
                _rank_unit_priority.erase(it);

                lock.unlock();
                _judge_full_cv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internal_size()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_priority_mutex);
            return _rank_unit_priority.size();
        }
        virtual bool internal_empty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_priority_mutex);
            return _rank_unit_priority.empty();
        }
        virtual void internal_clear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_priority_mutex);
            _closed.store(false, std::memory_order_release);
            _max_storage_capacity.store(0, std::memory_order_release);
            _rank_unit_priority.clear();
        }
        virtual void internal_close() override
        {
            _closed.store(true, std::memory_order_release);
            _max_storage_capacity.store(0, std::memory_order_release);
            _judge_empty_cv.notify_all();
            _judge_full_cv.notify_all();
        }
        virtual rank_strategy internal_strategy()const override
        {
            return rank_strategy::priority;
        }
        virtual std::size_t internal_get_sub_queue_count()const override
        {
            return 0;
        }
        virtual std::size_t internal_get_delay_uint_count()const override
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
            safety_unit_pointer _safety_unit_pointer;
            internals_time_t _delay_time;
            delay_unit(safety_unit_pointer safety_unit_pointer,internals_time_t delay_time = internals_clk::now())
            :_safety_unit_pointer(std::move(safety_unit_pointer)),_delay_time(delay_time) {}
        };
        struct comparator
        {
            bool operator()(const std::shared_ptr<delay_unit>& first, const std::shared_ptr<delay_unit>& second)const
            {
                return first->_delay_time > second->_delay_time;
            }
        };
    protected:
        std::jthread _background_detection;

        std::condition_variable_any _judge_empty_cv;
        std::condition_variable_any _judge_full_cv;

        mutable std::shared_mutex _rank_deferred_mutex; 
        std::multiset <std::shared_ptr<delay_unit>,comparator> _rank_unit_deferred;
    private:
        bool enqueue_with_backpressure(std::shared_ptr<delay_unit> struct_pointer, backpressure mode)
        {
            if(struct_pointer == nullptr) 
                throw operation_exception("The incoming pointer is null, please check the parameters passed from the upper layer.");

            std::size_t current_size = 0;
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            current_size = _rank_unit_deferred.size();
            if((_max_storage_capacity != 0 && current_size >= _max_storage_capacity) == false)
            {
                _rank_unit_deferred.insert(std::move(struct_pointer));
                lock.unlock();
                _judge_empty_cv.notify_one();
                return true;
            }
            switch(mode)
            {
                case backpressure::block:
                {
                    auto block_func = [this]()
                    {
                        return this->_rank_unit_deferred.size() < this->_max_storage_capacity
                        || this->_closed.load(std::memory_order_acquire);
                    };
                    _judge_full_cv.wait(lock, block_func);
                    if(_closed.load(std::memory_order_acquire)) return false;
                    _rank_unit_deferred.insert(std::move(struct_pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
                    return true;
                }
                case backpressure::overwrite:
                {
                    
                    if(!_rank_unit_deferred.empty())
                    {
                        auto replace_iterator = std::prev(_rank_unit_deferred.end());
                        _rank_unit_deferred.erase(replace_iterator);
                    }
                    _rank_unit_deferred.insert(std::move(struct_pointer));
                    lock.unlock();
                    _judge_empty_cv.notify_one();
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
        void background_detection()
        {
            // 后台检测线程
            while (!_closed.load(std::memory_order_acquire))
            {
                bool has_expired = false;
                std::chrono::system_clock::time_point next_check_time;
                
                {
                    std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
                    if (!_rank_unit_deferred.empty())
                    {
                        auto now = std::chrono::system_clock::now();
                        auto earliest_task = *_rank_unit_deferred.begin();
                        
                        if (earliest_task->_delay_time <= now)
                        {
                            has_expired = true;
                        }
                        else
                        {
                            next_check_time = earliest_task->_delay_time;
                        }
                    }
                }
                
                if (has_expired)
                {
                    _judge_empty_cv.notify_one();          // 有元素到期，叫醒消费者
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                else if (!_rank_unit_deferred.empty())
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
            _background_detection = std::jthread(&rank_deferred::background_detection, this);
        }
        ~rank_deferred()
        {
            internal_close();
            if(_background_detection.joinable())
                _background_detection.join();
        }
    protected:
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            std::shared_ptr<delay_unit> small_unit = std::make_shared<delay_unit>(std::move(pointer));
            return enqueue_with_backpressure(small_unit, mode);
        }
        virtual bool internal_push(safety_unit_pointer pointer, backpressure mode, 
            internals_time delay_time) override
        {
            if(_closed.load(std::memory_order_acquire)) return false;
            if(pointer == nullptr) return false;
            std::shared_ptr<delay_unit> small_unit = std::make_shared<delay_unit>(std::move(pointer), *delay_time);
            return enqueue_with_backpressure(small_unit, mode);
        }
        virtual std::size_t internal_push_batch(std::vector<safety_unit_pointer>&& pointer, backpressure mode) override
        {
            if(_closed.load(std::memory_order_acquire)) return 0;
            if(pointer.empty())  throw operation_exception("The vector pointers is empty.");
            std::size_t complete_push_unit_counter = 0;
            for(auto& unit : pointer)
            {
                if (internal_push(unit, mode))
                {
                    complete_push_unit_counter++;
                }
            }
            return complete_push_unit_counter; 
        }
        virtual safety_unit_pointer internal_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            if(_rank_unit_deferred.empty() && _closed.load(std::memory_order_acquire)) return nullptr;
            _judge_empty_cv.wait(lock);
            auto it = _rank_unit_deferred.begin();
            safety_unit_pointer pointer = (*it)->_safety_unit_pointer;
            _rank_unit_deferred.erase(it);
            lock.unlock();
            _judge_full_cv.notify_one();
            return pointer;
        }
        virtual std::vector<safety_unit_pointer> internal_pop_batch(std::size_t count) override
        {
            std::vector<safety_unit_pointer> pointer;
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            const std::size_t safety_count = std::min(count, _rank_unit_deferred.size());
            for(std::size_t i = 0; i < safety_count; i++)
            {
                if(_rank_unit_deferred.empty()) break;
                if((*_rank_unit_deferred.begin())->_delay_time <= internals_clk::now())
                {
                    auto& delay_ptr = const_cast<std::shared_ptr<delay_unit>&>(*_rank_unit_deferred.begin());
                    pointer.push_back(std::move(delay_ptr->_safety_unit_pointer));
                    _rank_unit_deferred.erase(_rank_unit_deferred.begin());
                }
                else
                {
                    _judge_empty_cv.wait(lock);
                }
            }
            lock.unlock();
            if (safety_count > 0) _judge_full_cv.notify_one();
            return pointer;
        }
        virtual safety_unit_pointer internal_try_pop() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            if(_rank_unit_deferred.empty()) return nullptr;
            auto it = _rank_unit_deferred.begin();
            if((*it)->_delay_time < internals_clk::now())
            {
                safety_unit_pointer pointer = (*it)->_safety_unit_pointer;
                _rank_unit_deferred.erase(it);
                lock.unlock();
                _judge_full_cv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual safety_unit_pointer internal_try_pop_for(const std::chrono::milliseconds& timeout) override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            auto  popup_func = [this]()
            {
                return !this->_rank_unit_deferred.empty() || this->_closed.load(std::memory_order_acquire);
            };
            if(_judge_empty_cv.wait_for(lock, timeout, popup_func))
            {
                auto it = _rank_unit_deferred.begin();
                safety_unit_pointer pointer = (*it)->_safety_unit_pointer;
                _rank_unit_deferred.erase(it);

                lock.unlock();
                _judge_full_cv.notify_one();
                return pointer;
            }
            return nullptr;
        }
        virtual std::size_t internal_size()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            return _rank_unit_deferred.size();
        }
        virtual bool internal_empty()const override
        {
            std::shared_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            return _rank_unit_deferred.empty();
        }
        virtual void internal_clear() override
        {
            std::unique_lock<std::shared_mutex> lock(_rank_deferred_mutex);
            _closed.store(false, std::memory_order_release);
            _max_storage_capacity.store(0, std::memory_order_release);
            _rank_unit_deferred.clear();
        }
        virtual void internal_close() override
        {
            _closed.store(true, std::memory_order_release);
            _max_storage_capacity.store(0, std::memory_order_release);
            _judge_empty_cv.notify_all();
            _judge_full_cv.notify_all();
        }
        virtual rank_strategy internal_strategy()const override
        {
            return rank_strategy::delay;
        }
        virtual std::size_t internal_get_sub_queue_count()const override
        {
            return 0;
        }
        virtual std::size_t internal_get_delay_uint_count()const override
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