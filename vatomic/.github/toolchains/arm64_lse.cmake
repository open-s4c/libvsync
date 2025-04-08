# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-aarch64 -L /usr/aarch64-linux-gnu)

option(VATOMIC_ENABLE_ARM64_LXE
       "Use LXE atomics ldx/stx in fastpath, LSE in slowpath" off)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+lse")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

if(VATOMIC_ENABLE_ARM64_LXE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVATOMIC_ENABLE_ARM64_LXE")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
