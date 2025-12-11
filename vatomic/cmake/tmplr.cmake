# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

set(OPENS4C_URL "https://github.com/open-s4c")
set(TMPLR_VERSION "1.4")
set(TMPLR_URL "${OPENS4C_URL}/tmplr/archive/refs/tags/v${TMPLR_VERSION}.tar.gz")
set(TMPLR_SHA256
    "ab6b67cd9894afbd8f262a7739598902c873c89007bcddb818afe65b405294ea")

if(DEFINED TMPLR_PROGRAM AND EXISTS "${TMPLR_PROGRAM}")
    # Cached values are valid; nothing further needed.
    return()
endif()

execute_process(
    COMMAND
        "${CMAKE_SOURCE_DIR}/scripts/ensure-cmd.sh" -q #
        --workdir "${CMAKE_BINARY_DIR}" #
        --url "${TMPLR_URL}" #
        --sha256 "${TMPLR_SHA256}" #
        "tmplr" "${TMPLR_VERSION}"
    OUTPUT_VARIABLE TMPLR_PROGRAM
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE ENSURE_TMPLR_RC)
if(NOT ENSURE_TMPLR_RC EQUAL 0)
    message(FATAL_ERROR "Failed to ensure tmplr ${TMPLR_VERSION} is available")
endif()
message(STATUS "tmplr v${TMPLR_VERSION}: ${TMPLR_PROGRAM}")
