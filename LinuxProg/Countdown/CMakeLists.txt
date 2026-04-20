cmake_minimum_required(VERSION 3.28)
project(Countdown LANGUAGES C)
add_executable(${PROJECT_NAME} Countdown.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Include)
