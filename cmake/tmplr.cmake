# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: MIT                                                 #

include(ExternalProject)
set(OPENS4C_URL "https://github.com/open-s4c")
set(TMPLR_VERSION "1.4")
set(TMPLR_URL "${OPENS4C_URL}/tmplr/archive/refs/tags/v${TMPLR_VERSION}.tar.gz")
set(TMPLR_SHA256
    "ab6b67cd9894afbd8f262a7739598902c873c89007bcddb818afe65b405294ea")
set(TMPLR_PROGRAM "${CMAKE_BINARY_DIR}/tmplr/tmplr")

ExternalProject_Add(
    tmplr-build
    URL ${TMPLR_URL}
    URL_HASH SHA256=${TMPLR_SHA256}
    SOURCE_DIR "${CMAKE_BINARY_DIR}/tmplr"
    BINARY_DIR "${CMAKE_BINARY_DIR}/tmplr"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_COMMAND}
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${TMPLR_PROGRAM})

message(STATUS "TMPLR_PROGRAM will be at ${TMPLR_PROGRAM}")

add_custom_target(
    tmplr-check
    COMMAND ${TMPLR_PROGRAM} -V
    COMMENT "Running tmplr version check"
    DEPENDS tmplr-build)
