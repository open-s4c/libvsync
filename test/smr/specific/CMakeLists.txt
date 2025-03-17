file(GLOB TEST_FILES *.c)

set(TEST_DEFS SMR_MAX_NTHREADS=${NTHREADS})

foreach(F IN ITEMS ${TEST_FILES})
    get_filename_component(TEST_FILE ${F} NAME)
    get_filename_component(TEST_CASE ${F} NAME_WE)

    add_executable(${TEST_CASE} ${TEST_FILE})
    target_compile_definitions(${TEST_CASE} PRIVATE ${TEST_DEFS})
    target_link_libraries(${TEST_CASE} vsync pthread)
    v_add_bin_test(NAME ${TEST_CASE} COMMAND ${TEST_CASE})

endforeach()

add_subdirectory(gdump)
