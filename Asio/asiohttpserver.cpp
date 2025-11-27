#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <optional>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using boost::asio::awaitable;
using boost::asio::co_spawn;      // 启动协程
using boost::asio::detached;      // 分离到后台
using boost::asio::use_awaitable; // 令牌

/**
 * @brief `常量`：`web` 根目录
 */
static const std::string web_root_path = "webroot";

/**
 * @brief `常量`：默认 `index` 文件名
 */
static constexpr const char *DEFAULT_INDEX_FILE = "index.html";

/**
 * @brief `常量`：`Server` 头信息
 */
static const std::string SERVER_NAME = "test_asio_http_server";

static const std::unordered_map<std::string, std::string> extension_map{
    {"html", "text/html"},
    {"htm", "text/html"},
    {"css", "text/css"},
    {"js", "application/javascript"},
    {"json", "application/json"},
    {"xml", "application/xml"},
    {"txt", "text/plain"},
    {"pdf", "application/pdf"},
    {"png", "image/png"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},
    {"bmp", "image/bmp"},
    {"ico", "image/x-icon"},
    {"svg", "image/svg+xml"},
    {"webp", "image/webp"},
    {"mp4", "video/mp4"},
    {"webm", "video/webm"},
    {"ogg", "video/ogg"},
    {"mp3", "audio/mpeg"},
    {"wav", "audio/wav"},
    {"flac", "audio/flac"},
    {"aac", "audio/aac"},
};

std::string mime_type(std::string path)
{
auto dot = path.rfind('.');
if (dot == std::string::npos)
    return "text/plain";
auto ext = std::string(path.substr(dot + 1));
auto it = extension_map.find(ext);
if (it == extension_map.end())
    return "text/plain";
return it->second;
}

/**
 * @brief 从文件中读取内容
 * @param content_path `文件路径`
 * @return `文件内容`
 */
awaitable<std::string> construct_response_content(const std::string &content_path)
{
    std::ifstream file(content_path, std::ios::binary);
    if (!file)
        co_return std::string{}; 
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    co_await asio::post(asio::use_awaitable);  
    co_return data;  
}

awaitable<http::response<http::string_body>> make_error_response(const std::string file_path)
{
    http::response<http::string_body> response;
    response.version(11);
    response.set(http::field::server, SERVER_NAME);
    response.result(http::status::internal_server_error);
    response.set(http::field::content_type,mime_type(file_path));
    response.body() = co_await construct_response_content(file_path);
    response.content_length(response.body().size());
    response.keep_alive(false);
    response.prepare_payload();
    co_return response;
}
/**
 * @brief 构建 `500 Internal Server Error` 响应
 * @return `HTTP响应`（`awaitable`）
 */
awaitable<http::response<http::string_body>> make_server_error_response(const std::string web_root)
{
    co_return co_await make_error_response(web_root + "/500.html");
}


/**
 * @brief 构建 `400 Bad Request` 响应
 */
awaitable<http::response<http::string_body>> make_client_error_response(const std::string web_root)
{
    co_return co_await make_error_response(web_root + "/404.html");
}



/**
 * @brief `构建安全的绝对文件路径`，防止目录穿越
 * @param server_root `站点根目录`
 * @param raw_target `HTTP请求目标`
 * @return `安全绝对路径`，失败返回 `std::nullopt`
 */
std::optional<std::filesystem::path> make_safe_full_path(const std::filesystem::path &server_root, std::string raw_target)
{
    try
    {
        const auto qpos = raw_target.find('?');
        if (qpos != std::string::npos)
        {
            raw_target = raw_target.substr(0, qpos);
        }
        if (raw_target.empty())
        {
            raw_target = "/";
        }

        std::filesystem::path relative = std::filesystem::path(raw_target).lexically_normal();

        if (!relative.empty())
        {
            auto native = relative.native();
            if (!native.empty() && native.front() == '/')
            {
                relative = native.substr(1);
            }
        }

        if (!raw_target.empty() && raw_target.back() == '/')
        {
            relative /= DEFAULT_INDEX_FILE;
        }

        std::error_code ec;
        const std::filesystem::path canonical_root = std::filesystem::weakly_canonical(server_root, ec);
        if (ec)
        {
            return std::nullopt;
        }
        const std::filesystem::path tentative = server_root / relative;
        const std::filesystem::path full_path = std::filesystem::weakly_canonical(tentative, ec);
        if (ec)
        {
            return std::nullopt;
        }
        const std::string full_path_str = full_path.string();
        const std::string root_str = canonical_root.string();
        if (full_path_str.rfind(root_str, 0) != 0)
        {
            return std::nullopt;
        }

        if (!std::filesystem::exists(full_path) || !std::filesystem::is_regular_file(full_path))
        {
            return std::nullopt;
        }

        return full_path;
    }
    catch (...)
    {
        return std::nullopt;
    }
}



