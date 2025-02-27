file(GLOB SRCS *.c)

foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${SRC}
        TIMEOUT 300
        DEPENDENCIES vsync)
endforeach()

# Enable some tests with VMM and set extra options for Dartagnan
set(DAT3M_BOUND_verify_basic_variation 5)
set(DAT3M_BOUND_verify_basic 5)
set(DAT3M_BOUND_verify_mutual_exclusion 5)
set(DAT3M_BOUND_verify_empty 10)

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
