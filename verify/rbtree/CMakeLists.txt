file(GLOB TEST_CASES testcase_*.h)

set(VERIFY_FILE verify_rbtree.c)

set(NUM_THREADS 4)

set(TEST_DEFS SMR_MAX_NTHREADS=${NUM_THREADS} RBTREE_STATIC_MALLOC)

set(CHECK_FLAGS -DVSYNC_VERIFICATION -DSMR_MAX_NTHREADS=${NUM_THREADS}
                -DRBTREE_STATIC_MALLOC)

if(${VSYNCER_CHECK_FULL})
    unset(SMR_FLAG)
else()
    set(SMR_FLAG -DSMR_NONE)
endif()

foreach(test_path IN ITEMS ${TEST_CASES})
    get_filename_component(test_name ${test_path} NAME)
    get_filename_component(test_prefix ${test_path} NAME_WE)

    set(tc TEST_CASE="${test_name}")
    set(tc_check -DTEST_CASE="'\"${test_name}\"'")
    set(TEST rbtree_${test_prefix})
    string(TOLOWER ${TEST} TEST)
    add_executable(${TEST} ${VERIFY_FILE})
    target_link_libraries(${TEST} vsync pthread)
    target_compile_definitions(${TEST} PUBLIC ${TEST_DEFS} ${tc})
    add_test(NAME ${TEST} COMMAND ${TEST})
    set_tests_properties(${TEST} PROPERTIES TIMEOUT 10)
    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${VERIFY_FILE}
        CFLAGS ${tc_check} ${CHECK_FLAGS} ${SMR_FLAG}
        TIMEOUT 600
        MEMORY_MODELS imm
        DEPENDENCIES vsync)
endforeach()
