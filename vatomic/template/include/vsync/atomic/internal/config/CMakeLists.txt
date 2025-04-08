# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT

# grep templates for each .in .h will be generated
file(GLOB TARGETS *.in)

set(ALL_TYPES
    u8
    u16
    u32
    u64
    sz
    ptr
    fnc)

foreach(TARGET IN ITEMS ${TARGETS})

    get_filename_component(TARGET ${TARGET} NAME_WE)

    if(DEFINED ${TARGET}_TYPES)
        set(TYPES ${${TARGET}_TYPES})
    else()
        set(TYPES ${ALL_TYPES})
    endif()

    set(TMPL ${TARGET}.h)

    foreach(TY IN ITEMS ${TYPES})
        string(REPLACE TY ${TY} OUT ${TMPL})
        add_custom_target(
            ${OUT}
            COMMAND
                $<TARGET_FILE:tmplr.bin> -DTY=${TY}
                ${CMAKE_CURRENT_SOURCE_DIR}/../../vatomic.rules
                ${CMAKE_CURRENT_SOURCE_DIR}/${TMPL}.in >
                ${PROJECT_SOURCE_DIR}/include/vsync/atomic/internal/config/${OUT}
        )
        add_dependencies(vatomic-generate ${OUT})
    endforeach()
endforeach()
