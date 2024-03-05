include_guard(GLOBAL)

option(BUILD_WITH_CTEST "Add test fixture to build before testing" ON)

# Wrapper around `add_test` that adds a dependency on the build. Parameters:
# NAME: Test name, COMMAND: First value is the binary, anything else gets passed
# through.
function(v_add_bin_test)
    set(options "")
    # `add_test` command can have multiple values, but we are only interested in
    # the first one, which is the executable target. All arguments are available
    # in `PREFIX_UNPARSED_ARGUMENTS` and will be passed through to the final
    # `add_test` COMMAND.
    set(single_value_args "NAME" "COMMAND")
    set(multi_value_args "")
    set(mandatory_arguments NAME COMMAND)
    cmake_parse_arguments(TST "${options}" "${single_value_args}"
                          "${multi_value_args}" ${ARGN})

    foreach(argname ${mandatory_arguments})
        if(NOT DEFINED "TST_${argname}")
            message(
                FATAL_ERROR
                    "Missing mandatory parameter: ${argname}.\n"
                    "Help: The following parameters are required: ${mandatory_arguments}"
            )
        endif()
    endforeach()

    if(NOT TARGET "${TST_COMMAND}")
        message(
            FATAL_ERROR
                "COMMAND argument ${TST_COMMAND} to `v_add_bin_test` was not a CMake target"
        )
    endif()

    # Add a label to the test based on the parent directories, e.g.
    # "datastruct/queue/bbq", which would match on `-L datastruct` as well as
    # `-L bbq`.
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH parent_dir)
    cmake_path(RELATIVE_PATH parent_dir BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
               OUTPUT_VARIABLE test_dir_label)

    add_test(NAME "${TST_NAME}" COMMAND "${TST_COMMAND}"
                                        ${TST_UNPARSED_ARGUMENTS})

    if(BUILD_WITH_CTEST)
        if(NOT TEST "build_${TST_COMMAND}")
            add_test(NAME "build_${TST_COMMAND}"
                     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                             --target "${TST_COMMAND}")
            set_tests_properties(
                "build_${TST_COMMAND}" PROPERTIES FIXTURES_SETUP
                                                  "build_${TST_COMMAND}")
        endif()
        set_tests_properties("${TST_NAME}" PROPERTIES FIXTURES_REQUIRED
                                                      "build_${TST_COMMAND}")
    endif()
    if(V_TEST_LABELS)
        list(APPEND test_dir_label ${V_TEST_LABELS})
    endif()
    set_tests_properties("${TST_NAME}" PROPERTIES LABELS "${test_dir_label}")

endfunction()

function(v_add_bin_test_advanced)
    set(options "")
    set(single_value_args "NAME" "TIMEOUT" "COST" "PROCESSORS")
    set(multi_value_args "")

    cmake_parse_arguments(TST "${options}" "${single_value_args}"
                          "${multi_value_args}" ${ARGN})

    v_add_bin_test(NAME "${TST_NAME}" ${TST_UNPARSED_ARGUMENTS})

    # Default COST to the Timeout value (if set). If `COST` is explicitly set,
    # it will later override this default cost.
    if("${TST_TIMEOUT}")
        set_tests_properties("${TST_NAME}" PROPERTIES COST "${TST_TIMEOUT}")
    endif()

    # Override default values with explicit values (if provided)
    foreach(ctest_prop TIMEOUT COST PROCESSORS)
        if(DEFINED "TST_${ctest_prop}")
            set_tests_properties(
                "${TST_NAME}" PROPERTIES "${ctest_prop}" "${TST_${ctest_prop}}")
        endif()
    endforeach()

endfunction()

# Adds options that override the default values for stress tests
function(v_add_heavy_stress_test)
    set(options "")
    set(single_value_args
        "NAME"
        "STRESS_TIMEOUT"
        "STRESS_COST"
        "STRESS_PROCESSORS"
        "TIMEOUT"
        "COST"
        "PROCESSORS")
    set(multi_value_args "")

    cmake_parse_arguments(TST "${options}" "${single_value_args}"
                          "${multi_value_args}" ${ARGN})

    set(ctest_prop_args "")
    if("${TST_NAME}" MATCHES "_stress_")
        foreach(ctest_prop "TIMEOUT" "COST" "PROCESSORS")
            if(DEFINED "TST_STRESS_${ctest_prop}")
                list(APPEND ctest_prop_args "${ctest_prop}"
                     ${TST_STRESS_${ctest_prop}})
            elseif(DEFINED "TST_${ctest_prop}")
                list(APPEND ctest_prop_args "${ctest_prop}"
                     ${TST_${ctest_prop}})
            endif()
        endforeach()
    else()
        foreach(ctest_prop "TIMEOUT" "COST" "PROCESSORS")
            if(DEFINED "TST_${ctest_prop}")
                list(APPEND ctest_prop_args "${ctest_prop}"
                     ${TST_${ctest_prop}})
            endif()
        endforeach()
    endif()
    v_add_bin_test_advanced(NAME "${TST_NAME}" ${ctest_prop_args}
                            ${TST_UNPARSED_ARGUMENTS})

endfunction()
