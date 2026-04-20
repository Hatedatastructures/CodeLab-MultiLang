cmake_minimum_required(VERSION 3.28)
project(ModelTest LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME} ModelTest.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Asio/Model
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE)
endif()
