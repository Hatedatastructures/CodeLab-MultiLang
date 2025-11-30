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
static constexpr const std::string index_path = "/index.html";

/**
 * @brief `常量`：`Server` 头信息
 */
static const std::string SERVER_NAME = "test_asio_http_server";

/**
 * @brief `全局变量`：静态文件缓存
 * 键：文件系统路径（使用系统分隔符）
 * 值：文件内容
 */
static std::unordered_map<std::string, std::string> file_cache;

/**
 * @brief 加载静态文件到内存
 * @param root_path `根目录路径`
 */
void load_static_files(const std::string& root_path)
{
    if (!std::filesystem::exists(root_path))
    {
        std::cerr << "webroot not found: " << root_path << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path))
    {
        if (std::filesystem::is_regular_file(entry))
        {
            std::ifstream file(entry.path(), std::ios::binary);
            if (file)
            {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                // 使用系统原生路径格式作为键
                file_cache[entry.path().string()] = std::move(content);
                std::cout << "Load file: " << entry.path().string() << " (" << file_cache[entry.path().string()].size() << " bytes)" << std::endl;
            }
        }
    }
}

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
 * @brief 从内存缓存中读取内容
 * @param content_path `文件路径`
 * @return `文件内容指针`，如果不存在返回 `nullptr`
 */
const std::string* construct_response_content(const std::string &content_path)
{
    auto it = file_cache.find(content_path);
    if (it != file_cache.end())
    {
        return &it->second;
    }
    return nullptr;
}

awaitable<http::response<http::string_body>> make_error_response(const std::string file_path)
{
    http::response<http::string_body> response;
    response.version(11);
    response.set(http::field::server, SERVER_NAME);
    response.result(http::status::internal_server_error);
    response.set(http::field::content_type,mime_type(file_path));
    // 直接从内存读取
    const std::string* content = construct_response_content(std::filesystem::path(file_path).string());
    if (content)
    {
        response.body() = *content;
    }
    // 如果错误页面本身不存在，body 保持为空
    
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
    std::cout << "500 internal server error: " << web_root + "/500.html" << std::endl;
    co_return co_await make_error_response(web_root + "/500.html");
}



/**
 * @brief 构建 `404 Bad Request` 响应
 */
awaitable<http::response<http::string_body>> make_client_error_response(const std::string web_root)
{
    std::cout << "404 not found: " << web_root + "/404.html" << std::endl;
    co_return co_await make_error_response(web_root + "/404.html");
}



/**
 * @brief `构建安全的绝对文件路径`，防止目录穿越
 * @param server_root `站点根目录`
 * @param raw_target `HTTP请求目标`
 * @return `安全绝对路径`，失败返回 `std::nullopt`
 */
std::string make_safe_full_path(const std::filesystem::path &server_root, std::string raw_target)
{
    try
    {
        std::filesystem::path file_path;
        
        if (raw_target.empty() || (raw_target.size() == 1 && raw_target[0] == '/'))
        {
            file_path = server_root / index_path.substr(1); // 去掉开头的 / 以避免绝对路径覆盖
        }
        else
        {
            // 移除开头的 /，防止 path 将其视为根路径
            std::string rel_path = (raw_target[0] == '/') ? raw_target.substr(1) : raw_target;
            file_path = server_root / rel_path;
        }

        // 转换为系统原生路径格式，以匹配缓存键
        return file_path.make_preferred().string();
    }
    catch (...)
    {
        std::cerr << "make safe full path error: " << raw_target << std::endl;
        return std::string{};
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
    const std::string target = std::string(request.target());
    
    // std::cout << "request target: " << target << std::endl;

    std::string file_absolute_path;
    try
    {
        // 使用统一的安全路径构建函数
        file_absolute_path = make_safe_full_path(web_root_path, target);
        if (file_absolute_path.empty())
        {
            std::cout << "request file error: " << target << std::endl;
            co_return co_await make_client_error_response(web_root_path);
        }
        
        // std::cout << "request file: " << file_absolute_path << std::endl;

        // 直接从内存读取，无需 co_await
        const std::string* body = construct_response_content(file_absolute_path);
        if(body)
        {
            response.result(boost::beast::http::status::ok);
            response.set(http::field::content_type,mime_type(file_absolute_path));
            response.body() = *body;
            response.content_length(response.body().size());
        }
        else
        {
            co_return co_await make_client_error_response(web_root_path);
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
        /**
         * @brief 为客户端 `socket` 启用 `TCP_NODELAY`，降低 `Nagle` 算法导致的延迟
         */
        try
        {
            socket.set_option(asio::ip::tcp::no_delay(true));
        }
        catch (const std::exception &e)
        {
            // 忽略此处异常，不影响后续处理
        }

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
    catch (const boost::system::system_error &e)
    {
        // 忽略正常的连接关闭 (end_of_stream) 和 客户端断开 (connection_reset)
        if (e.code() != http::error::end_of_stream && 
            e.code() !=  boost::asio::error::connection_reset &&
            e.code() !=  boost::asio::error::broken_pipe)
        {
            std::cerr << "client " << socket.remote_endpoint() << " error: " << e.what() << std::endl;
        }
        socket.close();
        co_return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "client " << socket.remote_endpoint() << " error: " << e.what() << std::endl;
        socket.close();
        co_return;
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
        /**
         * @brief `监听器`初始化：开启地址复用，并设置最大 `backlog`
         */
        try
        {
            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.listen(asio::socket_base::max_listen_connections);
            std::cout << "acceptor listen max connections: " << asio::socket_base::max_listen_connections << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "acceptor init error: " << e.what() << std::endl;
        }

        co_spawn(io_context, accept_loop(acceptor_), detached);
    }

private:
    asio::ip::tcp::acceptor acceptor_; 
};

int main()
{
    try
    {
        // 加载静态文件到内存
        load_static_files(web_root_path);

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
        // std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
