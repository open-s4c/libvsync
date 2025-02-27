file(GLOB SRCS *.c)
foreach(SRC ${SRCS})

    get_filename_component(TEST ${SRC} NAME_WE)
    set(TEST test_${TEST})

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    target_compile_options(${TEST} PRIVATE -O3)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

endforeach()
