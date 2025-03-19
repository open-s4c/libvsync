# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

# verification template file
set(VERIFY_FILE verify.c)

# verification limited by 4 threads max
set(NUM_THREADS 4)

set(TEST_DEFS VQUEUE_PRIO_TESTING VSKIPLIST_MAX_HEIGHT=2 VSTACK_TESTING
              SMR_MAX_NTHREADS=${NUM_THREADS} VQUEUE_PRIO_HEAP_CAPACITY=4)

set(CHECK_FLAGS
    -DVQUEUE_PRIO_TESTING -DVSTACK_TESTING -DVSKIPLIST_MAX_HEIGHT=2
    -DSKIPLIST_ONE_HEIGHT -DSMR_MAX_NTHREADS=${NUM_THREADS}
    -DVSTACK_XCHGER_COUNT=1)

set(MEMORY_MODELS "imm" "rc11")

set(ALGOS VQUEUE_PRIO_STACK_ARRAY_BASED VQUEUE_PRIO_STACK_TREE_BASED
          VQUEUE_PRIO_SKIPLIST_BASED VQUEUE_PRIO_HEAP_BASED)

if(${VSYNCER_CHECK_FULL})
    unset(SMR_FLAG)
else()
    set(SMR_FLAG -DSMR_NONE)
endif()

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_CASES})
    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    # The test includes whatever is given as TEST_CASE define TEST_CASE, we need
    # to pass it like this to be recognized
    set(tc TEST_CASE="${test_path}")

    # we have to escape it like this to work for check
    set(tc_check -DTEST_CASE="'\"${test_path}\"'")

    foreach(algo IN ITEMS ${ALGOS})

        # construct the test name
        set(TEST ${test_prefix}_${algo})

        # make it lower-case
        string(TOLOWER ${TEST} TEST)

        set(TEST_NAME ${TEST})

        # add the executable
        add_executable(${TEST_NAME} ${VERIFY_FILE})

        # link libs
        target_link_libraries(${TEST_NAME} vsync pthread)

        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST_NAME} PUBLIC ${algo} ${TEST_DEFS}
                                                       ${tc})

        # add it as a test
        v_add_bin_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

        # set timeout
        set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 10)

        foreach(WMM IN ITEMS ${MEMORY_MODELS})
            set(CHECK_NAME_PREFIX check_${TEST})
            set(CHECK_NAME ${CHECK_NAME_PREFIX}_${WMM})
            add_vsyncer_check(
                TARGET ${CHECK_NAME_PREFIX}
                SOURCE ${VERIFY_FILE}
                CFLAGS -D${algo} ${tc_check} ${CHECK_FLAGS} ${SMR_FLAG}
                MEMORY_MODELS ${WMM}
                TIMEOUT 600 #
                        ${CHECK_OPTIONS} #
                DEPENDENCIES vsync)
        endforeach()
    endforeach()

endforeach()