/**
 * @brief 构建静态文件的 `HTTP` 响应
 * @param request `HTTP请求`
 * @return `HTTP响应`（`awaitable`）
 */
awaitable<http::response<http::string_body>> build_response(const http::request<http::string_body> &request)
{
    http::response<http::string_body> response;
    response.set(http::field::server, SERVER_NAME);

    // 构建安全路径
    std::filesystem::path full_path;
    const std::string target = std::string(request.target());
    const std::filesystem::path server_root = web_root_path;
    
    

    std::string file_absolute_path;
    try
    {
        // 使用统一的安全路径构建函数
        auto full_path_opt = make_safe_full_path(server_root, target);
        if (!full_path_opt)
        {
            co_return co_await make_client_error_response(web_root_path);
        }
        full_path = *full_path_opt;
        file_absolute_path = full_path.string();
        
        std::cout << "request file: " << file_absolute_path << std::endl;

        std::string body = co_await construct_response_content(file_absolute_path);
        if(!body.empty())
        {
            response.result(boost::beast::http::status::ok);
            response.set(http::field::content_type,mime_type(file_absolute_path));
            response.body() = std::move(body);
            response.content_length(response.body().size());
        }
        else
        {
            co_return co_await make_server_error_response(web_root_path);
        }
        response.prepare_payload();
    }
    catch (...)
    {
        std::cerr << "build response error: " << file_absolute_path << std::endl;
    }
    co_return response;
}

/**
 * @brief 处理单个客户端连接的 `HTTP` 协程
 * @param socket `已连接的客户端socket`
 */
awaitable<void> handle_client(asio::ip::tcp::socket socket)
{
    try
    {
        beast::flat_buffer read_buffer;
        http::request<http::string_body> request;

        while (true)
        {
            // 异步读取 `HTTP` 请求
            co_await http::async_read(socket, read_buffer, request, use_awaitable);

            // 构建并写回响应
            auto response = co_await build_response(request);
            response.keep_alive(request.keep_alive());
            co_await http::async_write(socket, response, use_awaitable);
            if (response.need_eof())
            {
                break;
            }
            request = {};
        }

        beast::error_code ec;
        socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
        socket.close(ec);
    }
    catch (const std::exception &e)
    {
        std::cerr << "client " << socket.remote_endpoint() << " error: "<< e.what() << std::endl;
    }
}

/**
 * @brief 循环接受客户端连接的协程
 * @param acceptor `服务器监听器`
 */
awaitable<void> accept_loop(asio::ip::tcp::acceptor &acceptor)
{
    while (true)
    {
        // 异步接受新连接
        asio::ip::tcp::socket socket = co_await acceptor.async_accept(use_awaitable);

        // 打印新客户端连接信息
        std::cout << "new client connected: " << socket.remote_endpoint() << std::endl;

        // 启动客户端处理协程（`detached`：后台运行）
        co_spawn(acceptor.get_executor(),handle_client(std::move(socket)),detached);
    }
}

/**
 * @brief `TCPServer` 类
 */
class tcp_server
{
public:
    /**
     * @brief 构造函数：初始化监听器并启动接受循环
     * @param io_context `Asio` 的 `IO` 上下文
     * @param port `监听端口`
     */
    tcp_server(asio::io_context &io_context, unsigned short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        co_spawn(io_context, accept_loop(acceptor_), detached);
    }

private:
    asio::ip::tcp::acceptor acceptor_; 
};

int main()
{
    try
    {
        asio::io_context io_context;         
        tcp_server server(io_context, 8080); 

        std::cout << "http server running on port 8080, webroot: '" << web_root_path << "'" << std::endl;
        std::vector<std::jthread> threads;
        for (int i = 0; i < 16; ++i)
        {
            threads.emplace_back([&io_context](){ io_context.run(); });
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
