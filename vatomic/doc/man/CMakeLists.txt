# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

set(__DATE__ "Dec 2025")
set(__VERSION__ ${PROJECT_VERSION})

configure_file(vatomic.7 ${CMAKE_CURRENT_BINARY_DIR}/vatomic.7)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vatomic.7
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man7)

file(GLOB MAN3 *.3)
foreach(file ${MAN3})
    get_filename_component(target ${file} NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/${target})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${target}
            DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)
endforeach()
