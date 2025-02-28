ProcessorCount(PCOUNT)

file(GLOB TEST_FILES test_*.c)

# For Code coverage mode use only 4 threads, otherwise PCOUNT.
set(NUM_THREADS $<IF:$<CONFIG:Coverage>,4,${PCOUNT}>)

list(APPEND TEST_DEFS NTHREADS=${NUM_THREADS})

foreach(test_path IN ITEMS ${TEST_FILES})
    get_filename_component(test_name ${test_path} NAME)
    get_filename_component(test_prefix ${test_path} NAME_WE)
    set(TEST ${test_prefix})
    string(TOLOWER ${TEST} TEST)
    add_executable(${TEST} ${test_name})
    target_link_libraries(${TEST} vsync pthread)
    target_compile_definitions(${TEST} PUBLIC ${TEST_DEFS})
    v_add_heavy_stress_test(
        NAME
        ${TEST}
        COMMAND
        ${TEST}
        STRESS_TIMEOUT
        4200
        TIMEOUT
        3600)
endforeach()
