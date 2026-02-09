# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

set(TEST_DEFS -D_GNU_SOURCE -DNTHREADS=3)

if(NOT DEFINED ALGOS)
    set(ALGOS MPSC)
endif()

# verification template file
set(VERIFY_FILE verify.c)

# disabled for now with dat3m set(DAT3M_BOUND_LEVEL_QUEUE 4)
set(DAT3M_BOUND_MPSC 4)

set(LEVEL_QUEUE_CONFIG -DLEVEL_QUEUE_BLOCK_NUM_ENTRIES=1
                       -DLEVEL_QUEUE_BLOCK_NUM_ENTRIES=2)

set(MPSC_CONFIG -DDEFAULT)

set(CHECK_TIMEOUT 3600)

foreach(test_path IN ITEMS ${TEST_CASES})
    # name without extension
    get_filename_component(test_case ${test_path} NAME_WE)

    # define TEST_CASE, we need to pass it like this to be recognized
    set(tc TEST_CASE="${test_path}")
    # we have to escape it like this to work for check
    set(tc_check -DTEST_CASE="'\"${test_path}\"'" -DVMEM_LIB_ALLOC_TRACKING_OFF)

    foreach(algo IN ITEMS ${ALGOS})

        set(TEST_PREFIX ${test_case}_${algo})
        string(TOLOWER ${TEST_PREFIX} TEST_PREFIX)

        set(CONFIG_IDX 0)
        foreach(CONFIG IN ITEMS ${${algo}_CONFIG})
            math(EXPR CONFIG_IDX "${CONFIG_IDX}+1")
            set(TEST ${TEST_PREFIX}_${CONFIG_IDX})
            # add the executable
            add_executable(${TEST} ${VERIFY_FILE})
            # link libs
            target_link_libraries(${TEST} vsync pthread)
            # activate target algo by adding the appropriate define
            target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS}
                                                      ${tc} ${CONFIG})

            # add it as a test
            v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

            add_vsyncer_check(
                TARGET check_${TEST}
                SOURCE ${VERIFY_FILE}
                CFLAGS -D${algo} ${TEST_DEFS} ${tc_check} ${CONFIG}
                DEPENDENCIES vsync
                TIMEOUT ${CHECK_TIMEOUT} #
            )

            if(${DAT3M_BOUND_${algo}})
                add_vsyncer_check(
                    TARGET check_${TEST}
                    SOURCE ${VERIFY_FILE}
                    CFLAGS -D${algo} ${TEST_DEFS} ${tc_check} ${CONFIG}
                           USE_DAT3M
                    MEMORY_MODELS vmm DARTAGNAN_OPTIONS
                                  --bound=${DAT3M_BOUND_${algo}}
                    DEPENDENCIES vsync
                    TIMEOUT ${CHECK_TIMEOUT} #
                            ${CHECK_OPTIONS})
            endif()

        endforeach(CONFIG)
    endforeach(algo)
endforeach(test_path)
