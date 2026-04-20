### 日常代码学习库

- 跨语言代码练习与示例集合，`C/C++` 为主，包含网络编程、并发调度、数据结构模拟、系统编程、小型工具与前端静态页面等。
- 主要模块：`Asio` 网络模型与静态 `HTTP` 服务器、`Linux` 平台练习与网络示例、`C`/`C++20` 语法与标准库示例、`Go`/`Python` 脚本、`Qt` 示例、`Html` 静态资源。
- 自定义库迁移至: [跳转](https://github.com/Hatedatastructures/Custom-libraries#)

### 目录结构概览

- `CBasics/` C 语言基础与小程序示例
- `Cpp20Features/` 协程、`format`、`ranges` 等标准库示例
- `Networking/` 网络编程示例（`Asio` 服务器、`P2P`、线程池客户端等）
- `LinuxProg/` Linux 平台编程练习与网络示例
- `Year2024/`、`Year2025/` 每日练习代码与数据结构实现
- `Include/` 公共头文件与封装库
  - `Asio/` 网络模型组件
    - `HttpServer/` 简单静态 `HTTP` 服务器，`webroot/` 为前端静态资源
    - `Model/` 网络模型核心
      - `Network/` 协议封装（`http/json/tcp`）、会话管理、代理与加密
      - `Sched/` 任务调度与线程池
      - `Concurrent/` 并发容器与工具
      - `Container/` 数据结构模拟
  - `Linux/` 平台相关头文件
    - `Internet/` `UDP`/`TCP` 网络示例
    - `TcpInternet/` `TCP` 通讯模块化示例
    - `Thread/` 线程同步工具
    - `SmallProjects/` 倒计时、进度条、远程控制等小项目
  - `Encapsulation/` 线程池、同步队列等封装类
- `AiIntegration/` AI 接口相关测试
- `QtApp/` `Qt` 示例
- `ProjectSimulationApp/` 项目模拟应用
- `ThirdPartyLibTest/` 第三方库相关测试
- `Go/` 基础库与测试脚本
- `Python/` 简单脚本示例
- `Html/` 静态页面与素材
