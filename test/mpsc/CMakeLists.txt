target_include_directories(
    vsync INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)

file(GLOB TEST_FILES *.c)

# find the number of processors
ProcessorCount(PCOUNT)

if(${LIBVSYNC_CROSS_TESTS})
    # QEMU might be too slow, we cannot really stress test
    set(NUM_THREADS 4)
    set(ITERATIONS 10)
else()
    math(EXPR NUM_THREADS "${PCOUNT}*2")
    set(ITERATIONS 10000)
endif()

set(TEST_DEFS -D_GNU_SOURCE NTHREADS=${NUM_THREADS} IT=${ITERATIONS}
              DBG_ENABLE_COL_ALL VSYNC_ENABLE_DEBUG)

if(NOT DEFINED ALGOS)
    set(ALGOS MPSC)
endif()

foreach(test_path IN ITEMS ${TEST_FILES})
    # extract test_name with extension
    get_filename_component(test_name ${test_path} NAME)

    # name without extension
    get_filename_component(test_case ${test_path} NAME_WE)

    foreach(algo IN ITEMS ${ALGOS})

        set(TEST test_${test_case}_${algo})
        string(TOLOWER ${TEST} TEST)
        add_executable(${TEST} ${test_name})
        target_link_libraries(${TEST} vsync pthread)
        target_compile_definitions(${TEST} PUBLIC ${algo} ${TEST_DEFS})

        v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

        set_tests_properties(${TEST} PROPERTIES TIMEOUT 300 COST 60)
    endforeach()
endforeach()
