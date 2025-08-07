# ##############################################################################
# Adds cmake targets that help generate the markdown documentation make doxygen
# make markdown
# ##############################################################################
function(add_doc_targets)
    set(DOXYGEN_DOCKER "")
    set(INSTALL_TARGET "install-libvsync")
    # Note that for now we generate the doc based on the files we install with
    # libvsync Once we have a separate doc for vatomic&doc.h we can just use
    # ${PROJECT_SOURCE_DIR}/include and skip the installation step.
    set(DOXYGEN_INPUT "${CMAKE_CURRENT_BINARY_DIR}/installed")
    add_custom_target(
        ${INSTALL_TARGET}
        COMMAND
            ${CMAKE_COMMAND} -S"${PROJECT_SOURCE_DIR}"
            -B"${CMAKE_CURRENT_BINARY_DIR}"
            -DCMAKE_INSTALL_PREFIX="${DOXYGEN_INPUT}"
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_CURRENT_BINARY_DIR}")
    # ##########################################################################
    # Generate doxygen config file
    # ##########################################################################
    set(DOXYGEN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
    set(DOXYGEN_FILE_IN "${PROJECT_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    # Note that this will substitute @VAR@ with the value of VAR in
    # ${DOXYGEN_FILE_IN} and write the outout to ${DOXYGEN_FILE}
    configure_file(${DOXYGEN_FILE_IN} ${DOXYGEN_FILE})
    # ##########################################################################
    # Add doxygen target Run with: make doxygen
    # ##########################################################################
    set(DOXYGEN_TARGET "doxygen")
    # create output directory
    file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT}")
    add_custom_target(
        ${DOXYGEN_TARGET}
        COMMAND ${DOXYGEN_DOCKER} doxygen ${DOXYGEN_FILE}
        WORKING_DIRECTORY "${DOXYGEN_INPUT}"
        COMMENT "Generating Doxygen documentation for include folder."
        DEPENDS ${INSTALL_TARGET}
        VERBATIM)
    # ##########################################################################
    # Add markdown target Run with: make markdown
    # ##########################################################################
    set(DOC_OUTPUT "${PROJECT_SOURCE_DIR}/doc/api")
    set(MDOX mdox)
    set(MARKDOWN_TARGET "markdown")
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
