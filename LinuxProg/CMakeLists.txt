# Linux programming exercises
cmake_minimum_required(VERSION 3.28)

add_subdirectory(CommandLineTesting)
add_subdirectory(Debugging)
add_subdirectory(EnvironmentVariable)
add_subdirectory(InternetClient)
# add_subdirectory(InternetServer) # disabled
# add_subdirectory(MyShell) # disabled
add_subdirectory(PracticalExercises)
# add_subdirectory(ProcessOutput) # disabled
add_subdirectory(Countdown)
add_subdirectory(ProgressBar)
add_subdirectory(TcpClient)
add_subdirectory(ThreadPoolModuleTest)

# Linux-only projects - skip on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(ProcessPools)
    add_subdirectory(RemoteControlClient)
    add_subdirectory(RemoteControlServer)
    add_subdirectory(WriteSimulation)
    add_subdirectory(SystemProcessTest)
    add_subdirectory(TcpServer)
    add_subdirectory(ThreadMutex)
    add_subdirectory(VimWriteTest)
    add_subdirectory(VirtualAddress)
endif()
