set(FILE_NAME cachedq_verify.c)

set(TEST_NAME test_cachedq)
add_executable(${TEST_NAME} ${FILE_NAME})
target_link_libraries(${TEST_NAME} vsync pthread)
v_add_bin_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

add_vsyncer_check(
    TARGET check_cachedq
    SOURCE ${FILE_NAME}
    TIMEOUT 30
    DEPENDENCIES vsync
    MEMORY_MODELS imm rc11)

set(DAT3M_BOUND_test_cachedq 1)

if(${DAT3M_BOUND_${TEST_NAME}})
    add_vsyncer_check(
        TARGET check_cachedq
        SOURCE ${FILE_NAME}
        TIMEOUT 180
        MEMORY_MODELS vmm USE_DAT3M DARTAGNAN_OPTIONS
                      --bound=${DAT3M_BOUND_${TEST_NAME}}
        DEPENDENCIES vsync)
endif()
