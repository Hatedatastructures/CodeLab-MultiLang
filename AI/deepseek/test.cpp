#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp> // Boost 内置 JSON 库
#include <boost/json/parse.hpp>
#include <boost/json/serializer.hpp>

// 简化命名空间（C++ 开发者熟悉的用法，减少冗余）
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = boost::asio::ssl;
namespace json = boost::json;
using tcp = asio::ip::tcp;

// 核心函数：发送 HTTPS POST 请求到 AI API（纯 Boost 实现）
void send_ai_request(
    asio::io_context &io_ctx,       // Boost.Asio 核心上下文
    const std::string &host,        // API 域名（如 api.deepseek.com）
    const std::string &path,        // API 路径（如 /chat/completions）
    const std::string &api_key,     // 你的 API Key
    const json::value &request_body // Boost.JSON 格式的请求体
)
{
    // 1. 初始化 SSL 上下文（HTTPS 加密必需，基于 OpenSSL）
    ssl::context ssl_ctx(ssl::context::tlsv12_client);
    ssl_ctx.set_default_verify_paths();        // 加载系统 SSL 证书（避免证书验证失败）
    ssl_ctx.set_verify_mode(ssl::verify_none); // 开发阶段禁用证书验证（生产可开启）

    // 2. 解析域名和端口（Boost.Asio  resolver）
    tcp::resolver resolver(io_ctx);
    auto endpoints = resolver.resolve(host, "443"); // 443 = HTTPS 默认端口

    // 3. 创建 SSL 流（TCP 流 + SSL 加密封装）
    beast::ssl_stream<beast::tcp_stream> ssl_stream(io_ctx, ssl_ctx);

    // 4. 建立 TCP 连接 + SSL 握手（HTTPS 连接核心步骤）
    beast::get_lowest_layer(ssl_stream).connect(endpoints);
    ssl_stream.handshake(ssl::stream_base::client);

    // 5. 构造 HTTP POST 请求（Boost.Beast 原生请求对象）
    http::request<http::string_body> req(http::verb::post, path, 11); // HTTP/1.1
    req.set(http::field::host, host);                                 // 主机头（必需）
    req.set(http::field::content_type, "application/json");           // 数据格式：JSON
    req.set(http::field::authorization, "Bearer " + api_key);         // 认证信息
    req.body() = json::serialize(request_body);                       // Boost.JSON 转字符串
    req.prepare_payload();                                            // 自动计算 Content-Length（必需）

    // 6. 发送 HTTP 请求（加密传输）
    http::write(ssl_stream, req);

    // 7. 接收 API 响应（缓冲区存储数据）
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(ssl_stream, buffer, res);

    // 8. 处理响应结果
    if (res.result() == http::status::ok)
    { // HTTP 200 = 请求成功
        try
        {
            // 解析响应体（Boost.JSON 解析字符串）
            json::value response_json = json::parse(res.body());

            // 提取 AI 回复（DeepSeek API 格式：choices[0].message.content）
            // Boost.JSON 提取字段：用 at() 访问（不存在会抛异常，需捕获）
            const auto &choices = response_json.at("choices").as_array();
            const auto &message = choices.at(0).as_object().at("message").as_object();
            std::string ai_reply = message.at("content").as_string().c_str();

            // 输出结果
            std::cout << "AI 回复：\n"
                      << ai_reply << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "JSON 解析失败：" << e.what() << std::endl;
            std::cerr << "原始响应体：" << res.body() << std::endl;
        }
    }
    else
    { // API 业务错误（如 401 密钥错误、429 频率限制）
        std::cerr << "API 请求失败！状态码：" << res.result() << std::endl;
        std::cerr << "错误信息：" << res.body() << std::endl;
    }

    // 9. 关闭连接（优雅退出，避免资源泄漏）
    beast::error_code ec;
    ssl_stream.shutdown(ec);
    if (ec && ec != beast::errc::not_connected)
    {
        std::cerr << "SSL 关闭失败：" << ec.message() << std::endl;
        throw beast::system_error{ec};
    }
}

int main()
{
    try
    {
        // 1. 配置 AI API 核心参数（替换为你的信息）
        const std::string API_HOST = "api.deepseek.com";                        // 域名（无需加 https://）
        const std::string API_PATH = "/chat/completions";                       // 接口路径
        const std::string YOUR_API_KEY = ""; // 替换为你的 API Key

        // 2. 构造 JSON 请求体（纯 Boost.JSON 实现，无第三方库）
        json::object request_body; // Boost.JSON 对象（对应 JSON 的 {}）

        // 设置模型名（model: "deepseek-chat"）
        request_body["model"] = json::value("deepseek-reasoner");

        // 设置对话内容（messages: 数组，对应 JSON 的 []）
        json::array messages;
        // 系统角色（system: 助手身份）
        json::object system_msg;
        system_msg["role"] = json::value("system");
        system_msg["content"] = json::value("你是一个精通 C++ 的助手，只输出简洁准确的答案");
        messages.push_back(std::move(system_msg)); // 加入数组
        // 用户问题（user: 你的查询）
        json::object user_msg;
        user_msg["role"] = json::value("user");
        user_msg["content"] = json::value("请帮我写一个 C++ 程序，实现一个简单的 HTTP 请求功能");
        messages.push_back(std::move(user_msg));        // 加入数组
        request_body["messages"] = std::move(messages); // 绑定到请求体

        // 设置非流式输出（stream: false）
        request_body["stream"] = json::value(false);
        std::cout << "请求体：" << json::serialize(request_body) << std::endl;

        // 3. 初始化 Boost.Asio I/O 上下文（核心驱动）
        asio::io_context io_ctx;

        // 4. 发送请求（同步调用，简单易调试；如需异步可修改）
        send_ai_request(io_ctx, API_HOST, API_PATH, YOUR_API_KEY, json::value(request_body));
    }
    catch (const std::exception &e)
    { // 全局异常捕获（排错用）
        std::cerr << "程序异常：" << e.what() << std::endl;
        return 1;
    }

    return 0;
}