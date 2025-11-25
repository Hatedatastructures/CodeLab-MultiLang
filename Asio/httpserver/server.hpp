#pragma once
#include "../model/network/network.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <format>
#include <chrono>
#include <functional>
#include <filesystem>
#include <optional>
#include <unordered_map>
#include <boost/asio.hpp>
#include <atomic>


using namespace wan::network;

using request_processing_fn = std::function<http::response<>(const http::request<>&)>;

static const std::string INDEX_HTML_PATH = "index.html";

struct format_time
{
  static std::string format_print()
  {   // 加8小时为北京时间
    return std::format("[{:%Y-%m-%d %H:%M:%S}]", std::chrono::system_clock::now() + std::chrono::hours(8));
  }

  static std::string format_print(const std::string &msg)
  {
    return std::format("[{:%Y-%m-%d %H:%M:%S}] {}", std::chrono::system_clock::now() + std::chrono::hours(8), msg);
  }

  template <typename... strings>
  static std::string format_print(const std::string &msg, strings... args)
  {
    return std::format(format_time::format_print(msg), args...);
  }
};

/**
 * @brief 格式化打印日志
 * @param msg 日志消息
 * @param args 格式化参数
 * @return std::string 格式化后的带时间日志消息
 */
template <typename... strings>
std::string format_print(const std::string &msg, strings... args)
{
  return std::vformat(format_time::format_print(msg), std::make_format_args(args...));
}

struct asset
{
  std::string file_data;
  std::string read_file(const std::string &path)
  {
    std::ifstream file(path, std::ios::binary);
    if (!file)
      return {};
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return data;
  }
  asset() = default;
  asset(const std::string &html_path) : file_data(read_file(html_path)) {}
};
struct status_response
{
  asset html_404;
  asset html_500;
};

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

/**
 * @brief 简单的http静态网页服务器
 */

class server
{
  std::string web_root;
  boost::asio::io_context &io_context;                                               // io上下文
  status_response status_htmlresponses;                                              // 状态响应
  boost::asio::ip::tcp::endpoint endpoint;                                           // tcp端点
  boost::asio::ip::tcp::acceptor acceptor;                                           // tcp监听器
  session::session_management<http::request<>, http::response<>> session_management; // 会话连接管理
  std::atomic<bool> server_running{false};
private:

  /**
   * @brief 获取文件MIME类型
   * @param path 文件路径
   */
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
   * @brief 构建绝对路径
   * @param file_path web内的文件路径
   * @return std::string 绝对路径
   */
  std::string make_absolutely_path(const std::string &file_path)
  {
    if (web_root.empty())
    {
      web_root = "/";
    }
    return (std::filesystem::path(web_root) / file_path).string();
  }


  /**
   * @brief 预加载并缓存`404.html`和`500.html`
   */
  void preload_html()
  {
    auto path_404 = (std::filesystem::path(web_root) / "404.html").string();
    status_htmlresponses.html_404 = asset(path_404);
    auto path_500 = (std::filesystem::path(web_root) / "500.html").string();
    status_htmlresponses.html_500 = asset(path_500);
  }

  /**
   * @brief 生成静态文件响应
   * @param file_path 文件路径
   * @param keep_alive 是否保持连接
   * @return http::response<> 响应
   */
  http::response<> make_static_response(const std::string &file_path, bool keep_alive)
  {
    http::response<> response;
    asset reader(file_path);
    auto body = std::move(reader.file_data);
    if (body.empty())
    {
      response.result(boost::beast::http::status::not_found);
      response.base().set(http::field::content_type, "text/html; charset=UTF-8");
      response.body() = status_htmlresponses.html_404.file_data;
    }
    else
    {
      response.result(boost::beast::http::status::ok);
      response.base().set(http::field::content_type, mime_type(file_path));
      response.body() = std::move(body);
    }
    response.keep_alive(keep_alive);
    response.base().content_length(response.body().size());
    response.prepare_payload();
    return response;
  }

  /**
   * @brief 默认请求处理
   * @param request 请求
   * @return http::response<> 响应
   */
  http::response<> default_handle_request(const http::request<> &request)
  {
    auto target_sv = request.target();
    std::string target{target_sv.data(), target_sv.size()};
    bool keep = request.keep_alive();

    if (target == "/api/health")
    {
      http::response<> res;
      res.result(boost::beast::http::status::ok);
      return res;
    }

    std::string rel;
    if (target == "/" || target == "/index.html")
      rel = INDEX_HTML_PATH;
    else if (!target.empty() && target[0] == '/')
      rel = target.substr(1);
    else
      rel = target;

    try
    {
      auto root = std::filesystem::weakly_canonical(std::filesystem::path(web_root));
      auto full = std::filesystem::weakly_canonical(root / rel);
      std::filesystem::path rel_path = std::filesystem::relative(full, root);
      for (auto &part : rel_path)
      {
        if (part == "..")
          throw std::runtime_error("path out of root");
      }
      if (std::filesystem::exists(full) && std::filesystem::is_regular_file(full))
        return make_static_response(full.string(),keep);
    }
    catch (...)
    {
      return make_404_response(false);
    }

    return make_404_response(false);
  }

