/**
 * @file Conversation.hpp
 * @brief 会话管理类定义
 * @details 提供会话类的管理功能，包括会话的创建、销毁、消息的发送、接收等操作
 */
#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <future>
#include <optional>
#include <memory>
#include <functional>

#include "../../sched/ThreadPool.hpp"
#include "./Fundamental.hpp"

namespace conversation
{
    /**
     * @brief 复用`fundamental::stringable_constraints`约束
     */
    template <class protocol_t>
    concept serializable_constraints = fundamental::stringable_constraints<protocol_t>;

    using request = fundamental::request;
    using response = fundamental::response;

    class session_management_config
    {
    public:
        std::uint64_t thread_size{10};	   // 线程池大小
        std::uint64_t thread_max_size{64}; // 线程池最大线程数
    }; // end class session_management_config
    /**
     * @brief 会话管理类
     * @details 提供指定协议类型的会话管理功能
     * @warning 会话管理类在使用前必须先调用 `start()` 方法启动会话管理线程
     */
    template <serializable_constraints request_t = request, serializable_constraints response_t = response>
    class session_management
    {
    public:
        using session_ptr = std::shared_ptr<fundamental::session<request_t, response_t>>;
        using thread_pool = wan::pool::thread_pool;

    private:
        std::atomic<bool> _running{false};		   // 会话管理是否正在运行
        mutable std::shared_mutex _sessions_mutex; // 会话映射的互斥锁

        boost::asio::io_context &_io_context;						// io上下文
        boost::asio::steady_timer _cleanup_timer;					// 会话清理定时器
        std::chrono::seconds _cleanup_interval{60};					// 会话清理时间间隔
        std::unordered_map<std::string, session_ptr> _sessions_map; // 会话映射

        session_management_config _config; // 默认配置

        std::unique_ptr<thread_pool> _thread_pool;	   // 线程池
        std::atomic<bool> _thread_pool_running{false}; // 线程池是否正在运行
    private:
        /**
         * @brief 初始化线程池
         * @return true 线程池初始化成功
         * @return false 线程池初始化失败
         */
        bool _initialize_thread_pool()
        {
            if (!_thread_pool)
            {
                pool_config thread_pool_config;
                thread_pool_config._pool_name = "session_management";
                thread_pool_config._initial_threads = _config.thread_size * 2;
                thread_pool_config._min_threads = 2;
                thread_pool_config._max_threads = _config.thread_max_size * 4;
                thread_pool_config._core_threads = _config.thread_size * 2;
                thread_pool_config._queue_policy = rank_strategy::priority;
                thread_pool_config._scheduling_tactics = scheduling_tactics::adaptive;
                thread_pool_config._enable_monitoring = true;
                thread_pool_config._enable_performance_profiling = false;
                _thread_pool = wan::pool::make_thread_pool(thread_pool_config);
                if (_thread_pool && _thread_pool->start())
                {
                    _thread_pool_running.store(true);
                    return true;
                }
            }
            return false;
        }
        /**
         * @brief 停止线程池
         * @return true 线程池停止成功
         * @return false 线程池停止失败
         */
        bool _stop_thread_pool()
        {
            if (_thread_pool && _thread_pool_running.load())
            {
                if (_thread_pool->shutdown(std::chrono::seconds(5)))
                {
                    _thread_pool_running.store(false);
                    return true;
                }
            }
            return false;
        }
        /**
         * @brief 启动线程池
         * @return `true` 线程池启动成功
         * @return `false` 线程池启动失败
         */
        bool _start_thread_pool()
        {
            if (_thread_pool && !_thread_pool_running.load())
            {
                if (_thread_pool->start())
                {
                    _thread_pool_running.store(true);
                    return true;
                }
            }
            return false;
        }
        /**
         * @brief 启动清理定时器
         */
        void _start_cleanup_timer()
        {
            if (!_running.load())
                return;
            _cleanup_timer.expires_after(_cleanup_interval);
            auto cleanup_function = [this](const boost::system::error_code &ec)
            {
                if (!ec && _running.load())
                {
                    auto _cleanup_task = [this]()
                    {
                        _cleanup_inactive_sessions();
                    };
                    if (_thread_pool_running.load() && _thread_pool)
                        _thread_pool->submit_priority(weight::low, _cleanup_task);
                    else
                        _cleanup_task();
                    _start_cleanup_timer();
                }
            };
            _cleanup_timer.async_wait(cleanup_function);
        }
        /**
         * @brief 清理过期会话
         */
        void _cleanup_inactive_sessions()
        {
            // 检查管理器是否仍在运行，避免在停止后触发清理
            if (!_running.load())
                return;

            std::vector<std::string> inactive_sessions;
            {
                std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
                for (const auto &pair : _sessions_map)
                {
                    const auto &session = pair.second;
                    if (!session->is_connected() || session->get_statistics().get_idle_time() > std::chrono::minutes(10))
                    {
                        inactive_sessions.push_back(pair.first);
                    }
                }
            }

            if (!_running.load())
                return;

            for (const auto &session_string_id : inactive_sessions)
            {
                if (!_running.load())
                    break;
                remove_session_if_disconnected(session_string_id);
            }
        }

