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
        UDT_DLL/src/apps/shared.cpp
        )
add_executable(udt_cutter
        UDT_DLL/src/apps/app_demo_cutter.cpp
        ${SRC_FILES}
        )

add_executable(udt_json
        UDT_DLL/src/apps/app_demo_json.cpp
${SRC_FILES}
)

add_executable(udt_convert
        UDT_DLL/src/apps/app_demo_converter.cpp
        ${SRC_FILES}
)