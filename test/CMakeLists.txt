include(CTest)
include(ProcessorCount)
include(${CMAKE_SOURCE_DIR}/cmake/v_add_test.cmake)

set(VSYNCER_CHECK_MIN_TIMEOUT
    10
    CACHE STRING "minimum vsyncer check timeout (in seconds)")

# `add_vsyncer_check` is used for verification. It is currently unavailable,
# this is a place holder function to avoid errors
function(add_vsyncer_check)
    # parse arguments
    set(opts USE_DAT3M USE_GENMC10)
    set(ones TARGET SOURCE TIMEOUT GENMC10_EXTRA_OPTIONS DARTAGNAN_OPTIONS)
    set(mult CFLAGS DEPENDENCIES MEMORY_MODELS)
    cmake_parse_arguments(VSYNCER_CHECK "${opts}" "${ones}" "${mult}" ${ARGN})

    set(VSYNCER_LOG_DIR "${CMAKE_BINARY_DIR}/logs")
    set(VSYNCER_CSV_LOGFILE "${VSYNCER_LOG_DIR}/vsyncer_log.csv")
    if(NOT EXISTS "${VSYNCER_LOG_DIR}")
        file(MAKE_DIRECTORY "${VSYNCER_LOG_DIR}")
    endif()

    # timeout
    if(NOT DEFINED VSYNCER_CHECK_TIMEOUT)
        set(VSYNCER_CHECK_TIMEOUT ${VSYNCER_CHECK_MIN_TIMEOUT})
    elseif(${VSYNCER_CHECK_MIN_TIMEOUT} GREATER ${VSYNCER_CHECK_TIMEOUT})
        set(VSYNCER_CHECK_TIMEOUT ${VSYNCER_CHECK_MIN_TIMEOUT})
    endif()

    # CFLAGS
    list(APPEND CFLAGS -I${PROJECT_SOURCE_DIR}/include)

    # determine for which WMMs the check is added
    if(NOT DEFINED VSYNCER_CHECK_MEMORY_MODELS)
        set(WMMS "imm" "rc11")
    else()
        set(WMMS ${VSYNCER_CHECK_MEMORY_MODELS})
    endif()


    # for each WMM there will be an ll, and a ctest if VSYNC_CHECK is on.
    foreach(WMM IN ITEMS ${WMMS})

        string(TOUPPER ${WMM} WMM_UP)

        set(CFLAGS_WMM ${CFLAGS} -DVSYNC_VERIFICATION -DVSYNC_VERIFICATION_${WMM_UP})

        if (${VSYNCER_CHECK_USE_DAT3M})
            list(APPEND CFLAGS_WMM -DVSYNC_DISABLE_SPIN_ANNOTATION -DVSYNC_VERIFICATION_DAT3M)
        endif()

        # if VSYNCER_CHECK_FULL option is set to on, i.e. -DVSYNCER_CHECK_FULL=on
        if(VSYNCER_CHECK_FULL)
            # do nothing
        else()
            # add quick
            list(APPEND CFLAGS_WMM -DVSYNC_VERIFICATION_QUICK)
        endif()


        # LLVM-IR target and source path
        set(VSYNCER_CHECK_LL ${CMAKE_CURRENT_BINARY_DIR}/${VSYNCER_CHECK_TARGET}_${WMM}.ll)
        if(EXISTS ${VSYNCER_CHECK_SOURCE})
            set(VSYNCER_CHECK_SRC ${VSYNCER_CHECK_SOURCE})
        else()
            set(VSYNCER_CHECK_SRC
                ${CMAKE_CURRENT_SOURCE_DIR}/${VSYNCER_CHECK_SOURCE})
        endif()

        if(${VSYNCER_CHECK_USE_GENMC10})
            set(GENMC10_PATH "/usr/share/genmc10")
            set(GENM10_ENV "VSYNCER_GENMC_INCLUDE_PATH=${GENMC10_PATH}/include/genmc/include/")
        endif()

        set(CMD ${TOOLKIT_CMD}
            env
            CFLAGS="${CFLAGS_WMM}"
            ${GENM10_ENV}
            vsyncer compile -d
                -o=${VSYNCER_CHECK_LL} ${VSYNCER_CHECK_SRC})

        if (${VSYNCER_CHECK_USE_DAT3M})
            set(CMD ${CMD} -c dartagnan)
        endif()

        if(NOT ${VSYNCER_COMPILE_OUTPUT})
            # redirect the compile output to log. If compilation failed
            # spit out the log to console and stop
            set(CMD
               ${CMD} > ${VSYNCER_CHECK_LL}.log #
               || (cat ${VSYNCER_CHECK_LL}.log && false)
               )

        endif()

        add_custom_command(
            OUTPUT ${VSYNCER_CHECK_LL}
            COMMAND "${CMD}"
            DEPENDS ${VSYNCER_CHECK_SRC}
            COMMAND_EXPAND_LISTS)

        if(VSYNCER_CHECK)
            set(TARGET_NAME ${VSYNCER_CHECK_TARGET}_${WMM}_build)
            add_custom_target(${TARGET_NAME} ALL
                            DEPENDS ${VSYNCER_CHECK_LL})

            set(BUILD_FIXTURE_NAME "build_${VSYNCER_CHECK_TARGET}_${WMM}")

            # Add a test fixture which ensures the `.ll` file is built before the
            # test is invoked.
            if(BUILD_WITH_CTEST AND NOT TEST "${BUILD_FIXTURE_NAME}")
                add_test(NAME "${BUILD_FIXTURE_NAME}"
                        COMMAND "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR}
                                --target "${TARGET_NAME}")
                set_tests_properties(
                    "${BUILD_FIXTURE_NAME}"
                    PROPERTIES FIXTURES_SETUP "${BUILD_FIXTURE_NAME}")
            endif()
            # Prefix the vsyncer check with full, to get a distinct target name for full tests.
            # This is useful for tracking the test execution time of full tests.
            if(${VSYNCER_CHECK_FULL})
                set(CHECK_PREFIX "full_")
            else()
                set(CHECK_PREFIX "")
            endif()
            # Add a label to the test based on the parent directories, e.g.
            # "datastruct/queue/bbq", which would match on `-L datastruct`
            # as well as `-L bbq`.
            cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH parent_dir)
            cmake_path(RELATIVE_PATH parent_dir
                BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
                OUTPUT_VARIABLE test_dir_label)

                # appending the model to the target name
                set(TESTNAME "${CHECK_PREFIX}${VSYNCER_CHECK_TARGET}_${WMM}")

                set(CMD vsyncer check -d
                    -m "${WMM}" #
                    --csv-log "${VSYNCER_CSV_LOGFILE}" #
                    ${VSYNCER_CHECK_FLAGS} ${VSYNCER_CHECK_LL}
                )

                if (${VSYNCER_CHECK_USE_DAT3M})
                    set(CMD
                        env DARTAGNAN_OPTIONS=${VSYNCER_CHECK_DARTAGNAN_OPTIONS}
                        ${CMD} -c dartagnan)
                elseif(${VSYNCER_CHECK_USE_GENMC10})
                    set(GENMC10_OPTIONS "-${WMM} -disable-estimation -check-liveness -disable-spin-assume")
                    # append extra options to the options' string
                    if(VSYNCER_CHECK_GENMC10_EXTRA_OPTIONS)
                        set(GENMC10_OPTIONS "${GENMC10_OPTIONS} ${VSYNCER_CHECK_GENMC10_EXTRA_OPTIONS}")
                    endif()
                    # overwrite genmc options and pass them in the env-variable
                    # pass the model checker path to use genmc 10
                    set(CMD
                        env GENMC_SET_OPTIONS=${GENMC10_OPTIONS} #
                        ${CMD} #
                        "--model-checker-path" "${GENMC10_PATH}/bin")
                endif()

                if(${VSYNCER_CHECK_FULL})
                    # 96 hours
                    set(CHECK_TIMEOUT 345600)
                elseif(DEFINED ENV{CI})
                    # triple timeouts in CI
                    math(EXPR ci_timeout "${VSYNCER_CHECK_TIMEOUT} * 3")
                    set(CHECK_TIMEOUT ${ci_timeout})
                else()
                    set(CHECK_TIMEOUT ${VSYNCER_CHECK_TIMEOUT})
                endif()

                # add timeout option so that vsyncer fails before the CI
                set(CMD ${CMD} "--timeout" "${CHECK_TIMEOUT}s")

                # update the CI timeout as a fallback in case vsyncer had a bug or failed to check timeout
                math(EXPR CTEST_TIMEOUT "${CHECK_TIMEOUT} + 5")

                add_test(
                    NAME ${TESTNAME}
                    COMMAND ${TOOLKIT_CMD} ${CMD})

                # vsyncer returns 1 when genmc returned an internal error, so we
                # skip these cases
                set_property(TEST ${TESTNAME} PROPERTY SKIP_RETURN_CODE 1)

                if(BUILD_WITH_CTEST)
                    set(fixtures_required_key FIXTURES_REQUIRED)
                    set(fixtures_required_val "${BUILD_FIXTURE_NAME}")
                endif()

                set_tests_properties(
                    ${TESTNAME}
                    PROPERTIES
                        TIMEOUT "${CTEST_TIMEOUT}"
                        "${fixtures_required_key}" "${fixtures_required_val}"
                        LABELS "${test_dir_label};check")
        endif()
    endforeach()
endfunction()


# it is important to set VSYNC_SMR_NOT_AVAILABLE because SMR is not available
# yet and tests need to detect this and act accordingly
target_compile_definitions(
    vsync INTERFACE VSYNC_SMR_NOT_AVAILABLE VSYNC_DISABLE_DEPRECATED_WARNING
                    _GNU_SOURCE)

# add all subdirectories
file(GLOB SUBDIRS ${CMAKE_SOURCE_DIR}/test/*)
foreach(child ${SUBDIRS})
    if(IS_DIRECTORY "${child}")
        add_subdirectory(${child})
    endif()
endforeach()
