cmake_minimum_required(VERSION 3.20.0)

# 设置工具链 - 使用你的确切路径
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkgpack/vcpkg-master/vcpkg_m/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

project(Astar)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 详细调试信息
message(STATUS "=== CMake 配置调试信息 ===")
message(STATUS "1. 工具链文件: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "2. 目标平台: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "3. 项目名称: ${PROJECT_NAME}")

# 强制设置搜索路径
set(CMAKE_PREFIX_PATH 
    "C:/vcpkgpack/vcpkg-master/vcpkg_m/installed/x64-windows-static"
    "C:/vcpkgpack/vcpkg-master/vcpkg_m/installed/x64-windows"
)

message(STATUS "4. CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# 手动设置模块路径
set(CMAKE_MODULE_PATH 
    "C:/vcpkgpack/vcpkg-master/vcpkg_m/installed/x64-windows-static/share/cpprestsdk"
    "C:/vcpkgpack/vcpkg-master/vcpkg_m/installed/x64-windows/share/cpprestsdk"
)

message(STATUS "5. CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# 查找包
find_package(cpprestsdk REQUIRED)
message(STATUS "6. cpprestsdk 包查找成功")

add_executable(Astar Astar.cpp)
message(STATUS "7. 可执行文件目标创建成功")

target_link_libraries(Astar PRIVATE cpprestsdk::cpprest)
message(STATUS "8. 库链接成功")

message(STATUS "🎉 所有配置步骤完成!")