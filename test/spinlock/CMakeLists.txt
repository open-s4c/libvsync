# TODO: GenMC v0.9 wrongly reports a race in rc11 (it was correct in v0.8) The
# line below should be removed once that problem is fixed
# set(MEMORY_MODELS_hclhlock imm)

set(MEMORY_MODELS_rec_seqlock imm)
set(MEMORY_MODELS_seqcount imm)
set(MEMORY_MODELS_seqlock imm)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    add_executable(${TEST} ${SRC})
    target_link_libraries(${TEST} vsync pthread)
    target_include_directories(${TEST} PRIVATE include)
    v_add_bin_test(NAME ${TEST} COMMAND ${TEST})

    if(DEFINED MEMORY_MODELS_${TEST})
        set(MEMORY_MODELS "${MEMORY_MODELS_${TEST}}")
    else()
        set(MEMORY_MODELS "imm" "rc11")
    endif()

    if(${USE_DAT3M_${TEST}})
        set(USE_OPTION USE_DAT3M)
        set(CHECK_DEFS -DVSYNC_VERIFICATION_DAT3M)
    else()
        set(USE_OPTION)
        set(CHECK_DEFS -DVSYNC_VERIFICATION_GENMC)
    endif()

    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${SRC}
        MEMORY_MODELS ${MEMORY_MODELS}
        TIMEOUT 180 ${USE_OPTION}
        CFLAGS ${CHECK_DEFS}
        DEPENDENCIES vsync)
endforeach()
