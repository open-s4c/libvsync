cmake_minimum_required(VERSION 3.16)

# To change the compiler, set the following:

# set(CMAKE_C_COMPILER /usr/bin/gcc-7)

project(helloworld LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/../../deps/pkgtools/cmake/CPM.cmake)
set(CPM_libvsync_SOURCE "${CMAKE_SOURCE_DIR}/../..")
CPMAddPackage(NAME libvsync)

add_executable(helloworld helloworld.c)
target_link_libraries(helloworld vsync)

# To generate LSE instructions on Armv8, set the following:
# target_compile_options(helloworld PUBLIC "-march=armv8-a+lse")
