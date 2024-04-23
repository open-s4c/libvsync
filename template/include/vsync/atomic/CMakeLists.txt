add_subdirectory(internal)

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
            ${PROJECT_SOURCE_DIR}/tmplr/tmplr.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/vatomic.rules
            ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.in >
            ${ATOMICS_DIR}/include/vsync/atomic/${TARGET})
    add_dependencies(vatomic-generate ${TARGET})
endforeach()

set(core_TYPES u8 u16 u32 u64 ptr sz)
set(await_TYPES u32 u64 ptr)
set(TARGETS core await)
foreach(TARGET ${TARGETS})
    set(TMPL ${TARGET}_TY.h)
    foreach(TY ${${TARGET}_TYPES})
        string(REPLACE TY ${TY} OUT ${TMPL})
        add_custom_target(
            ${OUT}
            COMMAND
                ${PROJECT_SOURCE_DIR}/tmplr/tmplr.sh -DTY=${TY}
                ${CMAKE_CURRENT_SOURCE_DIR}/vatomic.rules
                ${CMAKE_CURRENT_SOURCE_DIR}/${TMPL}.in >
                ${ATOMICS_DIR}/include/vsync/atomic/${OUT})
        add_dependencies(vatomic-generate ${OUT})
    endforeach()
endforeach()
