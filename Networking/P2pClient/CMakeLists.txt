cmake_minimum_required(VERSION 3.28)
project(P2pClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    boost
    URL https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz
    URL_HASH SHA256=2575e74ffc3ef1cd0babac2c1ee8bdb5782a0ee672b1912da40e5b4b591ca01f
)
FetchContent_MakeAvailable(boost)

add_executable(${PROJECT_NAME} P2pClient.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${boost_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32 mswsock bcrypt crypt32)
endif()
