# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

# verification template file
set(VERIFY_FILE verify_stack.c)

# verification limited by 4 threads max
set(NUM_THREADS 4)

# add -DDBG_ALL to enable debug messages

set(TEST_DEFS VSTACK_TESTING SMR_MAX_NTHREADS=${NUM_THREADS}
              -DVSTACK_XCHGER_COUNT=1 -DVSTACK_XCHG_MAX_DURATION_MS=100)

set(CHECK_FLAGS -DVSYNC_VERIFICATION -DVSTACK_TESTING -DSMR_THRESHOLD=1
                -DSMR_MAX_NTHREADS=${NUM_THREADS} -DVSTACK_XCHGER_COUNT=1)

set(VERIFIY_WITHOUT_SMR_ON_QUICK
    check_test_case_lifo_stack_xbo_backoff_imm
    check_test_case_lifo_stack_elimination_backoff_imm
    check_test_case_2push_2pop_stack_xbo_backoff_imm
    check_test_case_2push_2pop_stack_elimination_backoff_imm
    check_test_case_only_pop_stack_xbo_backoff_imm
    check_test_case_only_pop_stack_elimination_backoff_imm)

set(MEMORY_MODELS "imm" "rc11")

if(NOT DEFINED ALGOS)
    set(ALGOS STACK_XBO_BACKOFF STACK_ELIMINATION_BACKOFF)
endif()

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_CASES})

    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    # The test includes whatever is given as TEST_CASE define TEST_CASE, we need
    # to pass it like this to be recognized
    set(tc TEST_CASE="${test_name}")

    # we have to escape it like this to work for check
    set(tc_check -DTEST_CASE="'\"${test_name}\"'")

    foreach(algo IN ITEMS ${ALGOS})

        if("${test_name}" MATCHES ".*specific.*")
            if(NOT ${algo} MATCHES "HELPER")
                continue()
            endif()
        endif()
        # construct the test name
        set(TEST ${test_prefix}_${algo})

        # make it lower-case
        string(TOLOWER ${TEST} TEST)

        set(TEST_NAME test_${TEST})

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
            if((NOT ${VSYNCER_CHECK_FULL}) AND ("${CHECK_NAME}" IN_LIST
                                                VERIFIY_WITHOUT_SMR_ON_QUICK))
                set(SMR_FLAG -DSMR_NONE)
            else()
                unset(SMR_FLAG)
            endif()
            add_vsyncer_check(
                TARGET ${CHECK_NAME_PREFIX}
                SOURCE ${VERIFY_FILE}
                CFLAGS -D${algo} ${tc_check} ${CHECK_FLAGS} ${SMR_FLAG}
                MEMORY_MODELS ${WMM}
                TIMEOUT 600 #
                DEPENDENCIES vsync)
        endforeach()
    endforeach()
endforeach()
