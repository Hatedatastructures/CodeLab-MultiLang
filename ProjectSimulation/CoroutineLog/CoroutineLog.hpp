#pragma once
#include <string>
#include <format>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/stream_file.hpp>

namespace asio = boost::asio;

namespace CoroutineLog
{
    template<typename T>
    concept compatible = requires(const T& x) { asio::buffer(x); };

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
            std::vector<asio::const_buffer> const_buffers;
            const_buffers.reserve(data.size());
            for (auto const& s : data)
            {
                const_buffers.emplace_back(asio::buffer(s));
            }
            boost::system::error_code ec;
            std::size_t n = co_await asio::async_write(*it->second,const_buffers,
            asio::redirect_error(asio::use_awaitable, ec));
            if (ec)
            {
                co_return 0;
            }
            co_return n;
        }
    private:
        asio::any_io_executor event_executor;
        asio::strand<asio::any_io_executor> serial_exec;
        mutable std::unordered_map<std::string, std::shared_ptr<asio::stream_file>> file_map;
    };
    // template<typename ...Args>
    // asio::awaitable<std::uint32_t> FileLogMassage(const std::string& format_str, Args... args)
    // {
    //     std::string data = std::vformat(format_str, std::make_format_args(args...));
    //     co_return co_await asio::async_write(std::cout,asio::buffer(data),asio::use_awaitable);
    // }
    // template<typename ...Args>
    // asio::awaitable<std::uint32_t> LogMassage(const std::string& format_str, Level level, Args... args)
    // {
    //     std::string data = std::vformat(format_str, std::make_format_args(args...));
    //     co_return co_await asio::async_write(std::cout,asio::buffer(data),asio::use_awaitable);
    // }
}
