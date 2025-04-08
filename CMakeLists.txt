# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.16)

project(
    vatomic
    LANGUAGES C
    VERSION 2.3.1
    DESCRIPTION "VSync atomics")

include(GNUInstallDirs)
include(CheckSymbolExists)
include(cmake/export.cmake)

# Create vatomic header library
add_library(vatomic INTERFACE)
target_include_directories(
    vatomic INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Prepare install targets
install(DIRECTORY include/vsync DESTINATION include)
install(FILES vmm.cat DESTINATION share/vsync/)
install(TARGETS vatomic EXPORT ${PROJECT_TARGETS})

# Enable development targets only if this is the top level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(VATOMIC_DEV_DEFAULT ON)
else()
    set(VATOMIC_DEV_DEFAULT OFF)
endif()

option(VATOMIC_DEV "Enable development targets" ${VATOMIC_DEV_DEFAULT})

if(VATOMIC_DEV)
    # Atomic templating
    add_subdirectory(template)
    add_subdirectory(tmplr)

    # Basic format sanitization
    add_custom_target(
        sanitize-vatomic
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/sanitize.sh include
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/sanitize.sh test
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/license-check.sh
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    # CMake formating checks
    set(CMAKE_FMT_CMD ${PROJECT_SOURCE_DIR}/scripts/cmake-format.sh
                      ${CMAKE_SOURCE_DIR})
    set(CMAKE_STYLE_FILE ${PROJECT_SOURCE_DIR}/.cmake-format)
    add_custom_target(cmake-format-apply COMMAND env STYLE=${CMAKE_STYLE_FILE}
                                                 SILENT=true ${CMAKE_FMT_CMD})
    # Clang formating checks
    set(CLANG_FMT_CMD ${PROJECT_SOURCE_DIR}/scripts/clang-format.sh
                      ${CMAKE_SOURCE_DIR})
    set(CLANG_STYLE_FILE ${PROJECT_SOURCE_DIR}/.clang-format)
    add_custom_target(clang-format-apply COMMAND env STYLE=${CLANG_STYLE_FILE}
                                                 SILENT=true ${CLANG_FMT_CMD})
    add_dependencies(clang-format-apply sanitize-vatomic)

    # General diff check for pipeline
    add_custom_target(diff-check COMMAND git --no-pager diff --exit-code)

    include(CTest)
    include(ProcessorCount)
    include(cmake/v_add_test.cmake)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(verify)
endif()
