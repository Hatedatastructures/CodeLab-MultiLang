cmake_minimum_required(VERSION 3.28)
project(SystemPathTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} SystemPathTest.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE)
endif()
