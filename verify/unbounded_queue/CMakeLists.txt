set(PREFIX "unbounded_queue")

set(TEST_DEFS -D_GNU_SOURCE -DSMR_MAX_NTHREADS=4)

set(VERIFY_WITH_GENMC10
    "vqueue_ub_total_test_case_1" #
    "vqueue_ub_total_test_case_3" #
    "vqueue_ub_total_test_case_4" #
    "vqueue_ub_total_test_case_6" #
)

# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

if(NOT DEFINED ALGOS)
    set(ALGOS VQUEUE_UB_LF_RECYCLE VQUEUE_UB_TOTAL VQUEUE_UB_LF)
endif()

# verification template file
set(VERIFY_FILE unbounded_queue_verify.c)

foreach(test_path IN ITEMS ${TEST_CASES})
    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_case ${test_path} NAME_WE)

    # define TEST_CASE, we need to pass it like this to be recognized
    set(tc TEST_CASE="${test_name}")
    # we have to escape it like this to work for check
    set(tc_check -DTEST_CASE="'\"${test_name}\"'")

    foreach(algo IN ITEMS ${ALGOS})

        set(TEST ${algo}_${test_case})
        string(TOLOWER ${TEST} TEST)

        # add the executable
        add_executable(${TEST} ${VERIFY_FILE})
        # link libs
        target_link_libraries(${TEST} vsync pthread)
        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS} ${tc})

        # add it as a test
        v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

        if(${TEST} IN_LIST VERIFY_WITH_GENMC10)
            set(CHECK_OPTIONS USE_GENMC10)
        else()
            unset(CHECK_OPTIONS)
        endif()

        add_vsyncer_check(
            TARGET check_${TEST}
            SOURCE ${VERIFY_FILE}
            CFLAGS -D${algo} ${TEST_DEFS} ${tc_check}
            DEPENDENCIES vsync
            TIMEOUT 1200 #
                    ${CHECK_OPTIONS})

    endforeach()

endforeach()
