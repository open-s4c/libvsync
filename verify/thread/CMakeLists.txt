file(GLOB SRCS *.c)

set(DAT3M_BOUND_mutex_tristate 3)
set(DAT3M_BOUND_once 5)
if(${VSYNCER_CHECK_FULL})
    set(DAT3M_BOUND_cond_test1 4)
    set(DAT3M_BOUND_cond_test2 6)
    set(DAT3M_BOUND_mutex_musl 5)
    set(DAT3M_BOUND_mutex_slim 4)
    set(DAT3M_BOUND_mutex_waiters 5)
else()
    set(DAT3M_BOUND_cond_test1 3)
    set(DAT3M_BOUND_cond_test2 3)
    set(DAT3M_BOUND_mutex_musl 3)
    set(DAT3M_BOUND_mutex_slim 2)
    set(DAT3M_BOUND_mutex_waiters 3)
endif()

foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    target_include_directories(${TEST} PRIVATE include)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    add_vsyncer_check(
        TARGET ${TEST}-check
        SOURCE ${SRC}
        TIMEOUT 20
        DEPENDENCIES vsync)

    if(${DAT3M_BOUND_${TEST}})
        add_vsyncer_check(
            TARGET ${TEST}-check
            SOURCE ${SRC}
            TIMEOUT 60
            MEMORY_MODELS vmm USE_DAT3M DARTAGNAN_OPTIONS
                          --bound=${DAT3M_BOUND_${TEST}}
            DEPENDENCIES vsync)
    endif()
endforeach()
