function(add_vsyncer_check)
    # skip if VSYNCER_CHECK is not defined
    if(NOT VSYNCER_CHECK)
        return()
    endif()
    ###########################################################################
    # Parse Params
    ###########################################################################
    set(opts USE_DAT3M USE_GENMC10)
    set(ones TARGET SOURCE TIMEOUT GENMC10_EXTRA_OPTIONS DARTAGNAN_OPTIONS)
    set(mult CFLAGS DEPENDENCIES MEMORY_MODELS)
    cmake_parse_arguments(VSYNCER_CHECK "${opts}" "${ones}" "${mult}" ${ARGN})
    ###########################################################################
    # Define Vars
    ###########################################################################
    set(TARGET ${VSYNCER_CHECK_TARGET})
    set(LOG_FILE "${CMAKE_BINARY_DIR}/vsyncer_log.csv")
    ###########################################################################
    # Set Client
    ###########################################################################
    if(EXISTS ${VSYNCER_CHECK_SOURCE})
        set(CLIENT ${VSYNCER_CHECK_SOURCE})
    else()
        set(CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/${VSYNCER_CHECK_SOURCE})
    endif()
    ###########################################################################
    # Set timeout
    ###########################################################################
    if(${VSYNCER_CHECK_TIMEOUT})
        set(TIMEOUT ${VSYNCER_CHECK_TIMEOUT})
    else()
        set(TIMEOUT 60)
    endif()
    ###########################################################################
    # Set CFLAGS
    ###########################################################################
    set(CFLAGS ${VSYNCER_CHECK_CFLAGS})
    list(APPEND CFLAGS #
        -I${PROJECT_SOURCE_DIR}/include #
        -DVSYNC_VERIFICATION #
        -DVSYNC_VERIFICATION_QUICK
        -DVSYNC_SMR_NOT_AVAILABLE
    )
    ###########################################################################
    # Define mode checker env vars
    ###########################################################################
    if(${VSYNCER_CHECK_USE_DAT3M})
        set(CHECKER dartagnan)
        set(CHECKER_ENV DARTAGNAN_OPTIONS=${VSYNCER_CHECK_DARTAGNAN_OPTIONS})
        list(APPEND CFLAGS -DVSYNC_DISABLE_SPIN_ANNOTATION
            -DVSYNC_VERIFICATION_DAT3M)
    else()
        set(CHECKER genmc)
        list(APPEND CFLAGS -DVSYNC_VERIFICATION_GENMC)
        if(${VSYNCER_CHECK_USE_GENMC10})
            set(GENMC10_OPTIONS #
                -disable-estimation #
                -check-liveness #
                -disable-spin-assume #
                ${VSYNCER_CHECK_GENMC10_EXTRA_OPTIONS}
            )
            set(CHECKER_ENV #
                GENMC_SET_OPTIONS=${GENMC10_OPTIONS} #
                GENMC_CMD=/usr/share/genmc10/bin/genmc #
            )
        endif()
    endif()
    ###########################################################################
    # Define memory models
    ###########################################################################
    if(DEFINED VSYNCER_CHECK_MEMORY_MODELS)
        set(WMMS ${VSYNCER_CHECK_MEMORY_MODELS})
    else()
        set(WMMS "imm" "rc11")
    endif()
    ###########################################################################
    # Add vsyncer check of the client as a ctest
    ###########################################################################
    foreach(WMM IN ITEMS ${WMMS})
        set(TEST_NAME ${TARGET}_${WMM})
        string(TOUPPER ${WMM} WMM_UP)
        list(APPEND CFLAGS -DVSYNC_VERIFICATION_${WMM_UP})
        string(REPLACE ";" " " CFLAGS "${CFLAGS}")
        string(REPLACE "\"'\"" "\"" CFLAGS "${CFLAGS}")
        set(VSYNCER_CMD #
            env CFLAGS=${CFLAGS} #
            ${CHECKER_ENV} #
            vsyncer check -d #
            --csv-log ${LOG_FILE} #
            --checker ${CHECKER} #
            --memory-model ${WMM} #
            --timeout ${TIMEOUT}s
        )
        add_test(NAME ${TEST_NAME} COMMAND ${VSYNCER_CMD} ${CLIENT})
    endforeach()

endfunction()
