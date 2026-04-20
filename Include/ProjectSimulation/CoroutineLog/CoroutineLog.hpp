#pragma once
#include <string>
#include <format>
#include <chrono>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/stream_file.hpp>


namespace performance_log
{
    template<typename container_type>
    concept compatible = requires(const container_type& x) { boost::asio::buffer(x); };

    enum class level
    {
        debug,
        info,
        warn,
        error,
        fatal,
    };

    namespace asio = boost::asio;
    namespace fs = std::filesystem;
    class coroutine_log
    {
        struct context
        {
            std::shared_ptr<asio::stream_file> handle;
            std::size_t current_size = 0;
        };
    public:
        explicit coroutine_log(const asio::any_io_executor& executor)
        : event_executor(executor),
          serial_exec(asio::make_strand(executor))
        {
            file_map.reserve(4);
            // 默认日志目录
            root_directory = fs::current_path() / "logs";
            // 默认时间偏移为 +8 小时，保持与旧行为一致
            time_offset = std::chrono::hours(8ULL);
        }

        /**
         * @brief 设置日志输出目录
         * @param directory_name 日志输出目录的路径
         */
        asio::awaitable<void> setOutputDirectory(const std::string& directory_name)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            root_directory = directory_name;
            if (!fs::exists(root_directory))
            {
                boost::system::error_code ec;
                //创建目录
                fs::create_directories(root_directory, ec);
            }
        }

