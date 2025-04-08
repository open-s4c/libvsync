# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARMEB_PATH /tmp/gcc-linaro-7.5.0-2019.12-x86_64_armeb-linux-gnueabi/bin)
set(CMAKE_C_COMPILER ${ARMEB_PATH}/armeb-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${ARMEB_PATH}/armeb-linux-gnueabi-g++)
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-armeb -L /usr/armeb-linux-gnueabi/libc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbig-endian")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -marm")
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -mno-thumb-interwork -mfpu=vfp -msoft-float")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CROSS_COMPILING_ARMEB TRUE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
