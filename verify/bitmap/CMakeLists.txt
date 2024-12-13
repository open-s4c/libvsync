# detect test cases header files
file(GLOB TEST_CASES test_case*.h)

# verification template file
set(VERIFY_FILE verify_bitmap.c)

set(ALGO bitmap)

set(DAT3M_BOUND_test_case_clr_interset_bitmap 4)
set(DAT3M_BOUND_test_case_intact_bitmap 4)
set(DAT3M_BOUND_test_case_range_bitmap 4)
set(DAT3M_BOUND_test_case_set_intersect_bitmap 4)

foreach(TC IN ITEMS ${TEST_CASES})

    get_filename_component(TC_NAME ${TC} NAME)
    get_filename_component(TC_NAME_WE ${TC} NAME_WE)
    set(TEST_NAME ${TC_NAME_WE}_${ALGO})

    # add executable test
    set(TC_DEF TEST_CASE="${TC_NAME}")
    add_executable(${TEST_NAME} ${VERIFY_FILE})
    target_link_libraries(${TEST_NAME} vsync pthread)
    target_compile_definitions(${TEST_NAME} PUBLIC ${TC_DEF})
    v_add_bin_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # add verification client
    set(TC_CHECK_DEF -DTEST_CASE="'\"${TC_NAME}\"'")
    add_vsyncer_check(
        TARGET check_${TEST_NAME}
        CFLAGS ${TC_CHECK_DEF}
        SOURCE ${VERIFY_FILE}
        TIMEOUT 120
        DEPENDENCIES vsync)

    # add verification client for vmm
    if(${DAT3M_BOUND_${TEST_NAME}})
        add_vsyncer_check(
            TARGET check_${TEST_NAME}
            CFLAGS ${TC_CHECK_DEF}
            SOURCE ${VERIFY_FILE}
            TIMEOUT 300 USE_DAT3M
            MEMORY_MODELS vmm USE_DAT3M DARTAGNAN_OPTIONS
                          --bound=${DAT3M_BOUND_${TEST_NAME}}
            DEPENDENCIES vsync)
    endif()

endforeach()
