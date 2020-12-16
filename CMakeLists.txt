cmake_minimum_required(VERSION 3.14)
project(mobilfone_test)

set(CMAKE_CXX_STANDARD 17)

set(Boost_INCLUDE_DIR C:/dev/vcpkg/installed/x64-windows-static/include)
set(Boost_LIBRARY_DIR C:/dev/vcpkg/installed/x64-windows-static/lib)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})

add_executable(mobilfone_test main.cpp calculator.hpp)

target_link_libraries(mobilfone_test ${Boost_LIBRARY_DIR})