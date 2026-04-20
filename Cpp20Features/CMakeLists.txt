# C++20 features
cmake_minimum_required(VERSION 3.28)

# C++ modules only supported by Ninja/VS generators
if(CMAKE_GENERATOR MATCHES "Ninja|Visual Studio")
    add_subdirectory(ModuleDemo)
else()
    message(STATUS "Skipping ModuleDemo: C++ modules not supported with ${CMAKE_GENERATOR}")
endif()

add_subdirectory(CoroutineDemo)
add_subdirectory(FormatDemo)
add_subdirectory(GeneratorDemo)
add_subdirectory(OspanstreamDemo)
add_subdirectory(PrintDemo)
add_subdirectory(RangesDemo)
add_subdirectory(FunctionBenchmark)
# ModelTest requires boost/asio.hpp - skip if not available
# add_subdirectory(ModelTest)
# SchedTest requires thread_pool.hpp from external include - skip if not available
# add_subdirectory(SchedTest)
