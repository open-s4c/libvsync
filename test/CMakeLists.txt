target_compile_definitions(vsync INTERFACE DEFAULT_SMR_EBR _GNU_SOURCE)

# add all subdirectories
file(GLOB SUBDIRS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/test/*)
foreach(child ${SUBDIRS})
    if(IS_DIRECTORY "${child}")
        add_subdirectory(${child})
    endif()
endforeach()
