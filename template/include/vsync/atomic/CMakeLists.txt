if(EXISTS internal)
    add_subdirectory(internal)
endif()

# find all template inputs
file(GLOB TARGETS *.in)
# exclude those generated for every type
list(FILTER TARGETS EXCLUDE REGEX "TY.h.in")

foreach(TARGET ${TARGETS})
    get_filename_component(TARGET ${TARGET} NAME_WE)
    set(TARGET ${TARGET}.h)
    add_custom_target(
        ${TARGET}
        COMMAND
            $<TARGET_FILE:tmplr.bin> ${CMAKE_CURRENT_SOURCE_DIR}/vatomic.rules
            ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.in >
            ${ATOMICS_DIR}/include/vsync/atomic/${TARGET})
    add_dependencies(vatomic-generate ${TARGET})
endforeach()
