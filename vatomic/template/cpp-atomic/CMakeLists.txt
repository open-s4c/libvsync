# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

set(unsigned_core_TYPES
    u8
    u16
    u32
    u64
    sz
    ptr
    bool)
set(signed_core_TYPES s8 s16 s32 s64)
set(TARGETS signed_core unsigned_core)
foreach(TARGET ${TARGETS})
    set(TMPL ${TARGET}_TY.hpp)
    foreach(TY ${${TARGET}_TYPES})
        set(OUT core_${TY}.hpp)
        add_custom_target(
            ${OUT}
            COMMAND
                ${TMPLR_PROGRAM} -FTY=${TY} ${VATOMIC_RULES}
                ${CMAKE_CURRENT_SOURCE_DIR}/${TMPL}.in >
                ${PROJECT_SOURCE_DIR}/include/vsync/atomic/${OUT})
        add_dependencies(${OUT} tmplr-build)
        add_dependencies(vatomic-generate ${OUT})
    endforeach()
endforeach()
