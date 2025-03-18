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
math(EXPR SMR_NUM_THREADS "${NUM_THREADS} + 1")

set(TEST_DEFS VQUEUE_PRIO_TESTING IT=${ITERATIONS}
              SMR_MAX_NTHREADS=${SMR_NUM_THREADS})

set(ALGOS VQUEUE_PRIO_STACK_ARRAY_BASED VQUEUE_PRIO_STACK_TREE_BASED
          VQUEUE_PRIO_SKIPLIST_BASED VQUEUE_PRIO_HEAP_BASED)

# HEAP_BASED is too slow for oversubscription
set(VQUEUE_PRIO_HEAP_BASED_NTHREADS 10)

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_FILES})

    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    foreach(algo IN ITEMS ${ALGOS})

        # construct the test name
        set(TEST ${test_prefix}_${algo})
        # make it lower-case
        string(TOLOWER ${TEST} TEST)

        # add the executable
        add_executable(${TEST} ${test_name})

        # link libs
        target_link_libraries(${TEST} vsync pthread)

        if(DEFINED ${algo}_NTHREADS)
            set(NTHREADS ${${algo}_NTHREADS})
        else()
            set(NTHREADS ${NUM_THREADS})
        endif()
        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST} PUBLIC ${TEST_DEFS} ${algo}
                                                  NTHREADS=${NTHREADS})

        # add it as a test, with a longer timeout for stress tests
        v_add_heavy_stress_test(
            NAME
            ${TEST}
            COMMAND
            ${TEST}
            STRESS_TIMEOUT
            1000
            STRESS_PROCESSORS
            "4")

    endforeach()

endforeach()
