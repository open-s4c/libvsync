# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
add_library(tmplr INTERFACE)
target_include_directories(tmplr INTERFACE include)

add_executable(tmplr.bin tmplr.c)
