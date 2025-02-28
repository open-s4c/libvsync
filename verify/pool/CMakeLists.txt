file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${SRC}
        TIMEOUT 500
        DEPENDENCIES vsync)
endforeach()
