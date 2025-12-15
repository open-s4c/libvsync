# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

file(GLOB TESTS *.c)
foreach(TST_FILE IN ITEMS ${TESTS})
    get_filename_component(TEST ${TST_FILE} NAME_WE)

    add_executable(${TEST} ${TST_FILE})

    target_link_libraries(${TEST} vatomic pthread)

    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})
endforeach()
