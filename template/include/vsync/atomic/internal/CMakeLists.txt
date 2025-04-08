# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
add_subdirectory(config)

# grep templates for each .in .h will be generated
file(GLOB TARGETS *.in)

foreach(TARGET ${TARGETS})
    get_filename_component(TARGET ${TARGET} NAME_WE)
    set(TARGET ${TARGET}.h)
    add_custom_target(
        ${TARGET}
        COMMAND
            $<TARGET_FILE:tmplr.bin>
            ${CMAKE_CURRENT_SOURCE_DIR}/../vatomic.rules
            ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.in >
            ${PROJECT_SOURCE_DIR}/include/vsync/atomic/internal/${TARGET})
    add_dependencies(vatomic-generate ${TARGET})
endforeach()
