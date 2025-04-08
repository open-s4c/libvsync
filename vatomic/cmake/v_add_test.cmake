# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
include_guard(GLOBAL)

# Wrapper around `add_test`
function(v_add_bin_test)
    set(options "")
    set(single_value_args "NAME" "COMMAND")
    set(multi_value_args "")
    set(mandatory_arguments NAME COMMAND)
    cmake_parse_arguments(TST "${options}" "${single_value_args}"
                          "${multi_value_args}" ${ARGN})
    add_test(NAME ${TST_NAME} COMMAND ${TST_COMMAND} ${TST_UNPARSED_ARGUMENTS})

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
