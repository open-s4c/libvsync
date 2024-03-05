file(GLOB SRCS *.c)
foreach(SRC ${SRCS})

    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    add_vsyncer_check(
        TARGET ${TEST}-check
        SOURCE ${SRC}
        TIMEOUT 500
        DEPENDENCIES vsync
        MEMORY_MODELS imm)
endforeach()

# Enable some tests with VMM and set extra options for Dartagnan
set(DAT3M_BOUND_bounded_spsc 4)
set(DAT3M_BOUND_bounded_mpmc_check_empty 5)
set(DAT3M_BOUND_bounded_mpmc_check_full 3)

foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    if(${DAT3M_BOUND_${TEST}})
        add_vsyncer_check(
            TARGET check_${TEST}
            SOURCE ${SRC}
            MEMORY_MODELS vmm USE_DAT3M DARTAGNAN_OPTIONS
                          --bound=${DAT3M_BOUND_${TEST}}
            TIMEOUT 500
            DEPENDENCIES vsync)
    endif()
endforeach()
