file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    target_compile_definitions(${TEST} PUBLIC _GNU_SOURCE)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    add_vsyncer_check(
        TARGET ${TEST}-check
        SOURCE ${SRC}
        DEPENDENCIES vsync)
endforeach()
