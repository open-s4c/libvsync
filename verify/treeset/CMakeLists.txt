file(GLOB TEST_CASES testcase_*.h)

set(VERIFY_FILE verify.c)

set(NUM_THREADS 4)

set(ALGOS BST_FINE RB_FINE BST_COARSE RB_COARSE)
set(TEST_DEFS TREESET_LOCK_TTAS)
set(CHECK_FLAGS -DVSYNC_VERIFICATION -DVMEM_LIB_ALLOC_TRACKING_OFF)

foreach(test_path IN ITEMS ${TEST_CASES})
    get_filename_component(test_prefix ${test_path} NAME_WE)

    set(tc TEST_CASE="${test_path}")
    set(tc_check -DTEST_CASE="'\"${test_path}\"'")

    foreach(algo IN ITEMS ${ALGOS})
        set(TEST treeset_${test_prefix}_${algo})
        string(TOLOWER ${TEST} TEST)
        add_executable(${TEST} ${VERIFY_FILE})
        target_link_libraries(${TEST} vsync pthread)
        target_compile_definitions(${TEST} PUBLIC TREESET_${algo} ${TEST_DEFS}
                                                  ${tc})
        add_test(NAME ${TEST} COMMAND ${TEST})
        set_tests_properties(${TEST} PROPERTIES TIMEOUT 10)

        add_vsyncer_check(
            TARGET ${TEST}_check
            SOURCE ${VERIFY_FILE}
            CFLAGS -DTREESET_${algo} ${tc_check} ${CHECK_FLAGS}
            TIMEOUT 240
            DEPENDENCIES vsync)
    endforeach()

endforeach()
