set(PREFIX "unbounded_queue")

file(GLOB TEST_FILES ${PREFIX}*.c)

# find the number of processors
ProcessorCount(PCOUNT)

if(${LIBVSYNC_CROSS_TESTS})
    # QEMU might be too slow, we cannot really stress test
    set(NUM_THREADS 4)
    set(ITERATIONS 10)
else()
    math(EXPR NUM_THREADS "${PCOUNT}*20")
    set(ITERATIONS 100)
endif()

math(EXPR MAX_THREADS "${NUM_THREADS}+3")

set(TEST_DEFS -D_GNU_SOURCE NTHREADS=${NUM_THREADS} IT=${ITERATIONS}
              -DSMR_MAX_NTHREADS=${MAX_THREADS})

if(NOT DEFINED ALGOS)
    set(ALGOS VQUEUE_UB_LF_RECYCLE VQUEUE_UB_TOTAL VQUEUE_UB_LF)
endif()

foreach(test_path IN ITEMS ${TEST_FILES})
    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_case ${test_path} NAME_WE)

    foreach(algo IN ITEMS ${ALGOS})

        set(TEST test_${test_case}_${algo})
        string(TOLOWER ${TEST} TEST)

        # silk test is not relevant to other implementations
        if(("${TEST}" MATCHES ".*silk.*") AND (NOT "${algo}" STREQUAL
                                               "VQUEUE_UB_LF"))
            continue()
        endif()

        add_executable(${TEST} ${test_name})
        target_link_libraries(${TEST} vsync pthread)
        target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS})

        v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

        # Don't run this test concurrently with others
        set_tests_properties(${TEST} PROPERTIES RUN_SERIAL 1)

        # exclude adding stress tests on VSYNCER_CHECK_FULL
        if((NOT ${VSYNCER_CHECK_FULL}) AND (NOT ("${TEST}" MATCHES ".*silk.*")))
            set_tests_properties(${TEST} PROPERTIES TIMEOUT 300 COST 60)
        elseif(${VSYNCER_CHECK_FULL})
            set_tests_properties(${TEST} PROPERTIES TIMEOUT 3600)
        endif()

    endforeach()

endforeach()
