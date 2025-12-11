# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# ##############################################################################
# Adds cmake targets that help generate the markdown documentation make doxygen
# make markdown
# ##############################################################################
function(add_doc_targets)
    find_program(MDOX_INSTALLED mdox DOC "mdox")
    find_program(DOXYGEN_INSTALLED doxygen DOC "doxygen")
    set(MARKDOWN_TARGET "markdown")
    set(DOXYGEN_TARGET "doxygen")

    # ##########################################################################
    # Add doxygen target Run with: make doxygen
    # ##########################################################################
    if(NOT DOXYGEN_INSTALLED)
        message(
            WARNING
                "Target `${DOXYGEN_TARGET}` disabled! Please install https://doxygen.nl/download.html"
        )
        return()
    endif()

    set(DOXYGEN_DOCKER "")
    set(DOXYGEN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
    set(DOXYGEN_FILE_IN "${PROJECT_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    set(DOXYGEN_INPUT "${PROJECT_SOURCE_DIR}")
    # Note that this will substitute @VAR@ with the value of VAR in
    # ${DOXYGEN_FILE_IN} and write the outout to ${DOXYGEN_FILE}
    configure_file(${DOXYGEN_FILE_IN} ${DOXYGEN_FILE})
    # create output directory
    file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT}")
    add_custom_target(
        ${DOXYGEN_TARGET}
        COMMAND ${DOXYGEN_DOCKER} doxygen ${DOXYGEN_FILE}
        WORKING_DIRECTORY "${DOXYGEN_INPUT}"
        COMMENT "Generating Doxygen documentation for include folder."
        VERBATIM)

    # ##########################################################################
    # Add markdown target Run with: make markdown
    # ##########################################################################

    if(NOT MDOX_INSTALLED)
        message(
            WARNING
                "Target `${MARKDOWN_TARGET}` disabled! Please install https://github.com/db7/mdox"
        )
        return()
    endif()

    set(DOC_OUTPUT "${PROJECT_SOURCE_DIR}/doc/api")
    set(MDOX mdox)
    add_custom_target(
        "${MARKDOWN_TARGET}"
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${DOC_OUTPUT}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DOC_OUTPUT}"
        COMMAND ${DOXYGEN_DOCKER} ${MDOX} -i ${DOXYGEN_OUTPUT}/xml -o
                "${DOC_OUTPUT}"
        COMMENT "Generating Markdown documentation for ${DOXYGEN_INPUT}"
        VERBATIM
        DEPENDS "${DOXYGEN_TARGET}")
endfunction()
