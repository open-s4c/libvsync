add_subdirectory(config)

# grep templates for each .in .h will be generated
file(GLOB TARGETS *.in)

foreach(TARGET ${TARGETS})
    get_filename_component(TARGET ${TARGET} NAME_WE)
    set(TARGET ${TARGET}.h)
    add_custom_target(
        ${TARGET}
        COMMAND
            ${PROJECT_SOURCE_DIR}/tmplr/tmplr.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/../vatomic.rules
            ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.in >
            ${ATOMICS_DIR}/include/vsync/atomic/internal/${TARGET})
    add_dependencies(vatomic-generate ${TARGET})
endforeach()
