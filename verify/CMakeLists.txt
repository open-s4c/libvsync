# it is important to set VSYNC_SMR_NOT_AVAILABLE because SMR is not available
# yet and tests need to detect this and act accordingly
target_compile_definitions(vsync INTERFACE VSYNC_SMR_NOT_AVAILABLE _GNU_SOURCE)

# add all subdirectories
file(GLOB SUBDIRS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/verify/*)
foreach(child ${SUBDIRS})
    if(IS_DIRECTORY "${child}")
        add_subdirectory(${child})
    endif()
endforeach()