    private:
        std::vector<session_ptr> _screening_session(const std::vector<std::string> &ids, bool only_connected) const
        {
            std::vector<session_ptr> targets;
            {
                std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
                targets.reserve(ids.size());
                for (const auto &id : ids)
                {
                    auto it = _sessions_map.find(id);
                    if (it != _sessions_map.end())
                    {
                        const auto &sp = it->second;
                        if (!only_connected || sp->is_connected())
                            targets.push_back(sp);
                    }
                }
            }
            return targets;
        }
        std::vector<session_ptr> _all_session(bool only_connected) const
        {
            std::vector<session_ptr> snapshot;
            {
                std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
                snapshot.reserve(_sessions_map.size());
                for (const auto &kv : _sessions_map)
                {
                    if (!only_connected || kv.second->is_connected())
                        snapshot.push_back(kv.second);
                }
            }
            return snapshot;
        }
        // 基于谓词收集会话快照（谓词返回true则包含）
        template <class prediction>
        std::vector<session_ptr> _conditional_filtering(prediction &&pred, bool only_connected) const
        {
            std::vector<session_ptr> snapshot;
            {
                std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
                snapshot.reserve(_sessions_map.size());
                for (const auto &kv : _sessions_map)
                {
                    const auto &id = kv.first;
                    const auto &sp = kv.second;
                    if (pred(id, sp) && (!only_connected || sp->is_connected()))
                        snapshot.push_back(sp);
                }
            }
            return snapshot;
        }

    public:
        session_management(boost::asio::io_context &io_context,
                           const session_management_config &config = session_management_config())
            : _io_context(io_context), _cleanup_timer(_io_context), _config(config)
        {
            _initialize_thread_pool();
        }
        ~session_management()
        {
            stop();
            _stop_thread_pool();
        }
        session_management(const session_management &) = delete;
        session_management &operator=(const session_management &) = delete;
        /**
         * @brief 启动会话管理
         * @return `true` 会话管理启动成功
         * @return `false` 会话管理启动失败
         */
        bool start()
        {
            if (!_running.load())
            {
                _running.store(true);
                if (!_thread_pool_running.load())
                {
                    if (!_thread_pool)
                    {
                        if (!_initialize_thread_pool())
                        {
                            _running.store(false);
                            return false;
                        }
                    }
                    else if (!_start_thread_pool())
                    {
                        _running.store(false);
                        return false;
                    }
                }
                _start_cleanup_timer();
                return true;
            }
            return true;
        }
        /**
         * @brief 停止会话管理
         * @return `true` 会话管理停止成功
         */
        bool stop()
        {
            _running.store(false);
            _cleanup_timer.cancel();

            // 同步清理所有会话，避免异步清理的竞态条件
            {
                std::lock_guard<std::shared_mutex> lock(_sessions_mutex);
                for (auto &pair : _sessions_map)
                    pair.second->close();
                _sessions_map.clear();
            }
            return true;
        }

