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
