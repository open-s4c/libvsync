set(ALGOS LOCK_FREE_SKIPLIST)

# detect test files
file(GLOB TEST_FILES test*.c)

# find the number of processors
ProcessorCount(PCOUNT)

if(${LIBVSYNC_CROSS_TESTS})
    # QEMU might be too slow, we cannot really stress test
    set(NUM_THREADS 4)
    set(ITERATIONS 10)
elseif(LIBVSYNC_ADDRESS_SANITIZER)
    set(NUM_THREADS "${PCOUNT}")
    set(ITERATIONS 10000)
else()
    math(EXPR NUM_THREADS "${PCOUNT}*2")
    if(${VSYNCER_CHECK_FULL})
        set(ITERATIONS 1000000)
    else()
        set(ITERATIONS 100000)
    endif()
endif()

# count the main thread in
math(EXPR SMR_NUM_THREADS "${NUM_THREADS} + 1")

set(TEST_DEFS NTHREADS=${NUM_THREADS} IT=${ITERATIONS}
              SMR_MAX_NTHREADS=${SMR_NUM_THREADS})

set(ALGOS LOCK_FREE_SKIPLIST)
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

        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST} PUBLIC ${TEST_DEFS} ${algo})

        # add it as a test
        v_add_heavy_stress_test(
            NAME
            ${TEST}
            COMMAND
            ${TEST}
            TIMEOUT
            100
            STRESS_TIMEOUT
            3300)

    endforeach()

endforeach()
