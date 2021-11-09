cmake_minimum_required(VERSION 3.20)
project(uberdemotools)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread rt)

include_directories(UDT_DLL/include)
include_directories(UDT_DLL/src)
include_directories(UDT_DLL/src/apps)

file (GLOB SRC_FILES
        UDT_DLL/src/*.h
        UDT_DLL/src/*.hpp
        UDT_DLL/src/*.cpp
        UDT_DLL/src/apps/app_demo_cutter.cpp
        UDT_DLL/src/apps/shared.cpp
        )
add_executable(udt_cutter
        ${SRC_FILES}
        )

