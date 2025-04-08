# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
project(vatomic_cpp LANGUAGES CXX)
# ##############################################################################
# compilation with C++
# ##############################################################################

add_executable(vatomic_compile_cxx vatomic_empty.cpp)
target_link_libraries(vatomic_compile_cxx vatomic)
v_add_bin_test(NAME vatomic-compile-cxx COMMAND vatomic_compile_cxx)
