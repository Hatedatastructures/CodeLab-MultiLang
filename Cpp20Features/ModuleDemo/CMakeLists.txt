cmake_minimum_required(VERSION 3.28)
project(ModuleDemo LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(${PROJECT_NAME} ModuleDemo.cpp)
target_sources(${PROJECT_NAME} PRIVATE
    FILE_SET CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES MyModule.cppm
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fmodules-ts)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Include)
