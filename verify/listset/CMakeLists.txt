# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

# verification template file
set(VERIFY_FILE verify_listset.c)

set(NUM_THREADS 4)

set(TEST_DEFS SMR_MAX_NTHREADS=${NUM_THREADS})

set(CHECK_FLAGS -DVSYNC_VERIFICATION -DSMR_MAX_NTHREADS=${NUM_THREADS})

if(NOT DEFINED ALGOS)
    set(ALGOS LISTSET_COARSE LISTSET_FINE LISTSET_LAZY LISTSET_OPT LISTSET_LF)
endif()

set(VERIFY_WITH_GENMC10
    # check_test_case_2_listset_opt_rc11 # Requires investigation (keep skipped
    # for now!) either a bug in genmc or when the node is retired!
    check_test_case_2_listset_lazy_imm #
    check_test_case_2_listset_opt_imm #
    check_test_case_2_listset_lf_imm #
    check_test_case_3_listset_opt_imm #
    check_test_case_3_listset_opt_rc11 #
    check_test_case_4_listset_opt_imm #
    check_test_case_4_listset_lazy_imm #
    check_test_case_8_listset_coarse_imm #
    check_test_case_8_listset_fine_imm #
    check_test_case_8_listset_lazy_imm #
    check_test_case_8_listset_opt_imm #
    check_test_case_7_listset_coarse_imm #
    check_test_case_7_listset_fine_imm #
    check_test_case_7_listset_lazy_imm #
    check_test_case_7_listset_opt_imm #
)

set(VERIFIY_WITHOUT_SMR_ON_QUICK #
    check_test_case_2_listset_coarse_imm #
    check_test_case_2_listset_fine_imm #
    check_test_case_2_listset_lazy_imm #
    check_test_case_2_listset_opt_imm #
    check_test_case_2_listset_lf_imm #
    check_test_case_4_listset_coarse_imm #
    check_test_case_4_listset_lf_imm #
    check_test_case_4_listset_fine_imm #
    check_test_case_4_listset_opt_imm #
    check_test_case_4_listset_lazy_imm #
    check_test_case_7_listset_coarse_imm #
    check_test_case_7_listset_fine_imm #
    check_test_case_7_listset_lazy_imm #
    check_test_case_7_listset_opt_imm #
    check_test_case_7_listset_lf_imm #
)

set(MEMORY_MODELS "imm" "rc11")

# for all files that start with test
foreach(test_path IN ITEMS ${TEST_CASES})

    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_prefix ${test_path} NAME_WE)

    # define TEST_CASE, we need to pass it like this to be recognized
    set(tc TEST_CASE="${test_name}")
    # we have to escape it like this to work for check
    set(tc_check -DTEST_CASE="'\"${test_name}\"'")

    foreach(algo IN ITEMS ${ALGOS})

        # construct the test name
        set(TEST ${test_prefix}_${algo})

        # make it lower-case
        string(TOLOWER ${TEST} TEST)

        # add the executable
        add_executable(${TEST} ${VERIFY_FILE})

        # link libs
        target_link_libraries(${TEST} vsync pthread)

        # activate target algo by adding the appropriate define
        target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS} ${tc})

        # add it as a test
        v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

        # set timeout
        set_tests_properties(${TEST} PROPERTIES TIMEOUT 10)

        foreach(WMM IN ITEMS ${MEMORY_MODELS})
            set(CHECK_NAME_PREFIX check_${TEST})
            set(CHECK_NAME ${CHECK_NAME_PREFIX}_${WMM})

            if(${CHECK_NAME} IN_LIST VERIFY_WITH_GENMC10)
                set(CHECK_OPTIONS USE_GENMC10)
            else()
                unset(CHECK_OPTIONS)
            endif()

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
                TIMEOUT 600
                MEMORY_MODELS ${WMM} #
                              ${CHECK_OPTIONS} #
                DEPENDENCIES vsync)
        endforeach()
    endforeach()
endforeach()
