# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

find_program(CARGO cargo DOC "Rust installation")
find_program(BPL boogie DOC "Boogie installation")

if(NOT BPL OR NOT CARGO)
    message("Skipping verification. Some dependencies are missing")
    return()
endif()

set(ARMV8_CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(RISCV_CMAKE_C_COMPILER riscv64-linux-gnu-gcc)

set(VSYNC_INCLUDE
    "-I$<JOIN:$<TARGET_PROPERTY:vatomic,INTERFACE_INCLUDE_DIRECTORIES>,;-I>")

set(ARCHS ARMV8 RISCV)

add_custom_command(
    OUTPUT BUILTIN_SCRIPTS
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copy verification scripts for the builtin atomics")

add_custom_command(
    OUTPUT PERMISSION_SCRIPTS
    COMMAND
        chmod +x ${CMAKE_CURRENT_BINARY_DIR}/cleaner.sh && chmod +x
        ${CMAKE_CURRENT_BINARY_DIR}/verify.sh && chmod +x
        ${CMAKE_CURRENT_BINARY_DIR}/generate.sh
    DEPENDS BUILTIN_SCRIPTS
    COMMENT "Give scripts permissions")

add_custom_command(
    OUTPUT ATOMIC_LIST
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lists/*
            ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS BUILTIN_SCRIPTS
    COMMENT "Copy the batches to the parent folder")

set(BUILTIN_FLAGS -DVATOMIC_DISABLE_ARM64_LSE -DVATOMIC_BUILTINS)
set(LLSC_FLAGS -DVATOMIC_DISABLE_ARM64_LSE)
set(LSE_FLAGS -march=armv8-a+lse)
set(LXE_FLAGS -march=armv8-a+lse -DVATOMIC_ENABLE_ARM64_LXE)

set(RISCV_TARGETS BUILTIN)
set(ARMV8_TARGETS LLSC BUILTIN LSE LXE)

set(ARMV8_FLAGS -O2 -x c -fkeep-inline-functions -S -mno-outline-atomics)
set(RISCV_FLAGS -O2 -x c -fkeep-inline-functions -S)

set(ATOMIC_CORE_GROUPS vatomic8 vatomic16 vatomic32 vatomic64 vatomicsz)
set(ATOMIC_AWAIT_GROUPS vatomic32 vatomic64)
set(ATOMIC_PTR_GROUPS vatomicptr)
set(LISTS CORE AWAIT PTR)
set(ORDERINGS acq rel rlx "")

set(ALL_ATOMIC_GROUPS
    vatomic8
    vatomic16
    vatomic32
    vatomic64
    vatomicsz
    vatomicptr
    vatomic_fence)

foreach(GRP ${ALL_ATOMIC_GROUPS})
    set(${GRP}_ATOMICS "")
endforeach()

# Fences
foreach(ORD ${ORDERINGS})
    list(APPEND vatomic_fence_ATOMICS vatomic_fence_${ORD})
endforeach()

# Core, Await, ptr
foreach(LIST ${LISTS})
    foreach(ATM_GRP ${ATOMIC_${LIST}_GROUPS})
        string(TOLOWER "lists/vatomic_${LIST}.txt" LIST_FILE)
        file(STRINGS ${LIST_FILE} ATM_LIST)
        foreach(ATM ${ATM_LIST})
            set(ATM_PREF ${ATM_GRP}_${ATM})
            list(APPEND ${ATM_GRP}_ATOMICS ${ATM_PREF})
        endforeach()
    endforeach()
endforeach()

foreach(ARCH ${ARCHS})
    string(TOLOWER "${ARCH}" SUB_DIR)
    set(SUFFIX ${SUB_DIR})
    set(SUB_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SUB_DIR})
    set(ASM_FILE ${SUB_DIR}/atomics.s)

    foreach(TARGET ${${ARCH}_TARGETS})
        string(TOLOWER "${SUFFIX}_${TARGET}" COMPOSED_TARGET)

        if(${ARCH} STREQUAL "ARMV8")
            add_custom_command(
                OUTPUT ASM_${COMPOSED_TARGET}
                COMMAND
                    ${${ARCH}_CMAKE_C_COMPILER} ${${TARGET}_FLAGS}
                    ${${ARCH}_FLAGS} "${VSYNC_INCLUDE}"
                    ${CMAKE_SOURCE_DIR}/include/vsync/atomic.h -o ${ASM_FILE} &&
                    ${CMAKE_CURRENT_BINARY_DIR}/cleaner.sh ${ASM_FILE}
                COMMAND_EXPAND_LISTS
                DEPENDS vatomic BUILTIN_SCRIPTS PERMISSION_SCRIPTS
                COMMENT
                    "Generating ${COMPOSED_TARGET} builtin atomics (with cleaner)"
            )
        else()
            add_custom_command(
                OUTPUT ASM_${COMPOSED_TARGET}
                COMMAND
                    ${${ARCH}_CMAKE_C_COMPILER} ${${TARGET}_FLAGS}
                    ${${ARCH}_FLAGS} "${VSYNC_INCLUDE}"
                    ${CMAKE_SOURCE_DIR}/include/vsync/atomic.h -o ${ASM_FILE}
                COMMAND_EXPAND_LISTS
                DEPENDS vatomic BUILTIN_SCRIPTS PERMISSION_SCRIPTS
                COMMENT "Generating ${COMPOSED_TARGET} builtin atomics")
        endif()
        foreach(GRP ${ALL_ATOMIC_GROUPS})
            set(DEPS_VAR "${GRP}_${COMPOSED_TARGET}_DEPS")
            set(${DEPS_VAR} "")

            foreach(ATM ${${GRP}_ATOMICS})
                add_test(NAME check_${COMPOSED_TARGET}_${ATM}
                         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/verify.sh ${ATM}
                                 ${SUFFIX})
                set_tests_properties(
                    check_${COMPOSED_TARGET}_${ATM}
                    PROPERTIES LABELS ${COMPOSED_TARGET}_${GRP})

                add_custom_command(
                    OUTPUT GEN_${COMPOSED_TARGET}_${ATM}
                    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/generate.sh ${ATM}
                            ${SUFFIX}
                    DEPENDS ASM_${COMPOSED_TARGET} ATOMIC_LIST
                    COMMENT "Generating Boogie for ${ATM} (${COMPOSED_TARGET})")

                list(APPEND ${DEPS_VAR} GEN_${COMPOSED_TARGET}_${ATM})
            endforeach()

            add_custom_target(
                build_boogie_${COMPOSED_TARGET}_${GRP}
                DEPENDS ${${DEPS_VAR}} ASM_${COMPOSED_TARGET}
                        PERMISSION_SCRIPTS)
        endforeach()
    endforeach()
endforeach()
