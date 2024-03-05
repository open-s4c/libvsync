# detect test files
file(GLOB TEST_FILES test*.c)

# find the number of processors
ProcessorCount(PCOUNT)

if(${LIBVSYNC_CROSS_TESTS})
    # QEMU might be too slow, we cannot really stress test
    set(NUM_THREADS 4)
    set(ITERATIONS 10)
else()
    set(NUM_THREADS ${PCOUNT})
    set(ITERATIONS 10000)
endif()

set(TEST_DEFS NTHREADS=${NUM_THREADS} IT=${ITERATIONS}
              SMR_MAX_NTHREADS=${NUM_THREADS})

if(NOT DEFINED ALGOS)
    set(ALGOS HASHTABLE_STANDARD)
endif()

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_FILES})

    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    if(${test_prefix} MATCHES ".*specific.*")
        # available to a sepcific case
        set(IMPLS "single")
    else()
        # available to all algos
        set(IMPLS ${ALGOS})
    endif()

    foreach(algo IN ITEMS ${IMPLS})

        # construct the test name
        set(TEST ${test_prefix}_${algo})
        # make it lower-case
        string(TOLOWER ${TEST} TEST)

        # add the executable
        add_executable(${TEST} ${test_name})

        # link libs
        target_link_libraries(${TEST} vsync pthread)

        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS})

        # add it as a test, with a longer timeout for stress tests
        v_add_heavy_stress_test(NAME ${TEST} COMMAND ${TEST} STRESS_TIMEOUT
                                3600)

    endforeach()

endforeach()
