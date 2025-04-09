# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
set(V_TEST_LABELS "CROSS")
# currently not installed with distros
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/templates)
    add_subdirectory(templates)
endif()

file(GLOB TESTS *test*.c)
foreach(test IN ITEMS ${TESTS})
    get_filename_component(TEST ${test} NAME_WE)
    # add the executable
    add_executable(${TEST} ${test})
    # link libs
    target_link_libraries(${TEST} vatomic pthread)
    # add test
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})
endforeach()
