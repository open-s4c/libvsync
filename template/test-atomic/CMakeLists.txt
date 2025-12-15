# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
add_custom_target(vatomic-test-generate)

file(GLOB TEMPLATES *TY.c.in)
set(GEN_TEST_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/test/atomics_gen)

set(ut_test_TY_TYPES u8 u16 u32 u64 sz ptr)
set(ut_test_await_TY_TYPES u32 u64 ptr)
set(mt_test_await_TY_TYPES u32 u64 ptr)
set(mt_test_rmw_TY_TYPES u32 u64 sz ptr)

foreach(TEMPLATE_FILE IN ITEMS ${TEMPLATES})
    get_filename_component(TEMPLATE_NAME ${TEMPLATE_FILE} NAME_WE)
    foreach(TY ${${TEMPLATE_NAME}_TYPES})
        string(REPLACE TY ${TY} TEST_FILE ${TEMPLATE_NAME}.c)
        add_custom_target(
            ${TEST_FILE}
            COMMAND ${TMPLR_PROGRAM} -FTY=${TY} ${VATOMIC_RULES}
                    ${TEMPLATE_FILE} > ${GEN_TEST_OUTPUT_DIR}/${TEST_FILE})
        add_dependencies(${TEST_FILE} tmplr-build)
        add_dependencies(vatomic-test-generate ${TEST_FILE})
    endforeach()
endforeach()

set(FUNS
    fence
    read
    write
    cmpxchg
    xchg
    get_max
    get_add
    get_sub
    get_and
    get_or
    get_xor
    get_inc
    get_dec
    max_get
    add_get
    sub_get
    and_get
    or_get
    xor_get
    inc_get
    dec_get
    max
    and
    or
    xor
    add
    sub
    inc
    dec
    await_eq
    await_neq
    await_lt
    await_le
    await_gt
    await_ge
    await_le_add
    await_le_sub
    await_le_set
    await_lt_add
    await_lt_sub
    await_lt_set
    await_gt_add
    await_gt_sub
    await_gt_set
    await_ge_add
    await_ge_sub
    await_ge_set
    await_neq_add
    await_neq_sub
    await_neq_set
    await_eq_add
    await_eq_sub
    await_eq_set)

foreach(FUN IN ITEMS ${FUNS})
    set(OUTPUT_FILE "call_FUNC.c")
    string(REPLACE FUNC ${FUN} OUTPUT_FILE ${OUTPUT_FILE})
    add_custom_target(
        ${OUTPUT_FILE}
        COMMAND
            ${TMPLR_PROGRAM} -FFUNC=${FUN} ${VATOMIC_RULES}
            ${CMAKE_CURRENT_SOURCE_DIR}/call_FUNC.c.in >
            ${GEN_TEST_OUTPUT_DIR}/call_tests/${OUTPUT_FILE})
    add_dependencies(${OUTPUT_FILE} tmplr-build)
    add_dependencies(vatomic-test-generate ${OUTPUT_FILE})
endforeach(FUN)

# make sure files stay formatted
add_custom_command(
    TARGET vatomic-test-generate
    POST_BUILD
    COMMAND cmake --build "${CMAKE_BINARY_DIR}" --target clang-format-apply
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