  /**
   * @brief 快捷生成一个服务端404响应
   * @param keep_alive 是否保持连接
   * @return `http::response<>` 响应
   */
  http::response<> make_404_response(bool keep_alive) const
  {
    http::response<> response;
    response.result(boost::beast::http::status::not_found);
    response.base().set(http::field::content_type, "text/html; charset=UTF-8");
    response.body() = status_htmlresponses.html_404.file_data;
    response.keep_alive(keep_alive);
    response.base().content_length(response.body().size());
    response.prepare_payload();
    return response;
  }

  /**
   * @brief 快捷生成一个服务端500响应
   * @param keep_alive 是否保持连接
   * @return `http::response<>` 响应
   */
  http::response<> make_500_response(bool keep_alive) const
  {
    http::response<> response;
    response.result(boost::beast::http::status::internal_server_error);
    response.base().set(http::field::content_type, "text/html; charset=UTF-8");
    response.body() = status_htmlresponses.html_500.file_data;
    response.keep_alive(keep_alive);
    response.base().content_length(response.body().size());
    response.prepare_payload();
    return response;
  }

  static void log_send_result(const std::shared_ptr<session::session<http::request<>, http::response<>>>& sess_ptr,
    const boost::system::error_code& ec)
  {
    if (!ec)
      std::cout << format_print("send response success :{}", sess_ptr->get_session_id()) << std::endl;
    else
      std::cout << format_print("send response error :{},{}", sess_ptr->get_session_id(), ec.message()) << std::endl;
  }

  /**
   * @brief 接受新的tcp连接并处理请求响应数据
   */
  void socket_accept()
  {
    if (!server_running.load() || !acceptor.is_open())
      return;
    // 处理新连接
    auto handle_function = [&](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
    {
      if (!ec)
      {
        using session_ptr = std::shared_ptr<session::session<http::request<>, http::response<>>>;

        // 接受数据的处理
        auto func = [this](const session_ptr& ptr, std::string_view data)
        {

          // 处理响应发送回调
          auto call = [sess_ptr = ptr](boost::system::error_code ec)
          {
            server::log_send_result(sess_ptr, ec);
          };  // end Lambda call

          // 解析请求
          auto request = http::request<>{};
          if (!request.from_string(data))
          {
            http::response<> bad = make_404_response(false);
            std::cout << format_print(" parsing failed ip:{},port:{}",ptr->get_remote_address(),ptr->get_remote_port()) << std::endl;
            auto send_and_close = [sess_ptr = ptr](boost::system::error_code ec)
            {
              server::log_send_result(sess_ptr, ec);
              sess_ptr->close();
            };
            ptr->async_send_response(bad, send_and_close);
            return;
          }

          try
          {
            http::response<> res = default_handle_request(request);
            std::cout << format_print("request success,from ip:{},port:{}",ptr->get_remote_address(),ptr->get_remote_port()) << std::endl;
            ptr->async_send_response(res, call);
          }
          catch (const std::exception &e)
          {
            http::response<> err = make_500_response(false);
            std::cout << format_print(" server error :{},{}",ptr->get_session_id(),e.what()) << std::endl;
            auto send_and_close = [sess_ptr = ptr](boost::system::error_code ec)
            {
              server::log_send_result(sess_ptr, ec);
              sess_ptr->close();
            };
            ptr->async_send_response(err, send_and_close); 
          } // end try

        }; // end Lambda func

        std::cout << format_print("connection successful,from ip {},port:{}",
              socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port()) << std::endl;
        const auto value = session_management.create_server_session(std::move(socket));
        std::cout << format_print("{} create session success,id:{} ", value.second->get_remote_address(), 
              value.first) << std::endl;

        value.second->set_reception_processing(func);
        value.second->start();
      }
      else
      {
        std::cout << format_print("accept error:{}", ec.message()) << std::endl;
      }
      if (server_running.load() && acceptor.is_open())
        socket_accept();
    }; // end Lambda handle_function
    acceptor.async_accept(handle_function);
  }

public:
  server(boost::asio::io_context &io_context, std::uint16_t port)
      : web_root("."), io_context(io_context), endpoint(boost::asio::ip::tcp::v4(), port),
        acceptor(io_context), session_management(io_context)
  {
    std::cout << format_print("{} server initialization succeeded,port:{}", endpoint.address().to_string(), port) << std::endl;
    preload_html();
  }

  /**
   * @brief 设置web根目录
   */
  void set_web_root(const std::string &root)
  {
    web_root = root;
    preload_html();
  }


  void start()
  {
    server_running.store(true);
    acceptor.open(endpoint.protocol());
    acceptor.bind(endpoint);
    acceptor.listen(boost::asio::socket_base::max_listen_connections);
    session_management.start();
    socket_accept();
  }

  ~server()
  {
    stop();
  }

  void stop()
  {
    server_running.store(false);
    boost::system::error_code ec;
    acceptor.cancel(ec); // 取消当前正在进行的接受操作
    acceptor.close(ec);
    session_management.stop();
  }

}; // end class server
