include(CTest)
include(ProcessorCount)
include(${CMAKE_SOURCE_DIR}/cmake/v_add_test.cmake)

# `add_vsyncer_check` is used for verification. It is currently unavailable,
# this is a place holder function to avoid errors
function(add_vsyncer_check)
    # place holder
endfunction()

# it is important to set VSYNC_SMR_NOT_AVAILABLE because SMR is not available
# yet and tests need to detect this and act accordingly
target_compile_definitions(
    vsync INTERFACE VSYNC_SMR_NOT_AVAILABLE VSYNC_DISABLE_DEPRECATED_WARNING
                    _GNU_SOURCE)

# add all subdirectories
file(GLOB SUBDIRS ${CMAKE_SOURCE_DIR}/test/*)
foreach(child ${SUBDIRS})
    if(IS_DIRECTORY "${child}")
        add_subdirectory(${child})
    endif()
endforeach()
