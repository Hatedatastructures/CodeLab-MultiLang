cmake_minimum_required(VERSION 3.28)
project(QtWidgetApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Qt must be installed locally, cannot use FetchContent
find_package(Qt6 COMPONENTS Widgets)
if(Qt6_FOUND)
    qt_standard_project_setup()
    add_executable(${PROJECT_NAME} QtWidgetApp.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
else()
    message(STATUS "Qt6 not found, building without Qt")
    add_executable(${PROJECT_NAME} QtWidgetApp.cpp)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Include)
