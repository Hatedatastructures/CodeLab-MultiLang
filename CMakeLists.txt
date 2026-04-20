cmake_minimum_required(VERSION 3.28)
project(CodeLabMultiLang LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)

# Shared header include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Add all subdirectories
add_subdirectory(CBasics)
add_subdirectory(Year2024)
add_subdirectory(Year2025)
add_subdirectory(Networking)
add_subdirectory(LinuxProg)
add_subdirectory(Cpp20Features)
add_subdirectory(AiIntegration)
add_subdirectory(QtApp)
add_subdirectory(ProjectSimulationApp)
add_subdirectory(ThirdPartyLibTest)
