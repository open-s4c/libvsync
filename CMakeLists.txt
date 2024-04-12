cmake_minimum_required(VERSION 3.16)

project(
    libvsync
    LANGUAGES C
    VERSION 0.0.0
    DESCRIPTION "Verified library of atomics, synchronization primitives and concurrent data structures")

option(LIBVSYNC_ADDRESS_SANITIZER "Compile with -fsanitize=address" "off")
option(LIBVSYNC_CODECHECK_BINSCOPE "Compile with necessary flags for binscope" "off")

include(GNUInstallDirs)
include(cmake/export.cmake)
include(CheckSymbolExists)

add_library(vsync INTERFACE)

target_include_directories(
    vsync INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(DIRECTORY include/vsync DESTINATION include)
install(TARGETS vsync EXPORT ${PROJECT_TARGETS})

##############################################################
#   Check for memset_s memcpy_s existence
##############################################################
check_symbol_exists(memset_s "string.h" VSYNC_MEMSET_S_EXISTS)
if(VSYNC_MEMSET_S_EXISTS)
    target_compile_definitions(vsync INTERFACE "VSYNC_MEMSET_S_EXISTS")
endif()
check_symbol_exists(memcpy_s "string.h" VSYNC_MEMCPY_S_EXISTS)
if(VSYNC_MEMCPY_S_EXISTS)
    target_compile_definitions(vsync INTERFACE "VSYNC_MEMCPY_S_EXISTS")
endif()

##############################################################
#   Important Config must be set
##############################################################
set(ATOMICS_DIR ${PROJECT_SOURCE_DIR}/include/vsync/atomic)

##############################################################
#   Commands
##############################################################
set(FMT_CMD ${PROJECT_SOURCE_DIR}/scripts/clang-format.sh
               ${CMAKE_SOURCE_DIR})
set(STYLE_FILE ${PROJECT_SOURCE_DIR}/.clang-format)

add_custom_target(clang-format-apply ALL
    COMMAND env STYLE=${STYLE_FILE} SILENT=true ${FMT_CMD})

add_custom_target(clang-format-check ALL
    COMMAND env STYLE=${STYLE_FILE} ${FMT_CMD})

add_custom_target(
    sanitize-vsync
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/sanitize.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_custom_target(diff-check COMMAND git --no-pager diff --exit-code)

add_dependencies(clang-format-apply sanitize-vsync)

add_dependencies(clang-format-check sanitize-vsync)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(LIBVSYNC_OPEN_DISTRO_TESTING "on")
    enable_testing()
    add_subdirectory(template)
    add_subdirectory(test)
    add_subdirectory(verify)
    add_subdirectory(tmplr)
    if(LIBVSYNC_ADDRESS_SANITIZER)
        target_compile_options(vsync INTERFACE -fsanitize=address)
        target_compile_definitions(vsync INTERFACE VSYNC_ADDRESS_SANITIZER)
        target_link_options(vsync INTERFACE -fsanitize=address)
    endif()
    if(LIBVSYNC_CODECHECK_BINSCOPE)
        target_compile_definitions(vsync INTERFACE _FORTIFY_SOURCE=2)
        target_compile_options(vsync INTERFACE -fstack-protector-strong -fstack-protector-all -fPIE -fPIC -O2)
        target_link_options(vsync INTERFACE -s -pie -Wl,-z,relro,-z,now)
    endif()
endif()

