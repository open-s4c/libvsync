target_include_directories(
    vsync INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)

# detect test files
file(GLOB TEST_FILES test*.c)

# find the number of processors
ProcessorCount(PCOUNT)

if(${LIBVSYNC_CROSS_TESTS})
    # QEMU might be too slow, we cannot really stress test
    set(NUM_THREADS 4)
    set(ITERATIONS 10)
else()
    math(EXPR NUM_THREADS "${PCOUNT}*2")
    if(${VSYNCER_CHECK_FULL})
        set(ITERATIONS 10000)
    else()
        set(ITERATIONS 1000)
    endif()
endif()

# count the main thread in
math(EXPR SMR_NUM_THREADS "${NUM_THREADS} + 2")

if(NOT DEFINED ALGOS)
    set(ALGOS STACK_XBO_BACKOFF STACK_ELIMINATION_BACKOFF)
endif()

set(TEST_DEFS
    VSTACK_TESTING IT=${ITERATIONS} VSTACK_XCHG_MAX_DURATION_MS=100
    VSTACK_XCHGER_COUNT=5 VSTACK_MAX_BACKOFF_MS=1000
    SMR_MAX_NTHREADS=${SMR_NUM_THREADS})

if(DEFINED LIBVSYNC_DISTRO_TESTING)
    set(ALGOS STACK_XBO_BACKOFF STACK_ELIMINATION_BACKOFF)
endif()

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_FILES})

    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    foreach(algo IN ITEMS ${ALGOS})
        set(TEST ${test_prefix}_${algo})

        if(${test_prefix} MATCHES ".*specific.*" AND (NOT ${algo} MATCHES
                                                      "HELPER"))
            set(SKIP on)
        else()
            set(SKIP off)
        endif()

        if(NOT ${SKIP})

            if(${algo} MATCHES "HELPER")
                set(TC_DEFS ${TEST_DEFS} ${algo} NTHREADS=16)
            else()
                set(TC_DEFS ${TEST_DEFS} ${algo} NTHREADS=${NUM_THREADS})
            endif()
            # make it lower-case
            string(TOLOWER ${TEST} TEST)

            # add the executable
            add_executable(${TEST} ${test_name})

            # link libs
            target_link_libraries(${TEST} vsync pthread)

            # activate target algo by adding the appropriate define
            target_compile_definitions(${TEST} PUBLIC ${TC_DEFS})

            # add it as a test
            v_add_bin_test_advanced(
                NAME
                ${TEST}
                COMMAND
                ${TEST}
                TIMEOUT
                3600
                PROCESSORS
                4)
        endif()
    endforeach()

endforeach()
