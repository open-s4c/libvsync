target_include_directories(
    vsync INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)
ProcessorCount(PCOUNT)

include_directories(include)

file(GLOB TEST_FILES *.c)

set(NTHREADS ${PCOUNT})

set(TEST_DEFS TST_IT=10000)

set(ALGOS simple)

foreach(test_path IN ITEMS ${TEST_FILES})
    foreach(algo IN ITEMS ${ALGOS})
        # extract test_name with extension
        get_filename_component(test_name ${test_path} NAME)

        # name without extension
        get_filename_component(test_case ${test_path} NAME_WE)

        set(TEST_CASE ${test_case}_${algo})

        string(TOLOWER ${TEST_CASE} TEST_CASE)

        add_executable(${TEST_CASE} ${test_name})

        target_link_libraries(${TEST_CASE} vsync pthread)

        # add defines
        target_compile_definitions(${TEST_CASE} PUBLIC ${TEST_DEFS} ${algo})

        v_add_bin_test_advanced(
            NAME
            ${TEST_CASE}
            COMMAND
            ${TEST_CASE}
            TIMEOUT
            3600
            PROCESSORS
            4)

    endforeach()
endforeach()