        /**
         * @brief 强制同步清理所有会话
         * @details 立即清理所有会话，不依赖定时器
         */
        void force_cleanup_all_sessions()
        {
            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);
            for (auto &pair : _sessions_map)
                pair.second->close();
            _sessions_map.clear();
        }
        auto create_session(boost::asio::ip::tcp::socket &&socket)
            -> session_ptr
        {
            if (socket.is_open())
            {
                session_ptr sess = std::make_shared<fundamental::session<request_t, response_t>>(std::move(socket));
                {
                    std::lock_guard<std::shared_mutex> lock(_sessions_mutex);
                    std::string session_string_id = sess->get_session_id();
                    _sessions_map[session_string_id] = sess;
                }
                return sess;
            }
            return nullptr;
        }
        /**
         * @brief 创建客户端会话
         * @param endpoint 会话端点
         * @return `std::pair<string,std::shared_ptr<session<request,response>>>` 会话指针
         */
        auto create_client_session(const boost::asio::ip::tcp::endpoint &endpoint)
            -> std::pair<std::string, session_ptr>
        {
            std::string session_string_id;
            boost::asio::ip::tcp::socket socket(_io_context);
            socket.connect(endpoint);
            auto sess = create_session(std::move(socket));
            if (sess)
                return std::make_pair(sess->get_session_id(), sess);
            return std::make_pair(session_string_id, nullptr);
        }
        /**
         * @brief 创建服务器会话
         * @param socket 会话套接字
         * @return `std::pair<string,std::shared_ptr<session<request,response>>>` 会话指针
         */
        auto create_server_session(boost::asio::ip::tcp::socket &&socket)
            -> std::pair<std::string, session_ptr>
        {
            auto sess = create_session(std::move(socket));
            if (sess)
                return std::make_pair(sess->get_session_id(), sess);
            return std::make_pair(std::string{}, nullptr);
        }
        /**
         * @brief 获取会话
         * @param session_string_id 会话ID
         * @return `std::shared_ptr<session<request,response>>` 会话指针
         */
        session_ptr get_session(const std::string &session_string_id)
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            auto it = _sessions_map.find(session_string_id);
            if (it != _sessions_map.end())
                return it->second;
            return nullptr;
        }
        /**
         * @brief 移除会话
         * @param session_string_id 会话`ID`
         * @return `true` 会话移除成功
         */
        bool remove_session(const std::string &session_string_id)
        {
            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);
            auto it = _sessions_map.find(session_string_id);
            if (it != _sessions_map.end())
            {
                it->second->close();
                _sessions_map.erase(it);
                return true;
            }
            return false;
        }

        /**
         * @brief 安全移除断开的会话
         * @param session_string_id 会话`ID`
         * @return `true` 会话已断开并移除成功，`false` 会话不存在或仍连接
         * @details 只移除确实断开连接的会话，避免误删已重新连接的会话
         */
        bool remove_session_if_disconnected(const std::string &session_string_id)
        {
            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);
            auto it = _sessions_map.find(session_string_id);
            if (it != _sessions_map.end())
            {
                if (!it->second->is_connected())
                {
                    it->second->close();
                    _sessions_map.erase(it);
                    return true;
                }
                return false;
            }
            return false;
        }
        /**
         * @brief 获取会话数量
         * @return  会话数量
         */
        std::uint64_t get_session_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            return _sessions_map.size();
        }
        /**
         * @brief 获取所有会话`ID`列表
         * @return `std::vector<std::string>` 会话`ID`列表
         */
        std::vector<std::string> get_session_ids() const
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            std::vector<std::string> session_ids;
            session_ids.reserve(_sessions_map.size());
            for (const auto &pair : _sessions_map)
                session_ids.push_back(pair.first);
            return session_ids;
        }
        /**
         * @brief 添加已存在的会话到管理器
         * @param session 会话指针
         * @return `true` 添加成功，`false` 会话为空或ID已存在
         */
        bool add_session(session_ptr session)
        {
            if (!session)
                return false;

            std::string session_id = session->get_session_id();
            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);

            // 检查ID是否已存在
            if (_sessions_map.find(session_id) != _sessions_map.end())
                return false;

            _sessions_map[session_id] = session;
            return true;
        }

        /**
         * @brief 添加已存在的会话到管理器（指定ID）
         * @param session_id 指定的会话ID
         * @param session 会话指针
         * @return `true` 添加成功，`false` 会话为空或ID已存在
         */
        bool add_session_with_id(const std::string &session_id, session_ptr session)
        {
            if (!session || session_id.empty())
                return false;

            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);

            // 检查ID是否已存在
            if (_sessions_map.find(session_id) != _sessions_map.end())
                return false;

            _sessions_map[session_id] = session;
            return true;
        }

        /**
         * @brief 批量添加会话到管理器
         * @param sessions 会话指针列表
         * @return 成功添加的会话数量
         */
        std::uint64_t add_sessions(const std::vector<session_ptr> &sessions)
        {
            std::uint64_t added_count = 0;
            std::lock_guard<std::shared_mutex> lock(_sessions_mutex);

            for (const auto &session : sessions)
            {
                if (session)
                {
                    std::string session_id = session->get_session_id();
                    if (_sessions_map.find(session_id) == _sessions_map.end())
                    {
                        _sessions_map[session_id] = session;
                        ++added_count;
                    }
                }
            }
            return added_count;
        }

        /**
         * @brief 检查会话ID是否存在
         * @param session_id 会话ID
         * @return `true` 存在，`false` 不存在
         */
        bool has_session(const std::string &session_id) const
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            return _sessions_map.find(session_id) != _sessions_map.end();
        }

        /**
         * @brief 获取已连接会话数量
         * @return 已连接会话数量
         */
        std::uint64_t get_connected_session_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            std::uint64_t count = 0;
            for (const auto &pair : _sessions_map)
            {
                if (pair.second->is_connected())
                    ++count;
            }
            return count;
        }

        /**
         * @brief 获取已断开会话数量
         * @return 已断开会话数量
         */
        std::uint64_t get_disconnected_session_count() const
        {
            std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
            std::uint64_t count = 0;
            for (const auto &pair : _sessions_map)
            {
                if (!pair.second->is_connected())
                    ++count;
            }
            return count;
        }

        /**
         * @brief 批量移除断开的会话
         * @return 移除的会话数量
         */
        std::uint64_t remove_disconnected_sessions()
        {
            std::vector<std::string> disconnected_ids;
            {
                std::shared_lock<std::shared_mutex> lock(_sessions_mutex);
                for (const auto &pair : _sessions_map)
                {
                    if (!pair.second->is_connected())
                        disconnected_ids.push_back(pair.first);
                }
            }

            std::uint64_t removed_count = 0;
            for (const auto &id : disconnected_ids)
            {
                // 使用安全移除方法，在持锁状态下最终检查连接状态
                if (remove_session_if_disconnected(id))
                    ++removed_count;
            }
            return removed_count;
        }
        /**
         * @brief 根据谓词筛选会话快照（公开接口）
         * @param pred 谓词函数，返回 `true` 则包含该会话，签名形如 `bool(const std::string&, const session_ptr&)`
         * @param only_connected 是否仅选择已连接会话，默认 `true`
         * @return 满足条件的会话指针快照
         */
        template <class prediction>
        std::vector<session_ptr> select_sessions_if(prediction &&pred, bool only_connected = true) const
        {
            return _conditional_filtering(std::forward<prediction>(pred), only_connected);
        }
        /**
         * @brief 使用会话指针执行一次性联动操作（单回调），在会话的io上下文中运行
         * @param sess 会话指针
         * @param linkage_operation 回调函数，签名形如 `void(session_ptr)`；内部可联动多步操作
         * @param priority 线程池提交优先级（仅影响调度入口）；默认 `weight::normal`
         * @return `true` 提交成功，`false` 会话为空
         */
        template <class operation>
        bool with_session(session_ptr sess, operation &&linkage_operation, weight priority = weight::normal)
        {
            if (!sess)
                return false;

            auto shell = [this, sp = std::move(sess), execute_function = std::forward<operation>(linkage_operation)]() mutable
            {
                auto linkage_function = [sp, execute_function]() mutable
                {
                    try
                    {
                        execute_function(sp);
                    }
                    catch (...)
                    {
                    }
                };
                boost::asio::dispatch(_io_context, linkage_function);
            };

            if (_thread_pool_running.load() && _thread_pool)
                _thread_pool->submit_priority(priority, std::move(shell));
            else
                std::async(std::launch::async, std::move(shell));
            return true;
        }
        /**
         * @brief 使用会话ID执行一次性联动操作（单回调），在会话的io上下文中运行
         * @param session_string_id 会话ID
         * @param linkage_operation 回调函数，签名形如 `void(session_ptr)`；内部可联动多步操作
         * @param priority 线程池提交优先级（仅影响调度入口）；默认 `weight::normal`
         * @return `true` 提交成功，`false` 会话不存在
         */
        template <class operation>
        bool with_session_id(const std::string &session_string_id, operation &&linkage_operation, weight priority = weight::normal)
        {
            auto sess = get_session(session_string_id);
            if (!sess)
                return false;
            return with_session({sess}, std::forward<operation>(linkage_operation), priority);
        }
        /**
         * @brief 针对指定会话ID集合执行联动操作
         * @param ids 会话ID列表
         * @param linkage_operation 回调函数 `void(session_ptr)`
         * @param priority 入口调度优先级
         * @param only_connected 仅对已连接会话执行
         * @return `true` 有提交；`false` 无匹配会话
         */
        template <class operation>
        bool with_sessions(const std::vector<std::string> &ids, operation &&linkage_operation, weight priority = weight::normal, bool only_connected = true)
        {
            auto targets = _screening_session(ids, only_connected);
            if (targets.empty())
                return false;

            auto shell = [this, vec = std::move(targets), execute_function = std::forward<operation>(linkage_operation)]() mutable
            {
                for (auto &sp : vec)
                {
                    auto linkage_function = [sp, execute_function]() mutable
                    {
                        try
                        {
                            execute_function(sp);
                        }
                        catch (...)
                        {
                        }
                    };
                    boost::asio::dispatch(_io_context, linkage_function);
                }
            };
            if (_thread_pool_running.load() && _thread_pool)
                _thread_pool->submit_priority(priority, std::move(shell));
            else
                std::async(std::launch::async, std::move(shell));
            return true;
        }
        /**
         * @brief 遍历所有会话执行联动操作
         * @param linkage_operation 回调 `void(session_ptr)`
         * @param priority 入口调度优先级
         * @param only_connected 仅对已连接会话执行
         */
        template <class operation>
        void for_each_session(operation &&linkage_operation, weight priority = weight::normal, bool only_connected = true)
        {
            auto snapshot = _all_session(only_connected);
            auto dispatch_function = [this, vec = std::move(snapshot), capture = std::forward<operation>(linkage_operation)]() mutable
            {
                for (auto &sp : vec)
                {
                    auto linkage_function = [sp, execute_function = capture]() mutable
                    {
                        try
                        {
                            execute_function(sp);
                        }
                        catch (...)
                        {
                        }
                    };
                    boost::asio::dispatch(_io_context, linkage_function);
                }
            };
            if (_thread_pool_running.load() && _thread_pool)
                _thread_pool->submit_priority(priority, std::move(dispatch_function));
            else
                std::async(std::launch::async, std::move(dispatch_function));
        }
        /**
         * @brief 向全部/指定连接状态的会话广播原始字节
         * @param data 原始数据
         * @param priority 入口调度优先级
         * @param only_connected 仅对已连接会话执行
         * @return `true` 有提交；`false` 无匹配会话
         */
        bool broadcast_bytes(std::string_view data, weight priority = weight::normal, bool only_connected = true)
        {
            auto snapshot = _all_session(only_connected);
            if (snapshot.empty())
                return false;
            auto payload = std::make_shared<std::string>(data);
            auto dispatch_function = [this, vec = std::move(snapshot), payload]() mutable
            {
                for (auto &sp : vec)
                {
                    auto send_fn = [sp, payload]() mutable
                    {
                        try
                        {
                            sp->send_bytes(*payload);
                        }
                        catch (...)
                        {
                        }
                    };
                    boost::asio::dispatch(_io_context, std::move(send_fn));
                }
            };
            if (_thread_pool_running.load() && _thread_pool)
                _thread_pool->submit_priority(priority, std::move(dispatch_function));
            else
                std::async(std::launch::async, std::move(dispatch_function));
            return true;
        }
        /**
         * @brief 向全部已管理的会话广播请求
         * @note 仅在会话管理器处于运行状态时有效
         * @param request 请求数据
         * @param priority 入口调度优先级
         * @param only_connected 是否仅对已连接会话执行
         */
        bool broadcast_request(const request_t &request, weight priority = weight::normal, bool only_connected = true)
        {
            return broadcast_bytes(request.to_string(), priority, only_connected);
        }
        /**
         * @brief 向全部已管理的会话广播响应
         * @note 仅在会话管理器处于运行状态时有效
         * @param response 响应数据
         * @param priority 入口调度优先级
         * @param only_connected 是否仅对已连接会话执行
         */
        bool broadcast_response(const response_t &response, weight priority = weight::normal, bool only_connected = true)
        {
            return broadcast_bytes(response.to_string(), priority, only_connected);
        }
        /**
         * @brief 获取内部线程池统计信息
         * @return 线程池统计信息
         */
        std::optional<pool_statistics> get_thread_pool_statistics() const
        {
            if (_thread_pool_running.load() && _thread_pool)
                return std::optional<pool_statistics>(_thread_pool->get_statistics());
            return std::nullopt;
        }
    }; // end class

    struct endpoint_config
    {
        std::string host;
        std::uint16_t port{0};
        std::uint64_t min_connections{1};
        std::uint64_t max_connections{8};

        std::chrono::milliseconds borrow_timeout{2000};
        std::chrono::milliseconds connect_timeout{1500};
        std::chrono::seconds health_check_interval{10};

        fundamental::session_config session_cfg{};
    }; // end struct endpoint_config

    /**
     * @brief 会话连接池
     * @tparam request_t 请求数据类型
     * @tparam response_t 响应数据类型
     * @details 提供会话连接的管理和维护功能
     * @note 主要面对客户端会话管理
     */
    template <serializable_constraints request_t = request, serializable_constraints response_t = response>
    class connection_pool
    {
    public:
        using session_ptr = std::shared_ptr<fundamental::session<request_t, response_t>>;
        using session_internal = fundamental::session<request_t, response_t>;

        struct pool_stats
        {
            std::uint64_t remaining_available{0};
            std::uint64_t in_use{0};
            std::uint64_t total{0};
        };

    private:
        /**
         * @brief 端点键
         * @details 用于唯一标识会话连接池中的端点
         */
        struct endpoint_key
        {
            std::string host;
            std::uint16_t port{0};
            bool operator==(const endpoint_key &other) const noexcept
            {
                return port == other.port && host == other.host;
            }
        }; // end struct endpoint_key

        /**
         * @brief 端点键哈希函数
         * @details 用于将端点键映射到哈希值
         */
        struct endpoint_hash
        {
            std::size_t operator()(const endpoint_key &k) const noexcept
            {
                std::hash<std::string> hs;
                std::hash<std::uint16_t> hp;
                return hs(k.host) ^ (static_cast<std::size_t>(hp(k.port)) << 1);
            }
        }; // end struct endpoint_hash

        /**
         * @brief 端点连接池
         * @details 存储和管理特定端点的会话连接
         */
        struct endpoint_pool
        {

            using session_weak_ptr = std::weak_ptr<fundamental::session<request_t, response_t>>;

            std::mutex mtx; // 互斥锁
            std::condition_variable cv;

            endpoint_config cfg;													  // 端点配置
            std::deque<session_ptr> remaining_available;							  // 剩余可用会话
            std::unordered_set<std::string> weak_reference_collection;				  // 借出的会话 ID 集合
            std::unordered_map<std::string, session_weak_ptr> weak_reference_mapping; // 借出会话弱引用

            std::atomic<bool> healthy{true}; // 端点健康状态
        }; // end struct endpoint_pool

        std::atomic<bool> _running{false}; // 会话管理器运行状态

        boost::asio::io_context &_io_context;	// IO上下文
        boost::asio::steady_timer _check_timer; // 健康检查定时器

        mutable std::shared_mutex _map_mutex; // 会话连接池互斥锁

        std::unordered_map<endpoint_key, std::shared_ptr<endpoint_pool>, endpoint_hash> _endpoint_pools; // 端点连接池
    private:
        /**
         * @brief 获取指定端点连接池
         * @param host 主机名
         * @param port 端口号
         * @return 端点连接池指针
         */
        std::shared_ptr<endpoint_pool> _get_pool(const std::string &host, std::uint16_t port) const
        {
            endpoint_key key{host, port};
            std::shared_lock<std::shared_mutex> rlock(_map_mutex);
            auto it = _endpoint_pools.find(key);
            if (it == _endpoint_pools.end())
                return nullptr;
            return it->second;
        }
        /**
         * @brief 计算指定端点连接池已连接会话总数
         * @param pool 端点连接池指针
         * @warning 调用前需持有 pool->mtx 锁
         * @return 已连接会话总数
         */
        std::uint64_t _connected_total_locked(const std::shared_ptr<endpoint_pool> &pool) const
        {
            std::uint64_t cnt = 0;
            for (auto &sp : pool->remaining_available)
            {
                if (sp && sp->is_connected())
                    ++cnt;
            }
            for (auto &kv : pool->weak_reference_mapping)
            {
                auto sp = kv.second.lock();
                if (sp && sp->is_connected())
                    ++cnt;
            }
            return cnt;
        }
        /**
         * @brief 计划下一次健康检查
         * @details 基于端点配置的健康检查间隔，设置定时器触发下一次检查
         */
        void _schedule_next_check()
        {
            if (!_running.load())
                return;
            std::chrono::seconds interval{3};
            {
                std::shared_lock<std::shared_mutex> rlock(_map_mutex);
                for (auto &kv : _endpoint_pools)
                {
                    auto pool = kv.second;
                    if (pool && pool->cfg.health_check_interval > interval)
                        interval = pool->cfg.health_check_interval;
                }
            }
            _check_timer.expires_after(interval);
            auto cb = [this](const boost::system::error_code &ec)
            {
                if (!ec && _running.load())
                {
                    _health_check_tick();
                    _schedule_next_check();
                }
            };
            _check_timer.async_wait(cb);
        }
        /**
         * @brief 健康检查 tick
         * @details 遍历所有端点连接池，检查会话健康状态，更新端点健康状态
         */
        void _health_check_tick()
        {
            std::shared_lock<std::shared_mutex> rlock(_map_mutex);
            for (auto &kv : _endpoint_pools)
            {
                auto pool = kv.second;
                std::unique_lock<std::mutex> lock(pool->mtx);

                auto it = pool->remaining_available.begin();
                while (it != pool->remaining_available.end())
                { // 清理断开的空闲会话
                    if (!(*it)->is_connected())
                        it = pool->remaining_available.erase(it);
                    else
                        ++it;
                }
                // 清理已借出但已失效/断开的弱引用，避免统计和内存泄漏
                auto itw = pool->weak_reference_mapping.begin();
                while (itw != pool->weak_reference_mapping.end())
                {
                    auto spw = itw->second.lock();
                    if (!spw || !spw->is_connected())
                    {
                        pool->weak_reference_collection.erase(itw->first);
                        itw = pool->weak_reference_mapping.erase(itw);
                    }
                    else
                    {
                        ++itw;
                    }
                }
                lock.unlock();
                _preheat_pool(pool);
            }
        }
        /**
         * @brief 预加热会话池
         * @details 检查会话池是否需要预加热，根据配置创建新会话
         * @param pool 端点连接池指针
         */
        void _preheat_pool(std::shared_ptr<endpoint_pool> &pool)
        {
            if (!pool)
                return;
            std::uint64_t need = 0;
            {
                std::unique_lock<std::mutex> lock(pool->mtx);
                std::uint64_t connected = _connected_total_locked(pool);
                if (connected < pool->cfg.min_connections)
                    need = pool->cfg.min_connections - connected;
            }
            for (std::uint64_t i = 0; i < need; ++i)
            { // 预加热会话池，创建新会话
                auto sp = _create_session_sync(pool->cfg);
                std::unique_lock<std::mutex> lock(pool->mtx);
                std::uint64_t connected = _connected_total_locked(pool);
                if (sp && sp->is_connected() && connected < pool->cfg.max_connections)
                {
                    pool->remaining_available.emplace_back(std::move(sp));
                    pool->cv.notify_one();
                }
            }
        }
        /**
         * @brief 同步建连
         * @details 根据端点配置创建新会话，阻塞当前线程直到建连完成
         * @param cfg 端点配置
         * @return 会话指针
         */
        session_ptr _create_session_sync(const endpoint_config &cfg)
        {
            auto proto = cfg.session_cfg._enable_ssl ? fundamental::session_type::SSL_CLIENT : fundamental::session_type::TCP_CLIENT;
            auto sp = std::make_shared<session_internal>(_io_context, proto, cfg.session_cfg);
            if (sp->connect(cfg.host, cfg.port))
                return nullptr;
            // auto erro_func = [](const boost::system::error_code &ec)
            // {
            //   if(!ec)
            //     return;
            //   std::cout << "async_connect failed: " << ec.message() << std::endl;
            // };
            // sp->async_connect(cfg.host, cfg.port, erro_func);
            return sp;
        }

    public:
        connection_pool(boost::asio::io_context &io_context)
            : _io_context(io_context), _check_timer(io_context) {}

        ~connection_pool()
        {
            stop();
        }
        /**
         * @brief 启动会话池
         * @details 初始化会话池，创建健康检查定时器
         */
        bool start()
        {
            if (_running.exchange(true))
                return true;
            _schedule_next_check();
            std::shared_lock<std::shared_mutex> rlock(_map_mutex);
            for (auto &kv : _endpoint_pools)
            {
                auto pool = kv.second;
                _preheat_pool(pool);
            }
            return true;
        }
        /**
         * @brief 停止会话池
         * @details 关闭所有会话，取消健康检查定时器
         */
        bool stop()
        {
            if (!_running.exchange(false))
                return true;
            _check_timer.cancel();
            std::unique_lock<std::shared_mutex> wlock(_map_mutex);
            for (auto &kv : _endpoint_pools)
            {
                auto pool = kv.second;
                std::unique_lock<std::mutex> lock(pool->mtx);
                while (!pool->remaining_available.empty())
                {
                    auto sp = pool->remaining_available.front();
                    pool->remaining_available.pop_front();
                    sp->close();
                }
                pool->cv.notify_all();
            }
            return true;
        }
        /**
         * @brief 添加端点
         * @details 根据端点配置创建新的ip连接池
         * @param cfg 端点配置
         * @return 是否添加成功
         */
        bool add_endpoint(const endpoint_config &cfg)
        {
            if (cfg.host.empty() || cfg.port == 0 || cfg.min_connections > cfg.max_connections)
                return false;
            endpoint_key key{cfg.host, cfg.port};
            std::unique_lock<std::shared_mutex> wlock(_map_mutex);
            if (_endpoint_pools.find(key) != _endpoint_pools.end())
                return true; // 存在为成功
            auto pool = std::make_shared<endpoint_pool>();
            pool->cfg = cfg;
            _endpoint_pools.emplace(key, pool);
            wlock.unlock();
            _preheat_pool(pool);
            return true;
        }
        /**
         * @brief 删除端点
         * @details 根据主机名和端口号删除指定的ip连接池
         * @param host 主机名
         * @param port 端口号
         * @return 是否删除成功
         */
        bool remove_endpoint(const std::string &host, std::uint16_t port)
        {
            endpoint_key key{host, port};
            std::unique_lock<std::shared_mutex> wlock(_map_mutex);
            auto it = _endpoint_pools.find(key);
            if (it == _endpoint_pools.end())
                return false; // 不存在为失败
            auto pool = it->second;
            std::unique_lock<std::mutex> lock(pool->mtx);
            while (!pool->remaining_available.empty())
            {
                auto sp = pool->remaining_available.front();
                pool->remaining_available.pop_front();
                if (sp)
                    sp->close();
            }
            pool->cv.notify_all();
            _endpoint_pools.erase(it);
            return true;
        }
        /**
         * @brief 从端点连接池借用会话
         * @details 根据主机名和端口号从指定的ip连接池借用会话
         * @param host 主机名
         * @param port 端口号
         * @return 会话指针
         */
        std::optional<session_ptr> borrow(const std::string &host, std::uint16_t port,
                                          std::chrono::milliseconds timeout = std::chrono::milliseconds::zero())
        {
            auto pool = _get_pool(host, port);
            if (!pool)
                return std::nullopt;
            auto deadline = std::chrono::steady_clock::now() + (timeout.count() > 0 ? timeout : pool->cfg.borrow_timeout);
            std::unique_lock<std::mutex> lock(pool->mtx);
            while (true)
            {
                if (!pool->remaining_available.empty())
                {
                    auto sp = pool->remaining_available.front();
                    pool->remaining_available.pop_front();
                    if (sp && sp->is_connected())
                    {
                        pool->weak_reference_collection.insert(sp->get_session_id());
                        pool->weak_reference_mapping[sp->get_session_id()] = sp;
                        return sp;
                    } // 断开连接，丢弃并尝试重试
                }
                if (_connected_total_locked(pool) < pool->cfg.max_connections)
                {
                    lock.unlock();
                    auto sp_new = _create_session_sync(pool->cfg);
                    lock.lock();
                    if (sp_new && sp_new->is_connected())
                    {
                        pool->weak_reference_collection.insert(sp_new->get_session_id());
                        pool->weak_reference_mapping[sp_new->get_session_id()] = sp_new;
                        return sp_new;
                    }
                }
                if (std::chrono::steady_clock::now() >= deadline)
                    break;
                pool->cv.wait_until(lock, deadline);
            }
            return std::nullopt;
        }
        /**
         * @brief 尝试从端点连接池借用会话
         * @details 根据主机名和端口号从指定的ip连接池尝试借用会话
         * @param host 主机名
         * @param port 端口号
         * @return 会话指针
         */
        std::optional<session_ptr> try_borrow(const std::string &host, std::uint16_t port)
        {
            auto pool = _get_pool(host, port);
            if (!pool)
                return std::nullopt;
            std::unique_lock<std::mutex> lock(pool->mtx);
            if (!pool->remaining_available.empty())
            {
                auto sp = pool->remaining_available.front();
                pool->remaining_available.pop_front();
                if (sp && sp->is_connected())
                {
                    pool->weak_reference_collection.insert(sp->get_session_id());
                    pool->weak_reference_mapping[sp->get_session_id()] = sp;
                    return sp;
                } // 断开连接，直接返回
            }
            return std::nullopt;
        }
        /**
         * @brief 归还会话到端点连接池
         * @details 将之前从端点连接池借用的会话归还到连接池
         * @param sp 会话指针
         */
        void give_back(session_ptr sp)
        {
            if (!sp)
                return;
            auto key = endpoint_key{sp->get_remote_address(), sp->get_remote_port()};
            auto pool = _get_pool(key.host, key.port);
            if (!pool)
            {
                sp->close(); // 不是该连接池开辟的，直接关闭，防止资源泄露
                return;
            }
            std::unique_lock<std::mutex> lock(pool->mtx);
            pool->weak_reference_collection.erase(sp->get_session_id());
            pool->weak_reference_mapping.erase(sp->get_session_id());
            if (sp->is_connected())
                pool->remaining_available.emplace_back(std::move(sp)); // 正式归还链接
            lock.unlock();
            pool->cv.notify_one();
            _preheat_pool(pool); // 触发补足
        }
        /**
         * @brief 无效会话
         * @details 将之前从端点连接池借用的会话无效，主动关闭连接
         * @param sp 会话指针
         */
        void invalidate(session_ptr sp)
        {
            if (!sp)
                return;
            auto pool = _get_pool(sp->get_remote_address(), sp->get_remote_port());
            if (!pool)
            {
                sp->close();
                return;
            }
            std::unique_lock<std::mutex> lock(pool->mtx);
            pool->weak_reference_collection.erase(sp->get_session_id());
            pool->weak_reference_mapping.erase(sp->get_session_id());
            if (sp->is_connected())
                sp->close(); // 主动关闭连接
            lock.unlock();
            pool->cv.notify_one();
            _preheat_pool(pool); // 触发补足
        }
        /**
         * @brief 获取端点连接池状态
         * @details 获取指定主机名和端口号的端点连接池的状态
         * @param host 主机名
         * @param port 端口号
         * @return 连接池状态
         */
        pool_stats get_pool_stats(const std::string &host, std::uint16_t port) const
        {
            pool_stats s{};
            auto pool = _get_pool(host, port);
            if (!pool)
                return s;
            std::unique_lock<std::mutex> lock(pool->mtx);
            s.remaining_available = pool->remaining_available.size();
            s.in_use = pool->weak_reference_collection.size();
            s.total = _connected_total_locked(pool);
            lock.unlock();
            return s;
        }
    }; // end class connection_pool
} // end namespace conversation