set(MEMORY_MODELS_rec_seqlock imm)
set(MEMORY_MODELS_seqcount imm)
set(MEMORY_MODELS_seqlock imm)

file(GLOB SRCS *.c)

set(VERIFY_WITH_GENMC10 #
    hclhlock #
)

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

    if("${TEST}" IN_LIST VERIFY_WITH_GENMC10)
        set(CHECK_OPTIONS USE_GENMC10)
    else()
        unset(CHECK_OPTIONS)
    endif()

    add_vsyncer_check(
        TARGET check_${TEST}
        SOURCE ${SRC}
        MEMORY_MODELS ${MEMORY_MODELS}
        TIMEOUT 180 #
                ${CHECK_OPTIONS} #
        DEPENDENCIES vsync)
endforeach()

# correlate the bound for ararylock with the length of the array
set(ARRAY_LOCK_LEN 4)
math(EXPR ARRAY_LOCK_BOUND "${ARRAY_LOCK_LEN} + 1")
set(CFLAGS_arraylock -DARRAY_LOCK_LEN=${ARRAY_LOCK_LEN})
# Enable some tests with VMM and set extra options for Dartagnan
set(DAT3M_BOUND_arraylock ${ARRAY_LOCK_BOUND})
set(DAT3M_BOUND_caslock 4)
set(DAT3M_BOUND_clhlock 4)
if(${VSYNC_VERIFICATION_QUICK})
    set(DAT3M_BOUND_cnalock 5)
else()
    set(DAT3M_BOUND_cnalock 6)
endif()
set(DAT3M_BOUND_hclhlock_2t 4)
set(DAT3M_BOUND_hemlock 5)
set(DAT3M_BOUND_mcslock 4)
set(DAT3M_BOUND_rec_seqlock 5)
set(DAT3M_BOUND_rec_spinlock 4)
set(DAT3M_BOUND_rec_ticketlock 4)
set(DAT3M_BOUND_rwlock 5)
set(DAT3M_BOUND_semaphore 5)
set(DAT3M_BOUND_seqcount 4)
set(DAT3M_BOUND_seqlock 5)
set(DAT3M_BOUND_ticketlock 4)
set(DAT3M_BOUND_ttaslock 4)

foreach(SRC ${SRCS})
    get_filename_component(TEST ${SRC} NAME_WE)

    if(${DAT3M_BOUND_${TEST}})
        add_vsyncer_check(
            TARGET check_${TEST}
            SOURCE ${SRC}
            MEMORY_MODELS
                vmm #
                USE_DAT3M #
                DARTAGNAN_OPTIONS --bound=${DAT3M_BOUND_${TEST}}
            TIMEOUT 600
            CFLAGS ${CFLAGS_${TEST}}
            DEPENDENCIES vsync)
    endif()
endforeach()
