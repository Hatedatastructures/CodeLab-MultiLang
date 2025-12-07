#pragma once
#include <string>
#include <format>
#include <chrono>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/stream_file.hpp>

namespace asio = boost::asio;

namespace performance_log
{
    template<typename T>
    concept compatible = requires(const T& x) { asio::buffer(x); };

    enum class level
    {
        debug,
        info,
        warn,
        error,
        fatal,
    };

    inline std::string timestamp()
    {
       return std::format("[{:%Y-%m-%d %H:%M}]", std::chrono::system_clock::now() + std::chrono::hours(8));
    }

    class coroutine_log
    {
    public:
        explicit coroutine_log(const asio::any_io_executor& executor)
        : event_executor(executor),
          serial_exec(asio::make_strand(executor))
        {
            file_map.reserve(12);
        }

        asio::awaitable<bool> open_file(const std::string& path) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (const auto it = file_map.find(path); it != file_map.end() && it->second && it->second->is_open())
            {
                co_return true;
            }
            asio::stream_file fp (serial_exec, path,
                asio::file_base::write_only | asio::file_base::create | asio::file_base::append);
            if (!fp.is_open())
            {
                co_return false;
            }
            file_map.emplace(path, std::make_shared<asio::stream_file>(std::move(fp)));
            co_return true;
        }

        asio::awaitable<void> close_file(const std::string& path) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            if (const auto it = file_map.find(path); it != file_map.end())
            {
                it->second->close();
                file_map.erase(it);
            }
            co_return;
        }
        
        template <compatible container>
        asio::awaitable<std::size_t> file_write(const std::string& path, const container& data) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            auto it = file_map.find(path);
            if (it == file_map.end() || !it->second || !it->second->is_open())
            {
                asio::stream_file fp (serial_exec, path,
                    asio::file_base::write_only | asio::file_base::create | asio::file_base::append);
                if (!fp.is_open())
                {
                    co_return 0;
                }
                it = file_map.emplace(path, std::make_shared<asio::stream_file>(std::move(fp))).first;
            }
            boost::system::error_code ec;
            std::size_t n = co_await asio::async_write(*it->second,asio::buffer(data),
                asio::redirect_error(asio::use_awaitable, ec));
            if (ec)
            {
                co_return 0;
            }
            co_return n;
        }

        asio::awaitable<std::size_t> file_write(const std::string& path, const std::vector<std::string>& data) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            auto it = file_map.find(path);
            if (it == file_map.end() || !it->second || !it->second->is_open())
            {
                asio::stream_file fp (serial_exec, path,
                    asio::file_base::write_only | asio::file_base::create | asio::file_base::append);
                if (!fp.is_open())
                {
                    co_return 0;
                }
                it = file_map.emplace(path, std::make_shared<asio::stream_file>(std::move(fp))).first;
            }
            std::size_t total = 0;
            for (auto const& s : data) { total += s.size(); }
            std::string joined;
            joined.reserve(total);
            for (auto const& s : data) { joined.append(s); }
            boost::system::error_code ec;
            std::size_t n = co_await asio::async_write(*it->second,asio::buffer(joined.data(), joined.size()),
                asio::redirect_error(asio::use_awaitable, ec));
            if (ec)
            {
                co_return 0;
            }
            co_return n;
        }
        std::string to_string(const level& log_level) const
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
        asio::awaitable<std::size_t> console_write(const level& log_level, const std::string& data) const
        {
            co_await asio::dispatch(serial_exec, asio::use_awaitable);
            std::string log_str = timestamp() + std::format("[{}] ", to_string(log_level)) + data;
            std::cout.write(log_str.data(), static_cast<std::streamsize>(log_str.size()));
            std::cout.flush();
            std::size_t n = log_str.size();
            co_return n;
        }

        template<typename... Args>
        asio::awaitable<std::size_t> file_log_message(const std::string& path, const std::string& format, Args&&... args) const
        {
            std::string data = timestamp() + std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            co_return co_await file_write(path, data);
        }
        asio::awaitable<std::size_t> file_log_message(const std::string& path, const std::vector<std::string>& data) const
        {
            co_return co_await file_write(path, data);
        }
        asio::awaitable<std::size_t> file_log_message(const std::string& path, const std::string& data) const
        {
            co_return co_await file_write(path, timestamp() + data);
        }
        template<typename... Args>
        asio::awaitable<std::size_t> console_log_message(level log_level, const std::string& format, Args&&... args) const
        {
            std::string data = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            co_return co_await console_write(log_level, data);
        }
        asio::awaitable<std::size_t> console_log_message(level log_level, const std::string& data) const
        {
            co_return co_await console_write(log_level, data);
        }

    private:
        asio::any_io_executor event_executor;
        asio::strand<asio::any_io_executor> serial_exec;
        mutable std::unordered_map<std::string, std::shared_ptr<asio::stream_file>> file_map;
    };


    


}
