# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
set(CONFIG_IN_FILE "cmake/config.cmake.in")

if(NOT DEFINED PROJECT_TARGETS)
    set(PROJECT_TARGETS "${PROJECT_NAME}Targets")
endif()

set(PROJECT_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(PROJECT_VERSION_FILE
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")

include(CMakePackageConfigHelpers)
configure_package_config_file(${CONFIG_IN_FILE} ${PROJECT_CONFIG_FILE}
                              INSTALL_DESTINATION ${PROJECT_CMAKE_DIR})

write_basic_package_version_file(
    ${PROJECT_VERSION_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(
    EXPORT ${PROJECT_TARGETS}
    FILE ${PROJECT_TARGETS}.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${PROJECT_CMAKE_DIR})
install(FILES ${PROJECT_CONFIG_FILE} ${PROJECT_VERSION_FILE}
        DESTINATION ${PROJECT_CMAKE_DIR})
