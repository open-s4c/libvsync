# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# set CMAKE_PREFIX_PATH with the CMAKE_INSTALL_PATH used to install vatomic

cmake_minimum_required(VERSION 3.16)
project(test LANGUAGES C)
find_package(vatomic CONFIG REQUIRED)
add_executable(test test.c)
target_link_libraries(test PRIVATE vatomic::vatomic)
