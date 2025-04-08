# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT
set(V_TEST_LABELS "CROSS")

# Create a generator expression with a list of `-I/path/to/include_directory`
# flags The genex is adapted from the recommended approach here in the CMake
# manual:
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#whitespace-and-quoting
set(include_prop "$<TARGET_PROPERTY:vatomic,INTERFACE_INCLUDE_DIRECTORIES>")
set(VATOMIC_INC "$<$<BOOL:${include_prop}>:-I$<JOIN:${include_prop},;-I>>")

# ##############################################################################
# pause test
# ##############################################################################

add_executable(vatomic_pause pause_test.c)
target_link_libraries(vatomic_pause vatomic)
v_add_bin_test(NAME vatomic-pause COMMAND vatomic_pause)

# ##############################################################################
# Check - Configs
# ##############################################################################
set(AVAILABLE_CONFIG VATOMIC_ENABLE_ATOMIC_SC VATOMIC_ENABLE_ATOMIC_RLX DEFAULT)
set(AVAILABLE_ARCHS __arm__ __aarch64__ __x86_64__)
set(__arm__VARIANTS VATOMIC_ARMV8 VATOMIC_ARMV7)
set(__aarch64__VARIANTS __ARM_FEATURE_ATOMICS VATOMIC_ENABLE_ARM64_LLSC)
set(__x86_64__VARIANTS BUILTIN)

add_custom_target(vatomic-expand)

foreach(CONFIG IN ITEMS ${AVAILABLE_CONFIG})
    foreach(ARCH IN ITEMS ${AVAILABLE_ARCHS})
        foreach(VARIANT IN ITEMS ${${ARCH}VARIANTS})
            string(TOLOWER "expanded_${CONFIG}_${ARCH}_${VARIANT}.c"
                           OUTPUT_FILE)
            add_custom_command(
                TARGET vatomic-expand
                POST_BUILD
                COMMAND
                    ${CMAKE_C_COMPILER} -I${PROJECT_SOURCE_DIR}/include
                    -I${PROJECT_SOURCE_DIR}/atomics/include -D${CONFIG}
                    -D${ARCH} -D${VARIANT} -E
                    ${CMAKE_CURRENT_SOURCE_DIR}/vatomic_empty.c -o
                    ${OUTPUT_FILE}
                BYPRODUCTS ${PROJECT_BINARY_DIR}/${OUTPUT_FILE}
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
        endforeach(VARIANT)
    endforeach(ARCH)
endforeach(CONFIG)

# ##############################################################################
# compilation with C
# ##############################################################################

add_executable(vatomic_compile_c vatomic_empty.c)
target_link_libraries(vatomic_compile_c vatomic)
v_add_bin_test(NAME vatomic-compile-c COMMAND vatomic_compile_c)

# ##############################################################################
# macro leftovers
#
# The following targets look for macro leftovers, ie, macros that were defined
# when including atomic.h, but which are not supposed to used by user. We want
# to undefine these macros in vatomic_cleanup.h. Macro leftovers that are
# intended must be whitelisted below or added.
# ##############################################################################
if(NOT CMAKE_CROSSCOMPILING)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h"
        COMMAND
            ${CMAKE_C_COMPILER} ${CFLAGS} -dM -E #
            -include stdint.h #
            -include stddef.h #
            -include inttypes.h #
            -include limits.h #
            -include assert.h #
            -include stdbool.h #
            - < /dev/null #
            | sort #
            > "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h")

    add_custom_target(base-definitions
                      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h")

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h"
        COMMAND
            ${CMAKE_C_COMPILER} ${CFLAGS} -dM -E #
            "${VATOMIC_INC}" #
            -include stdint.h #
            -include stddef.h #
            -include inttypes.h #
            -include limits.h #
            -include assert.h #
            -include stdbool.h #
            -include vsync/atomic.h #
            - < /dev/null #
            | sort #
            > vatomic-definitions.h
        VERBATIM COMMAND_EXPAND_LISTS
        # Rerunning whenever vsync changes is a very conservative option, that
        # should definitely work
        DEPENDS vatomic)

    add_custom_target(
        vatomic-definitions
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h")

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt"
        COMMAND
            diff "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h"
            "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h" #
            | grep -e '^>' #
            | cut -d' ' -f3 #
            | cut -d'\(' -f1 #
            | grep -E -v
            --file=${CMAKE_CURRENT_SOURCE_DIR}/leftover_patterns.txt #
            | tee "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt" #
        DEPENDS base-definitions #
                vatomic-definitions #
                "${CMAKE_CURRENT_SOURCE_DIR}/leftover_patterns.txt")

    add_custom_target(leftovers ALL
                      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt")

    set(LEFTOVERS "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt")
    set(LEFTOVERS_CHECK_CMD
        "\
    if [[ -f ${LEFTOVERS} ]] && [[ ! -s ${LEFTOVERS} ]]; \
    then cat ${LEFTOVERS} && true; \
    else cat ${LEFTOVERS} && false; \
    fi")

    set(BUILD_LEFTOVERS_CMD "build-leftovers")
    set(LEFTOVERS_TEST "macro-leftovers-test")

    add_test(NAME ${LEFTOVERS_TEST} COMMAND bash -c "${LEFTOVERS_CHECK_CMD}"
                                            DEPENDS leftovers)

    # add fixtures to trigger leftovers generation without calling make
    # explicitly

    add_test(NAME "${BUILD_LEFTOVERS_CMD}"
             COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target
                     leftovers)

    set_tests_properties("${BUILD_LEFTOVERS_CMD}"
                         PROPERTIES FIXTURES_SETUP "${BUILD_LEFTOVERS_CMD}")

    set_tests_properties("${LEFTOVERS_TEST}"
                         PROPERTIES FIXTURES_REQUIRED "${BUILD_LEFTOVERS_CMD}")

endif()
# ##############################################################################
# generate an expanded vatomic file for manual inspection
# ##############################################################################
separate_arguments(CFLAGS UNIX_COMMAND "${CMAKE_C_FLAGS}")

if(NOT LIBVSYNC_OPEN_DISTRO_TESTING)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/vatomic_expand.h"
        COMMAND
            ${CMAKE_C_COMPILER} -E ${CFLAGS} "${VATOMIC_INC}" -include
            vsync/atomic.h ${CMAKE_CURRENT_SOURCE_DIR}/dispatcher_test.c >
            "${CMAKE_CURRENT_BINARY_DIR}/vatomic_expand.h"
        DEPENDS vatomic "${CMAKE_CURRENT_SOURCE_DIR}/dispatcher_test.c"
        COMMAND_EXPAND_LISTS VERBATIM)

    add_custom_target(expand ALL
                      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/vatomic_expand.h")

    # ##########################################################################
    # test vatomic dispatcher
    # ##########################################################################
    add_executable(dispatcher_test dispatcher_test.c)
    target_compile_options(dispatcher_test PUBLIC ${TEST_OPTS})
    target_link_libraries(dispatcher_test vatomic)
    v_add_bin_test(NAME dispatcher_test COMMAND dispatcher_test)

    # ##########################################################################
    # test freestanding
    # ##########################################################################
    add_executable(freestanding_test freestanding_test.c)
    target_compile_options(freestanding_test PUBLIC ${TEST_OPTS} -nostdinc)
    configure_file(include/vfreestanding.h.tpl include/vfreestanding.h)
    target_include_directories(freestanding_test
                               PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(freestanding_test vatomic)
    v_add_bin_test(NAME freestanding_test COMMAND freestanding_test)
endif()
