# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
if(NOT VATOMIC_DEV)
    return()
endif()

add_custom_target(vatomic-generate)

# after running vatomic-generate, run clang-format-apply, in order to fix format
# in autogenerted files.
add_custom_command(
    TARGET vatomic-generate
    POST_BUILD
    COMMAND cmake --build "${CMAKE_BINARY_DIR}" --target clang-format-apply
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

set(VATOMIC_RULES ${CMAKE_CURRENT_SOURCE_DIR}/vatomic.rules)

add_subdirectory(atomic)
add_subdirectory(cpp-atomic)
add_subdirectory(test-atomic)

# The following empty file helps LSP servers (eg, clangd) to figure out the
# symbols in the .h.in template files by updating the compile_commands.json
add_executable(empty empty.c)
set_target_properties(empty PROPERTIES COMPILE_OPTIONS "")
target_compile_features(empty INTERFACE c_std_99)
target_include_directories(empty PRIVATE include)
target_link_libraries(empty INTERFACE vatomic tmplr)
