# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
project(vatomic_cpp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# ##############################################################################
# compilation with C++
# ##############################################################################

file(GLOB TESTS *.cpp)

foreach(TEST ${TESTS})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} vatomic pthread)
    v_add_bin_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
