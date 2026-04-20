    #include <pthread.h>
    #include <unistd.h>
    #include <mutex>
    #include <utility>
    namespace wan
    {
        class mutex
        {
        private:
            pthread_mutex_t *_thread_mutex;
            bool _owns_mutex = false; //标识锁状态

        public:
            mutex() 
            : _thread_mutex(new pthread_mutex_t), _owns_mutex(true)
            {
                if(pthread_mutex_init(_thread_mutex, nullptr) != 0)
                {
                    delete _thread_mutex;
                    throw std::runtime_error("初始化失败");
                }
            }
            explicit mutex(pthread_mutex_t *_mutex_pointer)
            : _thread_mutex(_mutex_pointer), _owns_mutex(false) 
            {
                if(_mutex_pointer == nullptr)
                {
                    throw std::runtime_error("锁指针为空检查传入情况");
                }
            }
            mutex(const mutex &) = delete;
            mutex &operator=(const mutex &) = delete;
            mutex(mutex &&temporary_mutex)
            {
                if (temporary_mutex._thread_mutex != nullptr)
                {
                    _thread_mutex = temporary_mutex._thread_mutex;
                    temporary_mutex._thread_mutex = nullptr;
                    _owns_mutex = temporary_mutex._owns_mutex;
                    temporary_mutex._owns_mutex = false;
                }
            }
            mutex &operator=(mutex &&temporary_mutex)
            {
                if (this != &temporary_mutex)
                {
                    if (_owns_mutex != false && _thread_mutex != nullptr)
                    {
                        pthread_mutex_destroy(_thread_mutex);
                    }
                    _thread_mutex = temporary_mutex._thread_mutex;
                    _owns_mutex = temporary_mutex._owns_mutex; 
                    temporary_mutex._thread_mutex = nullptr;
                    temporary_mutex._owns_mutex = false;
                }
                return *this;
            }
            void lock()
            {
                if(_thread_mutex == nullptr)
                {
                    throw std::runtime_error("pthread_mutex_t类型为空，注意初始化");
                }
                int result = pthread_mutex_lock(_thread_mutex);
                if(result != 0)
                {
                    throw std::runtime_error("上锁失败");
                }
            }
            void unlock()
            {
                if(_thread_mutex == nullptr)
                {
                    throw std::runtime_error("pthread_mutex_t类型为空，当前值为空");
                }
                int result = pthread_mutex_unlock(_thread_mutex);
                if (result != 0)
                {
                    throw std::runtime_error("解锁失败");
                }
            }
            pthread_mutex_t* get_mutex()
            {
                return _thread_mutex;
            }
            ~mutex()
            {
                if (_owns_mutex)
                {
                    pthread_mutex_destroy(_thread_mutex);
                    delete _thread_mutex;
                    _thread_mutex = nullptr;
                }
            }
        };
        template <typename concurrent_mutex>
        class lock_guard
        {
        private:
            concurrent_mutex &_concurrent_mutex;

        public:
            lock_guard(concurrent_mutex& exterior_mutex)
            :_concurrent_mutex(exterior_mutex)
            {
                _concurrent_mutex.lock();
            }
            lock_guard(const lock_guard&) = delete;
            lock_guard& operator=(const lock_guard&) = delete;
            lock_guard(lock_guard&&) = delete;
            lock_guard& operator=(lock_guard&&) = delete;
            ~lock_guard() noexcept
            {
                try
                {
                    _concurrent_mutex.unlock();
                }
                catch(...)
                {
                    
                }
            }
        };
    }
