# Create a generator expression with a list of `-I/path/to/include_directory`
# flags The genex is adapted from the recommended approach here in the CMake
# manual:
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#whitespace-and-quoting
set(include_prop "$<TARGET_PROPERTY:vsync,INTERFACE_INCLUDE_DIRECTORIES>")
set(VATOMIC_INC "$<$<BOOL:${include_prop}>:-I$<JOIN:${include_prop},;-I>>")

# ##############################################################################
# pause test
# ##############################################################################

add_executable(vatomic_pause pause_test.c)
target_link_libraries(vatomic_pause vsync)
v_add_bin_test(NAME vatomic-pause COMMAND vatomic_pause)

# ##############################################################################
# compilation with C
# ##############################################################################

add_executable(vatomic_compile_c vatomic_empty.c)
target_link_libraries(vatomic_compile_c vsync)
v_add_bin_test(NAME vatomic-compile-c COMMAND vatomic_compile_c)

# ##############################################################################
# vatomicptr markable and stamped
# ##############################################################################

add_executable(atomicptr_markable atomicptr_markable_test.c)
target_link_libraries(atomicptr_markable vsync pthread)
v_add_bin_test(NAME atomicptr_markable COMMAND atomicptr_markable)

add_executable(atomicptr_stamped atomicptr_stamped_test.c)
target_link_libraries(atomicptr_stamped vsync pthread)
v_add_bin_test(NAME atomicptr_stamped COMMAND atomicptr_stamped)

# ##############################################################################
# macro leftovers
#
# The following targets look for macro leftovers, ie, macros that were defined
# when including atomic.h, but which are not supposed to used by user. We want
# to undefine these macros in vatomic_cleanup.h. Macro leftovers that are
# intended must be whitelisted below or added.
# ##############################################################################

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
        -include vsync/atomic/atomicptr_markable.h #
        -include vsync/atomic/atomicptr_stamped.h #
        - < /dev/null #
        | sort #
        > vatomic-definitions.h
    VERBATIM COMMAND_EXPAND_LISTS
    # Rerunning whenever vsync changes is a very conservative option, that
    # should definitely work
    DEPENDS vsync)

add_custom_target(vatomic-definitions
                  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h")

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt"
    COMMAND
        diff "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h"
        "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h" #
        | grep -e '^>' #
        | cut -d' ' -f3 #
        | cut -d'\(' -f1 #
        | grep -E -v --file=${CMAKE_CURRENT_SOURCE_DIR}/leftover_patterns.txt #
        | tee "${CMAKE_CURRENT_BINARY_DIR}/leftovers.txt" #
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/base-definitions.h"
            "${CMAKE_CURRENT_BINARY_DIR}/vatomic-definitions.h"
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

add_test(NAME macro-leftovers-test COMMAND bash -c "${LEFTOVERS_CHECK_CMD}"
                                           DEPENDS leftovers)

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
        DEPENDS vsync "${CMAKE_CURRENT_SOURCE_DIR}/dispatcher_test.c"
        COMMAND_EXPAND_LISTS VERBATIM)

    add_custom_target(expand ALL
                      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/vatomic_expand.h")

    # ##########################################################################
    # test vatomic dispatcher
    # ##########################################################################
    add_executable(dispatcher_test dispatcher_test.c)
    target_compile_options(dispatcher_test PUBLIC ${TEST_OPTS})
    target_link_libraries(dispatcher_test vsync)
    v_add_bin_test(NAME dispatcher_test COMMAND dispatcher_test)

    # ##########################################################################
    # test freestanding
    # ##########################################################################
    add_executable(freestanding_test freestanding_test.c)
    target_compile_options(freestanding_test PUBLIC ${TEST_OPTS} -nostdinc)
    configure_file(include/vfreestanding.h.tpl include/vfreestanding.h)
    target_include_directories(freestanding_test
                               PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(freestanding_test vsync)
    v_add_bin_test(NAME freestanding_test COMMAND freestanding_test)
endif()
