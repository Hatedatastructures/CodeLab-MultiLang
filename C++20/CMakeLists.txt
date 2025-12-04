cmake_minimum_required(VERSION 3.26)  # 确保版本≥3.23（支持FILE_SET）
project(ModuleDemo)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. 定义可执行目标（先创建目标）
add_executable(ModuleDemo
        module.cpp
        generator.cpp
        # 主程序（不包含模块接口文件，后续通过FILE_SET添加）
)

# 2. 关键：为目标添加 CXX_MODULES 类型的 FILE_SET，包含模块接口文件
target_sources(ModuleDemo
        PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}  # 模块文件的基础目录
        FILES
        mymodule.cppm  # 模块接口文件（必须添加到这里）
)

# 3. 为目标添加 GCC 模块支持选项（如果用GCC）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ModuleDemo PRIVATE -fmodules-ts)
endif()