file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST}-test ${SRC})
    target_link_libraries(${TEST}-test vsync pthread)
    v_add_bin_test(NAME ${TEST}-test COMMAND ${TEST}-test)

    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${SRC}
        TIMEOUT 1200
        DEPENDENCIES vsync)
endforeach()

# Enable some tests with VMM and set extra options for Dartagnan
set(DAT3M_BOUND_bbq_spsc 2)

foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    if(${DAT3M_BOUND_${TEST}})
        add_vsyncer_check(
            TARGET check_${TEST}
            SOURCE ${SRC}
            MEMORY_MODELS vmm USE_DAT3M DARTAGNAN_OPTIONS
                          --bound=${DAT3M_BOUND_${TEST}}
            TIMEOUT 700
            DEPENDENCIES vsync)
    endif()
endforeach()
