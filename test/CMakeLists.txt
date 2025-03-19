target_include_directories(
    vsync INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)

target_compile_definitions(vsync INTERFACE DEFAULT_SMR_EBR _GNU_SOURCE)

# add all subdirectories
file(GLOB SUBDIRS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/test/*)
list(REMOVE_ITEM SUBDIRS "${CMAKE_SOURCE_DIR}/test/include")

foreach(child ${SUBDIRS})
    if(IS_DIRECTORY "${child}")
        add_subdirectory(${child})
    endif()
endforeach()