        /**
         * @brief 设置日志文件的最大大小，超过该大小则创建新的日志文件
         * @param size 日志文件的最大大小，单位为字节
         */
        asio::awaitable<void> setMaxFileSize(const std::size_t size)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            max_file_size = size;
        }

        /**
         * @brief 设置时间偏移，用于日志中的时间戳
         * @param offset 时间偏移量，默认 +8 小时
         */
        asio::awaitable<void> setTimeOffset(const std::chrono::minutes offset)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            time_offset = offset;
        }

        /**
         * @brief 设置输出到日志文件的级别阈值
         * @param threshold 日志级别阈值
         */
        asio::awaitable<void> setFileLevelThreshold(const level threshold)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            file_level_threshold = threshold;
        }

        /**
         * @brief 设置输出到控制台日志的级别阈值
         * @param threshold 日志级别阈值
         */
        asio::awaitable<void> setConsoleLevelThreshold(const level threshold)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            console_level_threshold = threshold;
        }

        /**
         * @brief 设置日志文件最大归档数量，超过该数量则删除最旧的日志文件
         * @param count 最大归档数量
         */
        asio::awaitable<void> setMaxArchiveCount(const std::size_t count)
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            max_archive_count = count;
        }

        asio::awaitable<void> closeFile(const std::string& path) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (const auto it = file_map.find(path); it != file_map.end())
            {
                if (it->second.handle) it->second.handle->close();
                file_map.erase(it);
            }
        }

        /**
         * @brief 关闭所有已打开的文件句柄
         */
        asio::awaitable<void> shutdown() const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            for (auto it = file_map.begin(); it != file_map.end(); )
            {
                if (it->second.handle) it->second.handle->close();
                it = file_map.erase(it);
            }
        }

        /**
         * @brief 将日志消息输出到文件
         * @param filename 日志文件名
         * @param data 日志消息
         * @return 写入的字节数
         * @note 如果文件不存在，会尝试创建目录并打开文件
         */
        template <compatible container>
        auto fileWrite(const std::string& filename, const container& data) const
        -> asio::awaitable<std::size_t>
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);

            // 1. 获取文件路径
            fs::path target_path = root_directory / filename;
            if (fs::path(filename).is_absolute())
            {   // 如果路径是绝对路径，则直接使用，避免路径拼接错误
                target_path = filename;
            }
            std::string key = target_path.string();
            
            // 2. 获取或创建文件上下文
            auto it = file_map.find(key);
            if (it == file_map.end() || !it->second.handle || !it->second.handle->is_open())
            {
                if (!fs::exists(target_path.parent_path()))
                {   // 如果目录不存在，尝试创建
                    boost::system::error_code ec;
                    fs::create_directories(target_path.parent_path(), ec);
                }

                asio::stream_file fp(serial_exec, key,
                    asio::file_base::write_only | asio::file_base::create | asio::file_base::append);
                
                if (!fp.is_open())
                {   // 尝试自愈：如果文件存在但无法打开，可能是损坏，尝试删除并重新创建
                    if (boost::system::error_code ec; fs::exists(target_path, ec))
                    {
                        fs::remove(target_path, ec);
                        asio::stream_file fp_retry(serial_exec, key,
                            asio::file_base::write_only | asio::file_base::create | asio::file_base::append);
                        if (!fp_retry.is_open()) co_return 0;
                        fp = std::move(fp_retry);
                    }
                    else
                    {
                        co_return 0;
                    }
                }
                
                context ctx; // 初始化上下文并获取当前文件大小
                ctx.handle = std::make_shared<asio::stream_file>(std::move(fp));
                boost::system::error_code ec;
                ctx.current_size = fs::file_size(target_path, ec); 
                it = file_map.insert_or_assign(key, std::move(ctx)).first;
            }

            // 3. 滚动检查
            std::size_t write_size = asio::buffer_size(asio::buffer(data));
            if (it->second.current_size + write_size > max_file_size)
            {
                // 关闭当前句柄
                it->second.handle->close();
                
                // 生成归档文件名（保持扩展名在末尾）
                auto now = std::chrono::system_clock::now();
                auto timestamp_str = std::format("{:%Y%m%d_%H%M%S}", now);
                fs::path parent = target_path.parent_path();
                std::string stem = target_path.stem().string();
                std::string ext = target_path.extension().string();
                fs::path archive_path = parent / (stem + "-" + timestamp_str + ext);
                
                boost::system::error_code ec;
                fs::rename(target_path, archive_path, ec);
                // 归档保留策略清理
                cleanupOldArchives(target_path);
                
                // 重新创建新文件
                asio::stream_file new_fp(serial_exec, key,
                     asio::file_base::write_only | asio::file_base::create | asio::file_base::truncate); // truncate 清空
                
                if (!new_fp.is_open())
                {   // 滚动失败，移除映射
                    file_map.erase(it);
                    co_return 0;
                }
                
                it->second.handle = std::make_shared<asio::stream_file>(std::move(new_fp));
                it->second.current_size = 0;
            }

            // 4. 执行写入
            boost::system::error_code ec;
            std::size_t n = co_await asio::async_write(*it->second.handle, asio::buffer(data),
                asio::redirect_error(asio::use_awaitable, ec));
            
            if (ec)
            {
                file_map.erase(it); // 写入失败，移除映射
                co_return 0;
            }
            
            it->second.current_size += n;
            co_return n;
        }

        /**
         * @brief 针对 vector<string> 的特化：先合并再调用通用 fileWrite (避免递归循环)
         * @param path 日志文件名
         * @param data 日志消息向量，每个元素为一条日志消息
         * @return 写入的总字节数
         * @note 会将所有消息合并为一个字符串后写入文件
         */
        auto fileWrite(const std::string& path, const std::vector<std::string>& data) const
        -> asio::awaitable<std::size_t>
        {
            std::size_t total = 0;
            for (auto const& s : data) { total += s.size(); }
            std::string joined;
            joined.reserve(total);
            for (auto const& s : data) { joined.append(s); }
            
            co_return co_await fileWrite(path, joined);
        }

        static std::string toString(const level& log_level)
        {
            switch (log_level)
            {
                case level::debug: return "DEBUG";
                case level::info: return "INFO";
                case level::warn: return "WARN";
                case level::error: return "ERROR";
                case level::fatal: return "FATAL";
                default: return "";
            }
        }

        /**
         * @brief 将日志消息输出到控制台
         * @param log_level 日志级别
         * @param data 日志消息
         * @return 写入的字节数
         * @note 如果日志级别低于 console_level_threshold，则不输出
         */
        auto consoleWrite(const level& log_level, const std::string& data) const
        -> asio::awaitable<std::size_t>
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (static_cast<int>(log_level) < static_cast<int>(console_level_threshold))
            {
                co_return 0;
            }
            const std::string log_str = timestampString() + std::format("[{}] ", toString(log_level)) + data;
            std::cout.write(log_str.data(), static_cast<std::streamsize>(log_str.size()));
            std::cout.flush();
            std::size_t n = log_str.size();
            co_return n;
        }

        /**
         * @brief 将日志消息输出到控制台，并在末尾添加换行符
         * @param log_level 日志级别
         * @param data 日志消息
         * @return 写入的字节数
         * @note 如果日志级别低于 console_level_threshold，则不输出
         */
        auto consoleWriteLine(const level& log_level, const std::string& data) const
        -> asio::awaitable<std::size_t>
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (static_cast<int>(log_level) < static_cast<int>(console_level_threshold))
            {
                co_return 0;
            }
            std::string log_str = timestampString() + std::format("[{}] ", toString(log_level)) + data + "\n";
            std::cout.write(log_str.data(), static_cast<std::streamsize>(log_str.size()));
            std::cout.flush();
            std::size_t n = log_str.size();
            co_return n;
        }

        /**
         * @brief 写入日志消息到文件
         * @param filename 日志文件名
         * @param log_level 日志级别
         * @param format 日志消息格式化字符串
         * @param args 日志消息
         * @return 写入的字节数
         * @note 如果日志级别低于 file_level_threshold，则不输出
         */
        template<typename... Args>
        auto fileWriteFmt(const std::string& filename, level log_level, const std::string& format, Args&&... args) const
        -> asio::awaitable<std::size_t>
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (static_cast<int>(log_level) < static_cast<int>(file_level_threshold))
            {
                co_return 0;
            }
            const std::string data = timestampString() + std::format("[{}] ", toString(log_level))
                + std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            co_return co_await fileWrite(filename, data);
        }

        /**
         * @brief 写入日志消息到文件，并在末尾添加换行符
         * @param filename 日志文件名
         * @param data 日志消息
         * @return 写入的字节数
         * @note 会在日志消息前添加时间戳
         */
        asio::awaitable<std::size_t> fileWriteLine(const std::string& filename, const std::string& data) const
        {
            std::string s = timestampString() + data + "\n";
            co_return co_await fileWrite(filename, s);
        }


        /**
         * @brief 将日志消息输出到控制台
         * @param log_level 日志级别
         * @param format 日志消息格式化字符串
         * @param args 日志消息
         * @return 写入的字节数
         * @note 如果日志级别低于 console_level_threshold，则不输出
         */
        template<typename... Args>
        auto consoleWriteFmt(const level log_level, const std::string& format, Args&&... args) const
        -> asio::awaitable<std::size_t>
        {
            std::string data = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            co_return co_await consoleWrite(log_level, data);
        }

    private:
        asio::any_io_executor event_executor;
        asio::strand<asio::any_io_executor> serial_exec;
        mutable std::unordered_map<std::string, context> file_map;
        
        // 配置项
        fs::path root_directory;
        std::chrono::minutes time_offset{0ULL};
        std::size_t max_archive_count = 0; // 0 表示不限制归档个数
        level file_level_threshold = level::debug;
        level console_level_threshold = level::debug;
        std::size_t max_file_size = 10 * 1024 * 1024; // 10MB
    private:
        
        /**
         * @brief 构建时间戳字符串（使用实例级时间偏移）
         * 
         * @return std::string 时间戳字符串，格式为 "[YYYY-MM-DD HH:MM]"
         */
        std::string timestampString() const
        {
            auto now = std::chrono::system_clock::now() + time_offset;
            auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            // 提取秒数（整数部分）和毫秒数
            auto secs = std::chrono::time_point_cast<std::chrono::seconds>(ms);
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(ms - secs).count();
            
            return std::format("[{:%Y-%m-%d %H:%M:%S}.{:03d}]", secs, millis);
        }

        /**
         * @brief 清理旧归档文件
         */
        void cleanupOldArchives(const fs::path& target_path) const
        {
            if (max_archive_count == 0) { return; }
            boost::system::error_code ec;
            const fs::path parent = target_path.parent_path();
            if (!fs::exists(parent, ec)) { return; }
            const std::string stem = target_path.stem().string();
            const std::string ext = target_path.extension().string();
            const std::string prefix = stem + "-";
            std::vector<fs::directory_entry> archives;
            for (fs::directory_iterator it(parent, ec), end; it != end && !ec; ++it)
            {
                const auto& entry = *it;
                if (!entry.is_regular_file(ec)) { continue; }
                if (std::string name = entry.path().filename().string(); name.size() >= prefix.size() + ext.size()
                    && name.starts_with(prefix) && name.ends_with(ext))
                {
                    archives.push_back(entry);
                }
            }
            if (archives.size() <= max_archive_count) { return; }
            auto archives_sorted = [](const fs::directory_entry& a, const fs::directory_entry& b)
            {
                return a.path().filename().string() < b.path().filename().string();
            };
            std::sort(archives.begin(), archives.end(), archives_sorted);
            const std::size_t remove_count = archives.size() - max_archive_count;
            for (std::size_t i = 0; i < remove_count; ++i)
            {
                fs::remove(archives[i], ec);
            }
        }
    };
}
