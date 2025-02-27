# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

set(VERIFY_FILE verify.c)

set(MEMORY_MODELS "imm" "rc11")

set(ALGOS SIMPLEHT)

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

            add_vsyncer_check(
                TARGET ${CHECK_NAME_PREFIX}
                SOURCE ${VERIFY_FILE}
                CFLAGS -D${algo} ${tc_check}
                TIMEOUT 1200
                MEMORY_MODELS ${WMM} #
                DEPENDENCIES vsync)
        endforeach()
    endforeach()
endforeach()
