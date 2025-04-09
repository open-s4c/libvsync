# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-arm -L /usr/arm-linux-gnueabi)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mlittle-endian")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -marm")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-thumb-interwork -mfpu=vfp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=cortex-a9")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
