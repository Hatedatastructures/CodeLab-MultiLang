#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
static constexpr uint64_t CACHE_ALIGNMENT = 64;
namespace con
{
    /**
    * @brief #### 单生产单消费无锁队列类`SPSC`
    * @tparam  object_type 数据类型
    * @warning 在严格 `SPSC` 场景下，容器保证线程安全
    **/
    template<typename object_type>
    class pro_con_queue
    {
    private:
        std::atomic<uint64_t> producer;
        std::atomic<uint64_t> consumer;
        const uint64_t currentCapacity;
        static constexpr uint64_t defaultCapacity = 10;
        alignas(CACHE_ALIGNMENT) std::vector<object_type> sharedCircularQueue;
        uint64_t computePosition(uint64_t index) const
        { 
            return index % currentCapacity;
        }
    public:
        explicit pro_con_queue(const uint64_t newCapacity = defaultCapacity):producer(0),consumer(0),
        currentCapacity(std::max(newCapacity,static_cast<uint64_t>(1))),sharedCircularQueue(currentCapacity){}
        pro_con_queue(const pro_con_queue& anotherQueue) = delete;
        pro_con_queue(pro_con_queue&& anotherQueue) noexcept = delete;
        pro_con_queue& operator=(const pro_con_queue& anotherQueue) = delete;
        pro_con_queue& operator=(pro_con_queue&& anotherQueue) noexcept = delete;
        /**
     * @brief #### 向队列添加数据
     * @param produceData 生产数据
     * @return 是否成功
     */
        bool push(object_type&& produceData)
        {
            const uint64_t currentProducer = producer.fetch_add(1,std::memory_order_relaxed);
            const uint64_t position = computePosition(currentProducer);
            if(currentProducer - consumer.load(std::memory_order_acquire) >= currentCapacity)
            {
                producer.fetch_sub(1,std::memory_order_release);
                return false;
            }
            sharedCircularQueue[position] = std::forward<object_type>(produceData);
            std::atomic_thread_fence(std::memory_order_release);
            return true;
        }
        /**
     * @brief #### 从队列中取出数据
     * @param consumeData 
     * @return 是否成功
     */
        bool pop(object_type& consumeData)
        {
            const uint64_t currentConsumer = consumer.fetch_add(1,std::memory_order_relaxed);
            const uint64_t position = computePosition(currentConsumer);
            if(currentConsumer >= producer.load(std::memory_order_acquire))
            {
                consumer.fetch_sub(1,std::memory_order_release);
                return false;
            }
            std::atomic_thread_fence(std::memory_order_acquire);
            consumeData = sharedCircularQueue[position];
            return true;
        }
        bool empty() const
        {
            return consumer.load(std::memory_order_acquire) == producer.load(std::memory_order_acquire);
        }
        bool full() const
        {
            return producer.load(std::memory_order_acquire) - consumer.load(std::memory_order_acquire) == currentCapacity;
        }
        uint64_t size() const
        {
            return producer.load(std::memory_order_acquire) - consumer.load(std::memory_order_acquire);
        }
    };
    /**
   * @brief #### 多生产多消费有锁队列类`MPMC`
   * @tparam object_type 数据类型
   */
    template<typename object_type>
    class pros_cons_queue
    {
        static constexpr uint64_t defaultCapacity = 10;
    private:
        mutable std::mutex accessMutex;
        uint64_t currentCapacity;
        std::atomic<bool> closeId = false;
        std::condition_variable produceCondition;
        std::condition_variable consumeCondition;
        alignas(CACHE_ALIGNMENT) std::queue<object_type> sharedQueue;
        /**
     * @brief 判断队列是否满
     * @return 满返回 `true`，否则返回 `false`
     */
        bool fullInternal() const
        {
            return sharedQueue.size() == currentCapacity;
        }
        /**
     * @brief 判断队列是否为空
     * @return 空返回 `true`，否则返回 `false`
     */
        bool emptyInternal() const
        {
            return sharedQueue.empty();
        }
        /**
     * @brief 入队，不保证线程安全
     * @tparam enqueueType
     */
        template<typename enqueueType>
        void enqueue(enqueueType && produceData)
        {
            sharedQueue.push(std::forward<enqueueType>(produceData));
        }
    public:
        pros_cons_queue(const uint64_t newCapacity = defaultCapacity)
        :currentCapacity(std::max(newCapacity,static_cast<uint64_t>(1))){}
        pros_cons_queue(const pros_cons_queue& anotherQueue) = delete;
        pros_cons_queue(pros_cons_queue&& anotherQueue) noexcept = delete;
        pros_cons_queue& operator=(const pros_cons_queue& anotherQueue) = delete;
        pros_cons_queue& operator=(pros_cons_queue&& anotherQueue) noexcept = delete;
        /**
     * @brief  #### 阻塞式入队，队列满时等待
     * @tparam pushType
     * @param produceData
     * @return 成功返回 `true`，队列关闭已关闭返回 `false`
     */
        template<typename pushType>
        bool push(pushType&& produceData)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex);
            while(fullInternal() && !closeId)
            {
                produceCondition.wait(accessLock);
            }
            if(closeId) return false;
            enqueue(std::forward<pushType>(produceData));
            accessLock.unlock();
            consumeCondition.notify_one();
            return true;
        }
        /**
     * @brief  #### 非阻塞式入队，不等待
     * @tparam tryPushType
     * @param produceData
     * @return 成功返回 `true`，队列关闭已关闭返回 `false`
     */
        template<typename tryPushType>
        bool tryPush(tryPushType&& produceData)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex,std::try_to_lock);
            if(!accessLock.owns_lock()) return false;
            if(closeId.load(std::memory_order_relaxed) || fullInternal()) return false;
            enqueue(std::forward<tryPushType>(produceData));
            accessLock.unlock();
            consumeCondition.notify_one();
            return true;
        }
        /**
     * @brief  #### 限时入队，超时返回
     * @tparam precision
     * @tparam period
     * @param produceData
     * @param timeOut
     * @return 成功返回 `true`，超时 / 关闭返回 `false`
     */
        template<typename pushForType, typename precision, typename period>
        bool pushFor(pushForType&& produceData,const std::chrono::duration<precision, period> timeOut)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex);
            auto status = produceCondition.wait_for(accessLock,timeOut,[this](){return !fullInternal() || closeId;});
            if(closeId || status == std::cv_status::timeout) return false;
            enqueue(std::forward<pushForType>(produceData));
            accessLock.unlock();
            consumeCondition.notify_one();
            return true;
        }
        /**
     * @brief #### 阻塞式出队，队列空时等待
     * @return 成功返回 `true`，失败（锁竞争 / 空 / 关闭）返回 `false`
     */
        bool pop(object_type& consumeData)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex);
            while(emptyInternal() && !closeId)
            {
                consumeCondition.wait(accessLock);
            }
            if(closeId && emptyInternal()) return false;
            consumeData = std::move(sharedQueue.front());
            sharedQueue.pop();
            accessLock.unlock();
            produceCondition.notify_one();
            return true;
        }
        /**
     * @brief  #### 非阻塞式出队，不等待
     * @return 成功返回 `true`，失败（锁竞争 / 空 / 关闭）返回 `false`
     */
        bool tryPop(object_type& consumeData)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex,std::try_to_lock);
            if(!accessLock.owns_lock()) return false;
            if(closeId.load(std::memory_order_relaxed) || emptyInternal()) return false;
            consumeData = sharedQueue.front();
            sharedQueue.pop();
            accessLock.unlock();
            produceCondition.notify_one();
            return true;
        }
        /**
     * @brief  #### 限时出队，超时返回
     * @tparam precision 
     * @tparam period 
     * @param consumeData 
     * @param timeOut 
     * @return 成功返回 `true`，超时 / 关闭返回 `false`
     */
        template<typename precision, typename period>
        bool popFor(object_type& consumeData,const std::chrono::duration<precision, period> timeOut)
        {
            std::unique_lock<std::mutex> accessLock(accessMutex);
            auto status = consumeCondition.wait_for(accessLock,timeOut,[this](){return !emptyInternal() || closeId;});
            if((closeId && emptyInternal()) || status == std::cv_status::timeout) return false;
            consumeData = sharedQueue.front();
            sharedQueue.pop();
            accessLock.unlock();
            produceCondition.notify_one();
            return true;
        }
        /**
     * @brief #### 关闭队列，关闭后无法再入队或出队
     * @warning - 关闭后无法再入队
     */
        void close() 
        {
            if(closeId.load(std::memory_order_acquire)) return;
            std::unique_lock<std::mutex> accessLock(accessMutex);
            if(closeId) return;
            closeId.store(true,std::memory_order_release);
            accessLock.unlock();
            produceCondition.notify_all();
            consumeCondition.notify_all();
        }
        void clear()
        {
            std::lock_guard<std::mutex> lock(accessMutex);
            while(!emptyInternal())
            {
                sharedQueue.pop();
            }
        }
        void enable()
        {
            if(!closeId.load(std::memory_order_acquire)) return;
            std::unique_lock<std::mutex> accessLock(accessMutex);
            if(!closeId) return;
            closeId.store(false,std::memory_order_release);
            accessLock.unlock();
            produceCondition.notify_all();
            consumeCondition.notify_all();
        }
        bool full() const
        {
            std::lock_guard<std::mutex> accessLock(accessMutex);
            return fullInternal();
        }
        uint64_t size() const
        {
            std::lock_guard<std::mutex> accessLock(accessMutex);
            return sharedQueue.size();
        }
        bool empty() const
        {
            std::lock_guard<std::mutex> accessLock(accessMutex);
            return emptyInternal();
        }
        /**
     * @brief #### 检测队列是否关闭
     * @return 关闭返回 `true`，未关闭返回 `false`
     */
        bool whetherClose() const
        {
            return closeId.load(std::memory_order_acquire);
        }
        double loadJudgment() const
        {
            std::lock_guard<std::mutex> accessLock(accessMutex);
            return static_cast<double>(sharedQueue.size()) / static_cast<double>(currentCapacity);
        }
    };
    /**
   * @brief #### 多生产多消费有锁双队列类`MPMC`
   * @tparam object_type 数据类型
   */
    template<typename object_type>
    class pro_con_queues
    {
        static constexpr uint64_t defaultCapacity = 10;
    private: 
        uint64_t currentCapacity;
        std::thread supportingThread;
        std::atomic<bool> closeId,switchId;
        std::mutex produceMutex,consumeMutex;
        std::condition_variable produceCondition,consumeCondition;
        std::atomic<std::queue<object_type>*> produce,consume;
        alignas(CACHE_ALIGNMENT) std::queue<object_type> producePipe,consumePipe;
        /**
     * @brief #### 检测队列是否为空
     * @note - 队列空时，交换队列
     */
        void swapQueue()
        {
            if(consume.load(std::memory_order_acquire)->empty())
            {
                auto tmpProduce = produce.load(std::memory_order_relaxed);
                produce.store(consume.load(std::memory_order_relaxed),std::memory_order_release);
                consume.store(tmpProduce,std::memory_order_release);
            }
        }
        /**
     * @brief #### 后台辅助线程
     * @remark - 检测队列情况，交换队列，通知生产者或消费者
     */
        void supportingThreadFunc()
        {
            //循环检测队列情况
            while(!closeId.load(std::memory_order_acquire))
            {
                std::unique_lock<std::mutex> produceLock(produceMutex);
                auto conditionsExchange = [this]()
                {
                    return switchId.load(std::memory_order_acquire) || closeId.load(std::memory_order_acquire);
                };
                produceCondition.wait(produceLock,conditionsExchange);
                if(closeId.load(std::memory_order_acquire)) return;
                {
                    std::lock_guard<std::mutex> consumeLock(consumeMutex);
                    swapQueue();
                }
                switchId.store(false,std::memory_order_release);
                produceCondition.notify_all();
                consumeCondition.notify_one();
            }
        }
        /**
     * @brief #### 刷新队列
     * @remark - 刷新队列，将生产者队列中的数据全部转移到消费者队列中
     */
        void shutdown()
        {
            {
                std::lock_guard<std::mutex> produceLock(produceMutex);
                std::lock_guard<std::mutex> consumeLock(consumeMutex);
                closeId.store(true,std::memory_order_release);
            }
            produceCondition.notify_all();
            consumeCondition.notify_all();
            if(supportingThread.joinable())
            {
                supportingThread.join();
            }
        }

    public:
        pro_con_queues(uint64_t capacity = defaultCapacity)
        :currentCapacity(capacity),closeId(false),switchId(false),produce(&producePipe),consume(&consumePipe)
        {
            auto transmission = [this](){this->supportingThreadFunc();};
            supportingThread = std::thread(transmission);
        }
        pro_con_queues(const pro_con_queues& other) = delete;
        pro_con_queues& operator=(const pro_con_queues& other) = delete;
        pro_con_queues(pro_con_queues&& other) = delete;
        pro_con_queues& operator=(pro_con_queues&& other) = delete;
        /**
     * @brief #### 向队列中添加数据
     * @param produceData 生产数据
     * @return true 添加成功，false 添加失败
     */
        template<typename pushType>
        bool push(pushType&& produceData)
        {
            if(closeId.load(std::memory_order_acquire)) return false;
            while(true)
            {
                { //限制锁粒度，减少锁竞争
                    std::lock_guard<std::mutex> produceLock(produceMutex);
                    if(produce.load(std::memory_order_acquire)->size() < currentCapacity)
                    {
                        produce.load(std::memory_order_acquire)->push(std::forward<pushType>(produceData));
                        return true;
                    }
                }
                flush();
                if(closeId.load(std::memory_order_acquire)) return false;
            }
        }
        /**
     * @brief #### 从队列中取出数据
     * @param consumeData 消费数据
     * @return true 取出成功，false 取出失败
     */
        bool pop(object_type& consumeData)
        {
            std::unique_lock<std::mutex> consumeLock(consumeMutex);
            while (true)
            {
                auto queuePtr = consume.load(std::memory_order_acquire);
                if (!queuePtr->empty())
                {
                    consumeData = std::move(queuePtr->front());
                    queuePtr->pop();
                    return true;
                }
                if (closeId.load(std::memory_order_acquire)) return false;
                consumeCondition.wait(consumeLock);
            }
        }
        /**
     * @brief #### 获取生产者队列的大小
     * @return uint64_t 生产者队列的大小
     */
        uint64_t produceSizeUnsafe()
        {
            std::lock_guard<std::mutex> produceLock(produceMutex);
            return produce.load(std::memory_order_relaxed)->size();
        }
        /**
     * @brief #### 获取消费者队列的大小
     * @return uint64_t 消费者队列的大小
     */
        uint64_t consumeSizeUnsafe()
        {
            std::lock_guard<std::mutex> consumeLock(consumeMutex);
            return consume.load(std::memory_order_relaxed)->size();
        }
        /**
     * @brief #### 关闭队列
     * @remark - 关闭队列，通知生产者和消费者
     */
        void close()
        {
            closeId.store(true,std::memory_order_release);
            produceCondition.notify_all();
            consumeCondition.notify_all();
        }
        /**
     * @brief #### 刷新队列
     * @remark - 刷新队列，将生产者队列中的数据全部转移到消费者队列中
     */
        void flush()
        {
            bool swapFlag = false;
            if(switchId.compare_exchange_strong(swapFlag,true))
            { //比较两个布尔值是否相等,相等则切换为true，函数返回值代表是否已经切换，切换为真，反之为假
                produceCondition.notify_one();
            }
            std::unique_lock<std::mutex> produceLock(produceMutex);
            auto waitingConsumption = [this]()
            {
                return !switchId.load(std::memory_order_acquire) || closeId.load(std::memory_order_acquire);
            };
            produceCondition.wait(produceLock,waitingConsumption);
        }
        ~pro_con_queues()
        {
            flush();
            shutdown();
        }
    };
    /**
   * @brief #### 多生产多消费有锁信号量队列
   * @warning 由于标准库限制队列容量只能写死
   * @tparam object_type  数据类型
   */
    template<typename object_type>
    class proConSemaphoreQueue
    {
    private:

        static constexpr uint64_t largestSemaphore = 10ULL;
        std::vector<object_type> semaphoreQueue;

        std::counting_semaphore<largestSemaphore> produceSemaphore;
        std::counting_semaphore<largestSemaphore> consumeSemaphore;

        std::mutex produceMutex;
        std::mutex consumeMutex;

        uint64_t produceLocation;
        uint64_t consumeLocation;

    public:
        proConSemaphoreQueue()
        :semaphoreQueue(largestSemaphore),produceSemaphore(0),consumeSemaphore(largestSemaphore),
        produceLocation(0),consumeLocation(0){}
        void push(const proConSemaphoreQueue& produceData)
        {
            consumeSemaphore.acquire();
            std::unique_lock<std::mutex> produceLock(produceMutex);
            semaphoreQueue[produceLocation++] = produceData;
            produceLocation = produceLocation % largestSemaphore;
            produceLock.unlock();
            produceSemaphore.release();
        }
        void pop(object_type& consumeData)
        {
            produceSemaphore.acquire();
            std::unique_lock<std::mutex> consumeLock(consumeMutex);
            consumeData = std::move(semaphoreQueue[consumeLocation++]);
            consumeLocation = consumeLocation %  largestSemaphore;
            consumeLock.unlock();
            consumeSemaphore.release();
        }
    };
}
