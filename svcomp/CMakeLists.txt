# ##############################################################################
# This script exports a selection of test cases in a format submittable to
# SV-COMP (https://sv-comp.sosy-lab.org/).
#
# The TEST_SRCS variable contains the relative paths to each test case file. For
# each test case, the file is expanded with the compiler's -E option and a test
# .yml file is created correspondingly. The pairs of files are put in
# "svcomp/target" subdirectory of the build directory.
#
# See target comments below for more information.
# ##############################################################################

# get all spinlock tests
file(GLOB TEST_SRCS ${PROJECT_SOURCE_DIR}/test/spinlock/*.c)

# pick a few bounded queue tests
set(TEST_SRCS
    ${TEST_SRCS} ${PROJECT_SOURCE_DIR}/test/queue/bounded_spsc.c
    ${PROJECT_SOURCE_DIR}/test/queue/bounded_mpmc_check_empty.c
    ${PROJECT_SOURCE_DIR}/test/queue/bounded_mpmc_check_full.c)

# By default test cases use the default.yml.in as template. Set
# CONFIG_<test-case-name> to select a different config template or set OFF to
# disable the test case.

set(CONFIG_seqlock OFF)
set(CONFIG_seqcount OFF)
set(CONFIG_rec_seqlock OFF)

set(CONFIG_bounded_spsc data-race)
set(CONFIG_caslock data-race)
set(CONFIG_hemlock data-race)
set(CONFIG_rec_spinlock data-race)
set(CONFIG_twalock data-race)

set(CONFIG_clhlock memsafety)
set(CONFIG_bounded_mpmc_check_empty memsafety)

# ##############################################################################
# svcomp target
# ##############################################################################

# svcomp-expand target expands the selected test cases and for each of them
# creates a yml test file. All files are placed in ${TARGET_DIR}.
add_custom_target(svcomp-expand)

# svcomp-sanity target compiles the selected cases after expansion as a basic
# sanity check.
add_custom_target(svcomp-sanity)

# svcomp target expands the testcases and does basic sanity check
add_custom_target(svcomp)
add_dependencies(svcomp svcomp-expand svcomp-sanity)

# ##############################################################################
# flags and paths
# ##############################################################################
set(INITIAL_YEAR "2024")
string(TIMESTAMP RANGE "%Y")
if(NOT ${INITAL_YEAR} STREQUAL ${RANGE})
    set(RANGE "${INITIAL_YEAR}-${RANGE}")
endif()
set(TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/target)
set(LIBVSYNC_DIR ${PROJECT_SOURCE_DIR})
set(FLAGS -DVSYNC_VERIFICATION -DVSYNC_VERIFICATION_GENERIC
          -DVATOMIC_ENABLE_ATOMIC_SC)
set(CFLAGS -m32 --std=c99)
set(EXPAND_FLAGS -E -P ${CFLAGS} ${FLAGS})

set(LIBVSYNC_VERSION ${PROJECT_VERSION})
set(COMPILER_VERSION_LINE "${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
set(COPYRIGHT "SPDX-FileCopyrightText: ${RANGE} Huawei Technologies Co., Ltd.")
set(LICENSE "SPDX-License-Identifier: MIT")

# ##############################################################################
# copy source files and apply bugs.patch
# ##############################################################################
add_custom_target(
    svcomp-dir
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_DIR}/src)

# svcomp-copy target copies include and test files into ${TARGET_DIR}.
add_custom_target(
    svcomp-copy
    # copy include files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/include
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test
    # copy files for distribution
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/LICENSE ${TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include
            ${TARGET_DIR}/src/include
    COMMAND
        sed #
        -e '1i\# ${COPYRIGHT}\\n\# ${LICENSE}\\n' #
        ${CMAKE_CURRENT_SOURCE_DIR}/Makefile.in #
        > ${TARGET_DIR}/Makefile
    COMMAND
        sed #
        -e '1i${COPYRIGHT}\\n${LICENSE}\\n' #
        ${CMAKE_CURRENT_SOURCE_DIR}/bugs.patch #
        > ${TARGET_DIR}/src/bugs.patch
    COMMAND
        sed #
        -e '1i${COPYRIGHT}\\n${LICENSE}\\n' #
        -e 's^@LIBVSYNC_VERSION@^${LIBVSYNC_VERSION}^g' #
        -e 's^@COMPILER_VERSION_LINE@^${COMPILER_VERSION_LINE}^g' #
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md.in #
        > ${TARGET_DIR}/README.md
    DEPENDS svcomp-dir)
add_dependencies(svcomp svcomp-copy)

foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST ${TEST_SRC} NAME_WE)

    if(DEFINED CONFIG_${TEST} AND NOT CONFIG_${TEST})
        continue()
    endif()

    file(RELATIVE_PATH SOURCE_FILE ${PROJECT_SOURCE_DIR} ${TEST_SRC})
    get_filename_component(TEST_DIR ${SOURCE_FILE} DIRECTORY)

    add_custom_target(
        copy-${TEST}.c
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SRC}
                ${CMAKE_CURRENT_BINARY_DIR}/${TEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_DIR}/src/${TEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SRC}
                ${TARGET_DIR}/src/${TEST_DIR})
    add_dependencies(svcomp-copy copy-${TEST}.c)
endforeach()

add_custom_target(
    svcomp-bugs
    COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/bugs.patch
    DEPENDS svcomp-copy)

# ##############################################################################
# create targets for each test case
# ##############################################################################
foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST ${TEST_SRC} NAME_WE)
    file(RELATIVE_PATH SOURCE_FILE ${PROJECT_SOURCE_DIR} ${TEST_SRC})

    if(DEFINED CONFIG_${TEST} AND NOT CONFIG_${TEST})
        continue()
    endif()

    # expand test case
    add_custom_target(
        expand-${TEST}.i
        COMMAND
            ${CMAKE_C_COMPILER} ${EXPAND_FLAGS} #
            -I${CMAKE_CURRENT_BINARY_DIR}/include #
            -o ${TEST}.i ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}
        DEPENDS svcomp-bugs)

    # add a comment header to the file and place it in target/
    add_custom_target(
        export-${TEST}.i
        COMMAND
            sed #
            -e 's^@RANGE@^${RANGE}^g' #
            -e 's^@SOURCE_FILE@^${SOURCE_FILE}^g' #
            -e 's^@LIBVSYNC_VERSION@^${LIBVSYNC_VERSION}^g' #
            -e 's^@COMPILER_VERSION_LINE@^${COMPILER_VERSION_LINE}^g' #
            -e 's^@EXPAND_FLAGS@^${EXPAND_FLAGS}^g' #
            ${CMAKE_CURRENT_SOURCE_DIR}/header_text.c.in #
            > ${TARGET_DIR}/${TEST}.i
        COMMAND cat ${TEST}.i >> ${TARGET_DIR}/${TEST}.i
        DEPENDS expand-${TEST}.i)
    add_dependencies(svcomp-expand export-${TEST}.i)

    # create test yml files for svcomp
    set(CONFIG_FILE default.yml.in)
    if(DEFINED CONFIG_${TEST})
        set(CONFIG_FILE "${CONFIG_${TEST}}.yml.in")
    endif()

    add_custom_target(
        ${TEST}.yml
        COMMAND
            sed "s/FILENAME/${TEST}.i/g"
            ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE} >
            ${TARGET_DIR}/${TEST}.yml
        DEPENDS svcomp-dir)
    add_dependencies(svcomp-expand ${TEST}.yml)

    # configure test case yml
    add_custom_target(
        ${TEST}.bin
        COMMAND ${CMAKE_C_COMPILER} ${CFLAGS} -o ${TEST}.bin ${TEST}.i -lpthread
        DEPENDS expand-${TEST}.i)
    add_dependencies(svcomp-sanity ${TEST}.bin)

endforeach()
