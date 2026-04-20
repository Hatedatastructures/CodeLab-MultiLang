#pragma once
#include <chrono>
#include <atomic>
#include <string>
/**
 * @brief #### 时间转换工具类
 */
class convert_time
{
public:
    /**
   * @brief #### 将任意时间单位转换为毫秒
   */
    template <typename rep, typename period>
    static std::chrono::milliseconds toMilliseconds(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为秒
   */
    template <typename rep, typename period>
    static std::chrono::seconds toSeconds(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为分钟
   */
    template <typename rep, typename period>
    static std::chrono::minutes toMinutes(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::minutes>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为小时
   */
    template <typename rep, typename period>
    static std::chrono::hours toHours(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::hours>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为天
   */
    template <typename rep, typename period>
    static std::chrono::days toDays(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::days>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为周
   */
    template <typename rep, typename period>
    static std::chrono::weeks toWeeks(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::weeks>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为月
   */
    template <typename rep, typename period>
    static std::chrono::months toMonths(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::months>(duration);
    }
    /**
   * @brief #### 将任意时间单位转换为年
   */
    template <typename rep, typename period>
    static std::chrono::years toYears(const std::chrono::duration<rep, period>& duration)
    {
        return std::chrono::duration_cast<std::chrono::years>(duration);
    }
    /**
   * @brief #### 时间点格式化为 UTC 字符串
   */
    static std::string toUtcString(const std::chrono::system_clock::time_point& tp,
    std::string_view fmt = "%Y-%m-%dT%H:%M:%SZ") noexcept
    {
        const std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm{}; 
#if defined(_WIN32)
        gmtime_s(&tm, &t);
#else
        gmtime_r(&t, &tm);
#endif
        std::array<char, 128> buf{};
        std::strftime(buf.data(), buf.size(), fmt.data(), &tm);
        return std::string(buf.data());
    }
    /**
   * @brief #### 时间点格式化为本地时间字符串
   */
    static std::string toLocalString(const std::chrono::system_clock::time_point& tp,
    std::string_view fmt = "%Y-%m-%d %H:%M:%S") noexcept
    {
        const std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::array<char, 128> buf{};
        std::strftime(buf.data(), buf.size(), fmt.data(), &tm);
        return std::string(buf.data());
    }
};

// #### 任务当前状态枚举
enum class current_status : std::uint8_t
{
    pending = 0,     // 等待执行
    running = 1,     // 正在执行
    completed = 2,   // 执行完成
    cancelled = 3,   // 已取消
    timeout = 4,     // 执行超时
    failed = 5       // 执行失败
};

// #### 任务优先级枚举
enum class weight : std::int32_t
{
    lowest = -100, // 最低优先级
    low = -50,     // 低优先级
    normal = 0,    // 普通优先级
    high = 50,     // 高优先级
    highest = 100, // 最高优先级
    critical = 200 // 关键优先级
};

// #### 队列满时的处理策略枚举
enum class backpressure : std::uint8_t
{ 
    block,     // 阻塞
    drop,      // 丢弃
    overwrite, // 覆盖
    exception  // 抛出
}; 
// #### 任务调度策略枚举
enum class rank_strategy
{
    fifo, // 先进先出
    priority, // 优先级
    delay, // 延迟
    round_robin, // 轮询
};
enum class worker_state
{
    idle,     // 空闲状态 
    running,  // 运行状态 
    stopping, // 停止中   
    stopped,  // 已停止   
    error     // 错误状态 
};
//  调度策略枚举
enum class scheduling_tactics
{
    round_robin,    // 轮询调度   - 平均分配任务
    least_loaded,   // 最少负载   - 分配给负载最轻的线程
    adaptive,       // 自适应调度 - 根据性能动态调整
    priority_based, // 优先级调度 - 基于任务优先级
};
//扩缩容策略枚举
enum class expansion_strategy
{
    conservative, // 保守策略 - 缓慢调整
    aggressive,   // 激进策略 - 快速调整
    reactive,     // 响应策略 - 基于当前负载
    hybrid        // 混合策略 - 结合多种策略
};

enum class pool_state
{
    stopped,  // 已停止
    starting, // 启动中
    running,  // 运行中
    pausing,  // 暂停中
    paused,   // 已暂停
    stopping, // 停止中
    error     // 错误状态
};

inline std::string to_string(current_status state) noexcept
{
    switch (state)
    {
        case current_status::pending:    return "pending";
        case current_status::running:    return "running";
        case current_status::completed:  return "completed";
        case current_status::cancelled:  return "cancelled";
        case current_status::timeout:    return "timeout";
        case current_status::failed:     return "failed";
        default:                         return "unknown";
    }
}

inline std::string to_string(weight level) noexcept
{
    switch (level)
    {
        case weight::lowest:      return "lowest";
        case weight::low:         return "low";
        case weight::normal:      return "normal";
        case weight::high:        return "high";
        case weight::highest:     return "highest";
        case weight::critical:    return "critical";
        default:                         return std::to_string(static_cast<int>(level));
    }
}

/**
 * @class execution_exception
 * @brief #### 执行期间异常类
 */
class execution_exception : public std::exception
{
private:
    std::string _message; // 异常消息
    std::uint64_t _identifier; // 任务ID

public:
    /**
   * @brief 构造函数
   * @param message 异常消息
   * @param task_id 任务`ID`，默认为`0`
   */
    explicit execution_exception(std::string message, std::uint64_t task_id = 0) noexcept
        : _message(std::move(message)), _identifier(task_id)  {}
    
    execution_exception(const execution_exception& other) noexcept
        : _message(other._message), _identifier(other._identifier) {}

    execution_exception(execution_exception&& other) noexcept
        : _message(std::move(other._message)), _identifier(other._identifier) {}
    
    execution_exception& operator=(const execution_exception& other) noexcept
    {
        if (this != &other)
        {
            _message = other._message;
            _identifier = other._identifier;
        }
        return *this;
    }
    
    execution_exception& operator=(execution_exception&& other) noexcept
    {
        if (this != &other)
        {
            _message = std::move(other._message);
            _identifier = other._identifier;
        }
        return *this;
    }
    
    ~execution_exception() override = default;
    
    const char* what() const noexcept override
    {
        return _message.c_str();
    }
    
    std::uint64_t getIdentifier() const noexcept
    {
        return _identifier;
    }
    
    void setIdentifier(std::uint64_t id) noexcept
    {
        _identifier = id;
    }
    
    void swap(execution_exception& other) noexcept
    {
        using std::swap;
        swap(_message, other._message);
        swap(_identifier, other._identifier);
    }
};

/**
 * @brief #### 操作期间异常类
 */
class operation_exception : public std::exception
{
private:
    std::string _message; // 异常消息
    std::chrono::system_clock::time_point _time; // 异常时间点

public:
    operation_exception(std::string message) noexcept
        : _message(std::move(message)), _time(std::chrono::system_clock::now()) {}

    operation_exception(const operation_exception& other) noexcept
        : _message(other._message), _time(other._time) {}

    operation_exception(operation_exception&& other) noexcept
        : _message(std::move(other._message)), _time(other._time) {}

    operation_exception& operator=(const operation_exception& other) noexcept
    {
        if (this != &other)
        {
            _message = other._message;
            _time = other._time;
        }
        return *this;
    }

    operation_exception& operator=(operation_exception&& other) noexcept
    {
        if (this != &other)
        {
            _message = std::move(other._message);
            _time = other._time;
        }
        return *this;
    }

    ~operation_exception() override = default;

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

    std::chrono::system_clock::time_point getTime() const noexcept
    {
        return _time;
    }

    void setTime(std::chrono::system_clock::time_point time) noexcept
    {
        _time = time;
    }

    std::string getTime_string(std::string_view fmt = "%Y-%m-%d %H:%M:%S") const noexcept
    {
        return convert_time::toLocalString(_time, fmt);
    }
};

/**
 * @class derivation
 * @brief 任务返回类型封装类，支持任意类型的返回值，包括 `void` 类型
 * @warning 不支持对 `void` 类型的返回值进行转换
 */
class derivation
{ 
private:

    std::any _data;
    bool _void;

public:
    derivation() : _void(true) {}

    template<typename convert_t>
    derivation(convert_t&& value) 
    : _data(std::forward<convert_t>(value)), _void(false) {}

    derivation(derivation&& other) noexcept 
    : _data(std::move(other._data)), _void(std::move(other._void)) {}

    derivation& operator= (derivation&& other) noexcept
    {
        if(this != &other)
        {
            _data = std::move(other._data);
            _void = other._void;
        }
        return *this;
    }

    derivation(const derivation& ) = delete;

    derivation& operator= (const derivation&) = delete;

    template<typename implicit_type>
    operator implicit_type() const
    {
        static_assert(!std::isVoid_v<implicit_type>, "Cannot convert to void type");
        if(_void)
        {
            throw execution_exception("The void type cannot be converted.",0);
        }
        try
        {
            return std::any_cast<implicit_type>(_data);
        }
        catch(const std::bad_any_cast& conversion_e)
        {
            throw std::runtime_error(std::string("Type conversion failed: ") + conversion_e.what());
        }
    }

    // #### 检查是否为 void 类型
    bool isVoid() const noexcept
    {
        return _void;
    }

    // 检查是否有值
    bool hasValue() const noexcept
    {
        return !_void && _data.hasValue();
    }

    // #### 显式获取任务返回值
    template<typename convert_t>
    auto get() const
    {
        if constexpr (std::isVoid_v<convert_t>)
        {
            if(!_void)
            {
                throw execution_exception("The task has a return value and cannot be obtained as void", 0);
            }
            return;
        } 
        else
        {
            if(_void)
            {
                throw execution_exception("Cannot get value from void derivation", 0);
            }
            try
            {
                return std::any_cast<convert_t>(_data);
            }
            catch(const std::bad_any_cast& conversion_e)
            {
                throw std::runtime_error(std::string("Type conversion failed: ") + conversion_e.what());
            }    
        }
    }

    // 获取存储的类型信息
    const std::type_info& type() const noexcept
    {
        return _void ? typeid(void) : _data.type();
    }

    // 就地构造新值
    template<typename convert_t, typename... Args>
    void emplace(Args&&... args)
    { // void 类型不允许存储值
        static_assert(!std::isVoid_v<convert_t>, "Cannot emplace void type");
        _data.emplace<convert_t>(std::forward<Args>(args)...);
        _void = false;
    }
};
/**
 * @brief 工作线程统计信息
 *
 * 记录工作线程的性能统计数据，用于监控和优化
 */
class worker_statistics
{
public:
    std::atomic<std::uint64_t> tasksFailed{0};           // 执行失败任务数量
    std::atomic<std::uint64_t> tasksExecuted{0};         // 已执行任务数量
    std::atomic<std::uint64_t> totalIdleTime{0};        // 总空闲时间(微秒)
    std::atomic<std::uint64_t> totalExecutionTime{0};   // 总执行时间(微秒)

    std::chrono::steady_clock::time_point startTime;     // 线程启动时间
    std::chrono::steady_clock::time_point lastTaskTime; // 最后任务执行时间

    worker_statistics()
    {
        reset();
    }

    /**
   * @brief 重置统计信息
   */
    void reset()
    {
        tasksFailed.store(0, std::memory_order_relaxed);
        tasksExecuted.store(0, std::memory_order_relaxed);
        totalIdleTime.store(0, std::memory_order_relaxed);
        totalExecutionTime.store(0, std::memory_order_relaxed);
        startTime = std::chrono::steady_clock::now();
        lastTaskTime = startTime;
    }

    /**
   * @brief 获取平均任务执行时间
   * @return 平均执行时间(微秒)
   */
    double getAverageExecutionTime() const
    {
        auto executed = tasksExecuted.load(std::memory_order_relaxed);
        if (executed == 0)
            return 0.0;

        auto total_time = totalExecutionTime.load(std::memory_order_relaxed);
        return static_cast<double>(total_time) / executed;
    }

    /**
   * @brief 获取任务成功率
   * @return 成功率(0.0-1.0)
   */
    double getSuccessRate() const
    {
        auto executed = tasksExecuted.load(std::memory_order_relaxed);
        if (executed == 0)
            return 1.0;

        auto failed = tasksFailed.load(std::memory_order_relaxed);
        return static_cast<double>(executed - failed) / executed;
    }

    /**
   * @brief 获取线程利用率
   * @return 利用率(0.0-1.0)
   */
    double getUtilization() const
    {
        auto now = std::chrono::steady_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count();

        if (total_time == 0)
            return 0.0;

        auto execution_time = totalExecutionTime.load(std::memory_order_relaxed);
        return static_cast<double>(execution_time) / total_time;
    }
};
class load_metrics
{
public:
    std::atomic<double> throughput{0.0};               // 吞吐量(任务/秒)
    std::atomic<double> memoryUsage{0.0};             // 内存使用率
    std::atomic<double> cpuUtilization{0.0};          // CPU利用率
    std::atomic<double> averageTaskTime{0.0};        // 平均任务执行时间

    std::atomic<std::size_t> queueLength{0};          // 队列长度
    std::atomic<std::size_t> activeThreads{0};        // 活跃线程数

    std::chrono::steady_clock::time_point lastUpdate; // 最后更新时间

    /**
   * @brief 重置指标
   */
    void reset()
    {
        throughput.store(0.0, std::memory_order_relaxed);
        memoryUsage.store(0.0, std::memory_order_relaxed);
        cpuUtilization.store(0.0, std::memory_order_relaxed);
        averageTaskTime.store(0.0, std::memory_order_relaxed);

        queueLength.store(0, std::memory_order_relaxed);
        activeThreads.store(0, std::memory_order_relaxed);

        lastUpdate = std::chrono::steady_clock::now();
    }

    /**
   * @brief 计算综合负载分数
   * @return 负载分数(0.0-1.0)
   */
    double calculateLoadScore() const
    {
        auto cpu = cpuUtilization.load(std::memory_order_relaxed);
        auto memory = memoryUsage.load(std::memory_order_relaxed);
        auto queue_factor = std::min(queueLength.load(std::memory_order_relaxed) / 100.0, 1.0);

        // 加权计算综合负载分数
        return 0.4 * cpu + 0.3 * memory + 0.3 * queue_factor;
    }
};
class scaling_config
{
public:
    std::size_t minThreads = 1;                      // 最小线程数
    std::size_t maxThreads = 32;                     // 最大线程数
    std::size_t coreThreads = 4;                     // 核心线程数
    double scaleUpThreshold = 0.8;                  // 扩容阈值
    double scaleDownThreshold = 0.4;                // 缩容阈值

    std::size_t scaleUpStep = 1;                    // 扩容步长
    std::size_t scaleDownStep = 1;                  // 缩容步长
    bool enablePredictiveScaling = true;            // 启用预测性扩缩容

    std::chrono::milliseconds scaleUpDelay{1000};   // 扩容延迟
    std::chrono::milliseconds scaleDownDelay{5000}; // 缩容延迟
};
class pool_config
{
public:
    // 基础配置
    std::string _pool_name = "default_pool"; // 线程池名称标识
    std::size_t _minThreads = 1; // 最小线程数量
    std::size_t _maxThreads = 8; // 最大线程数量
    std::size_t _coreThreads = 4; // 核心线程数量
    std::size_t _initial_threads = 4; // 初始线程数量

    // 队列配置
    std::size_t _max_queue_size = 0; // 最大队列容量
    rank_strategy _queue_policy = rank_strategy::fifo; // 队列调度策略

    // 调度配置
    expansion_strategy _expansion_strategy = expansion_strategy::hybrid; // 扩缩容策略
    scheduling_tactics _scheduling_tactics = scheduling_tactics::adaptive; // 调度策略
    
    // 超时配置
    std::chrono::milliseconds _task_timeout{300}; // 任务执行超时时间
    std::chrono::milliseconds _idle_timeout{600}; // 线程空闲超时时间
    std::chrono::milliseconds _shutdown_timeout{1000}; // 线程池关闭超时时间

    // 监控配置
    bool _enable_monitoring = true; // 是否启用性能监控
    bool _enable_performance_profiling = false; // 是否启用性能分析
    std::chrono::milliseconds _monitoring_interval{1000}; // 性能监控采样间隔

    // 日志配置
    std::string _log_file_path; // 日志文件存储路径
    bool _enable_event_logging = false; // 是否启用事件日志记录

    /**
   * @brief 验证配置有效性
   * @return true 配置有效，false 配置无效
   */
    bool validate() const
    {
        return _minThreads > 0 && _maxThreads >= _minThreads && _initial_threads >= _minThreads &&
        _initial_threads <= _maxThreads && _coreThreads >= _minThreads && _coreThreads <= _maxThreads;
    }
};
struct pool_statistics
{
    // 基础统计
    std::atomic<std::uint64_t> _total_tasksFailed{0}; // 累计失败任务数量
    std::atomic<std::uint64_t> _total_tasks_timeout{0}; // 累计超时任务数量
    std::atomic<std::uint64_t> _total_tasks_cancelled{0}; // 累计取消任务数量
    std::atomic<std::uint64_t> _total_tasks_submitted{0}; // 累计提交任务数量
    std::atomic<std::uint64_t> _total_tasks_completed{0}; // 累计完成任务数量

    // 性能统计
    std::atomic<double> _current_throughput{0.0}; // 当前任务吞吐量
    std::atomic<double> _peak_throughput{0.0}; // 历史峰值吞吐量
    
    // 吞吐量计算辅助变量
    std::atomic<std::int64_t> _last_throughput_time{0}; // 上次吞吐量计算时间戳
    std::atomic<std::uint64_t> _last_completed_count{0}; // 上次完成任务计数

    // 线程统计
    std::atomic<std::size_t> _active_thread_count{0}; // 当前活跃线程数量
    std::atomic<std::size_t> _current_thread_count{0}; // 当前总线程数量
    std::atomic<std::size_t> _peak_thread_count{0}; // 历史峰值线程数量

    // 队列统计
    std::atomic<std::size_t> _current_queue_size{0}; // 当前队列任务数量
    std::atomic<std::size_t> _peak_queue_size{0}; // 历史峰值队列大小

    // 时间统计
    std::chrono::steady_clock::time_point _startTime; // 线程池启动时间
    std::chrono::steady_clock::time_point _lastTaskTime; // 最后任务执行时间

    /**
   * @brief 重置统计信息
   */
    void reset()
    {
        _total_tasksFailed.store(0, std::memory_order_relaxed);
        _total_tasks_timeout.store(0, std::memory_order_relaxed);
        _total_tasks_cancelled.store(0, std::memory_order_relaxed);
        _total_tasks_submitted.store(0, std::memory_order_relaxed);
        _total_tasks_completed.store(0, std::memory_order_relaxed);

        _peak_throughput.store(0.0, std::memory_order_relaxed);
        _current_throughput.store(0.0, std::memory_order_relaxed);
        
        _last_throughput_time.store(0, std::memory_order_relaxed);
        _last_completed_count.store(0, std::memory_order_relaxed);

        _peak_thread_count.store(0, std::memory_order_relaxed);
        _active_thread_count.store(0, std::memory_order_relaxed);
        _current_thread_count.store(0, std::memory_order_relaxed);

        _peak_queue_size.store(0, std::memory_order_relaxed);
        _current_queue_size.store(0, std::memory_order_relaxed);

        _lastTaskTime = _startTime;
        _startTime = std::chrono::steady_clock::now();
    }

    /**
   * @brief 计算成功率
   * @return 成功率(0.0-1.0)
   */
    double calculateSuccessRate() const
    {
        auto total = _total_tasks_submitted.load(std::memory_order_relaxed);
        if (total == 0)
            return 1.0;

        auto completed = _total_tasks_completed.load(std::memory_order_relaxed);
        return static_cast<double>(completed) / total;
    }

    /**
   * @brief 计算运行时间
   * @return 运行时间(秒)
   */
    double calculateUptime() const
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _startTime);
        return duration.count();
    }
};