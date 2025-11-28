cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkgpack/vcpkg-master/vcpkg_m/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

project(Astar)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpprestsdk REQUIRED)


add_executable(Astar Astar.cpp)

if(MSVC)
    set_property(TARGET Astar PROPERTY 
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_libraries(Astar PRIVATE cpprestsdk::cpprest)

message(STATUS "配置完成！已强制启用静态运行时 (MT/MTd)")