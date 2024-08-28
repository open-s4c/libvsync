option(LIBVSYNC_LOTTO "enables lotto stress tests" off)

# ##############################################################################
# Adds a lotto stress teset @param NAME test name, prefix lotto_ will be added #
# to it                                                                        #
# @param SOURCE path to the source file of the test                            #
# @param ROUNDS number of rounds to run lotto stress                           #
# @param CDEFS additional compiler definitions to                              #
# compile with, by default -DLOTTO -DVSYNC_THREAD_SANITIZER are defined        #
# @param COPTS additional compile options to compile with                      #
# ##############################################################################
function(add_lotto_test)
    if(NOT LIBVSYNC_LOTTO)
        return()
    else()
        # check if lotto binary is available
        find_program(LOTTO_EXISTS lotto)
        if(NOT LOTTO_EXISTS)
            message(
                WARNING
                    "lotto is not found, lotto tests will not be added! Consider installing lotto and try again on a fresh build folder"
            )
            return()
        endif()
    endif()
    # ##########################################################################
    # Parse arguments
    # ##########################################################################
    set(opts)
    set(ones NAME SOURCE ROUNDS)
    set(mult CDEFS COPTS)
    cmake_parse_arguments(LTEST "${opts}" "${ones}" "${mult}" ${ARGN})

    # ##########################################################################
    # Add executable compilation
    # ##########################################################################
    set(TARGET "lotto_${LTEST_NAME}")
    add_executable(${TARGET} "${LTEST_SOURCE}")
    target_link_libraries(${TARGET} PRIVATE vsync pthread tsan)
    target_compile_definitions(
        ${TARGET} PRIVATE VSYNC_LOTTO VSYNC_THREAD_SANITIZER "${LTEST_CDEFS}")
    target_compile_options(${TARGET} PRIVATE -fsanitize=thread "${LTEST_COPTS}")
    # ##########################################################################
    # Add ctest with lotto stress
    # ##########################################################################
    set(TEST_NAME "lotto_stress_${LTEST_NAME}")
    add_test(NAME ${TEST_NAME} COMMAND lotto stress -r ${LTEST_ROUNDS}
                                       ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

    # ##########################################################################
    # Add ctest with lotto stress
    # ##########################################################################
    if(BUILD_WITH_CTEST)
        set(TEST_FIXTURE_NAME "build_${TEST_NAME}")
        if(NOT TEST ${TEST_FIXTURE_NAME})
            add_test(NAME ${TEST_FIXTURE_NAME}
                     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                             --target ${TARGET})
            set_tests_properties(${TEST_FIXTURE_NAME}
                                 PROPERTIES FIXTURES_SETUP ${TEST_FIXTURE_NAME})
        endif()
        set_tests_properties(${TEST_NAME} PROPERTIES FIXTURES_REQUIRED
                                                     ${TEST_FIXTURE_NAME})
    endif()

endfunction()
