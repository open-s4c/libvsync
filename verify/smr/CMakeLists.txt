file(GLOB TEST_CASES test_case*.h)

# verification template file
set(VERIFY_FILE verify.c)

set(NUM_THREADS 3)
set(TEST_DEFS SMR_MAX_NTHREADS=3 CAP_TESTING VGDUMP_TESTING)
set(CHECK_DEFS -pthread -DCAP_TESTING -DSMR_MAX_NTHREADS=3)

set(MEMORY_MODELS "imm" "rc11")

set(DAT3M_BOUND_test_case_free_after_sync_smr_gdumpv1 1)
set(DAT3M_BOUND_test_case_sanity_smr_gdumpv1 1)

if(NOT DEFINED ALGOS)
    set(ALGOS SMR_EBR)
endif()

foreach(test_path IN ITEMS ${TEST_CASES})
    foreach(algo IN ITEMS ${ALGOS})

        if(${algo} MATCHES "SMR_CLEANUP")
            continue()
        endif()

        # name without extension
        get_filename_component(test_case ${test_path} NAME_WE)

        # The test includes whatever is given as TEST_CASE define TEST_CASE, we
        # need to pass it like this to be recognized
        set(tc TEST_CASE="${test_path}")

        # we have to escape it like this to work for check
        set(tc_check -DTEST_CASE="'\"${test_path}\"'")

        set(TEST_CASE ${test_case}_${algo})

        string(TOLOWER ${TEST_CASE} TEST_CASE)

        add_executable(${TEST_CASE} ${VERIFY_FILE})

        target_link_libraries(${TEST_CASE} pthread vsync)

        target_compile_definitions(${TEST_CASE} PUBLIC ${config})
        # define test case
        target_compile_definitions(${TEST_CASE} PUBLIC ${TEST_DEFS} ${tc}
                                                       ${algo})
        # add test
        v_add_bin_test(NAME ${TEST_CASE} COMMAND ${TEST_CASE})
        add_lotto_test(
            NAME
            ${TEST_CASE}
            SOURCE
            ${VERIFY_FILE}
            ROUNDS
            10
            CDEFS
            ${TEST_DEFS}
            ${tc}
            ${algo})

        foreach(WMM IN ITEMS ${MEMORY_MODELS})
            set(CHECK_NAME_PREFIX check_${TEST_CASE})
            set(CHECK_NAME ${CHECK_NAME_PREFIX}_${WMM})
            # add verification client
            add_vsyncer_check(
                TARGET ${CHECK_NAME_PREFIX}
                SOURCE ${VERIFY_FILE}
                CFLAGS ${CHECK_DEFS} ${tc_check} -D${algo}
                MEMORY_MODELS ${WMM}
                TIMEOUT 6000 #
                DEPENDENCIES vsync)
        endforeach()

        if(${DAT3M_BOUND_${TEST_CASE}})
            add_vsyncer_check(
                TARGET ${CHECK_NAME_PREFIX}
                SOURCE ${VERIFY_FILE}
                CFLAGS ${CHECK_DEFS} ${tc_check} -D${algo}
                TIMEOUT 1200 #
                MEMORY_MODELS
                    vmm USE_DAT3M DARTAGNAN_OPTIONS
                    --wmm.analysis.relationAnalysis=lazy
                    --bound=${DAT3M_BOUND_${TEST_CASE}}
                DEPENDENCIES vsync)
        endif()
    endforeach()
endforeach()

# Add checks for stand-alone testcases
file(GLOB STAND_ALONE_TCS *.c)
list(FILTER STAND_ALONE_TCS EXCLUDE REGEX "verify\.c")
foreach(test_path IN ITEMS ${STAND_ALONE_TCS})
    get_filename_component(test_case ${test_path} NAME_WE)
    set(CHECK_NAME check_${test_case})
    add_vsyncer_check(
        TARGET ${CHECK_NAME}
        SOURCE ${test_path}
        TIMEOUT 400 #
        DEPENDENCIES vsync)
endforeach()

if(EXISTS specific)
    add_subdirectory(specific)
endif()
