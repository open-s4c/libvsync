target_include_directories(
    vsync INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                    "$<INSTALL_INTERFACE:include>")

install(DIRECTORY include/vsync DESTINATION include)

vsync_install(
    DIRECTORY include/vsync/queue
    DESTINATION include/vsync
    COMPONENTS STABLE OPEN_DISTRO)

vsync_install(
    DIRECTORY test/
    DESTINATION test/queue
    COMPONENTS STABLE_TEST OPEN_DISTRO
    EXTRA_ARGS EXCLUDE_FROM_ALL)

if(${LIBVSYNC_BUILD_TESTS})
    add_subdirectory(test)
    add_subdirectory(verify)
endif()
