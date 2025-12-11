cmake_minimum_required(VERSION 3.16)

project(
    libvsync
    LANGUAGES C
    VERSION 4.2.1
    DESCRIPTION
        "Verified library of atomics, synchronization primitives and concurrent data structures"
)

option(LIBVSYNC_ADDRESS_SANITIZER "Compile with -fsanitize=address" "off")
option(LIBVSYNC_CODECHECK_BINSCOPE "Compile with necessary flags for binscope"
       "off")

include(GNUInstallDirs)
include(cmake/export.cmake)
include(cmake/doc.cmake)
include(CheckSymbolExists)

# ##############################################################################
# add vatomic lib.
# ##############################################################################
find_package(vatomic 3.2 CONFIG)
if(NOT vatomic_FOUND)
    add_subdirectory(vatomic)
endif()

add_library(vsync INTERFACE)
# make vatomic a dependency of vsync
target_link_libraries(vsync INTERFACE vatomic)

target_include_directories(
    vsync INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(DIRECTORY include/vsync DESTINATION include)
install(DIRECTORY vatomic/include/vsync DESTINATION include)
install(FILES vmm/vmm.cat DESTINATION share/vsync)
install(TARGETS vsync EXPORT ${PROJECT_TARGETS})

# ##############################################################################
# Check for memset_s memcpy_s existence
# ##############################################################################
check_symbol_exists(memset_s "string.h" VSYNC_MEMSET_S_EXISTS)
if(VSYNC_MEMSET_S_EXISTS)
    target_compile_definitions(vsync INTERFACE "VSYNC_MEMSET_S_EXISTS")
endif()
check_symbol_exists(memcpy_s "string.h" VSYNC_MEMCPY_S_EXISTS)
if(VSYNC_MEMCPY_S_EXISTS)
    target_compile_definitions(vsync INTERFACE "VSYNC_MEMCPY_S_EXISTS")
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # if top level, enable testing by default
    set(TESTING_DEFAULT ON)
else()
    set(TESTING_DEFAULT OFF)
endif()

option(LIBVSYNC_ENABLE_TESTS "Enable libvsync tests" ${TESTING_DEFAULT})

if(${LIBVSYNC_ENABLE_TESTS})
    # ##########################################################################
    # Important Config must be set
    # ##########################################################################
    set(ATOMICS_DIR ${PROJECT_SOURCE_DIR})

    # ##########################################################################
    # Commands
    # ##########################################################################
    set(CMAKE_FMT_CMD ${PROJECT_SOURCE_DIR}/scripts/cmake-format.sh
                      ${CMAKE_SOURCE_DIR})

    set(CMAKE_STYLE_FILE ${PROJECT_SOURCE_DIR}/.cmake-format)

    add_custom_target(cmake-format-apply COMMAND env STYLE=${CMAKE_STYLE_FILE}
                                                 SILENT=true ${CMAKE_FMT_CMD})

    set(CLANG_FMT_CMD ${PROJECT_SOURCE_DIR}/scripts/clang-format.sh
                      ${CMAKE_SOURCE_DIR})

    set(CLANG_STYLE_FILE ${PROJECT_SOURCE_DIR}/.clang-format)

    add_custom_target(clang-format-apply COMMAND env STYLE=${CLANG_STYLE_FILE}
                                                 SILENT=true ${CLANG_FMT_CMD})

    add_custom_target(
        sanitize-vsync
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/sanitize.sh
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    add_custom_target(diff-check COMMAND git --no-pager diff --exit-code)

    add_dependencies(clang-format-apply sanitize-vsync)

    add_doc_targets()

    set(LIBVSYNC_OPEN_DISTRO_TESTING "on")
    enable_testing()
    include(CTest)
    include(ProcessorCount)
    include(${CMAKE_SOURCE_DIR}/cmake/v_add_test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/check.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/lotto_test.cmake)
    include(cmake/tmplr.cmake)
    set(TMPLR_PROGRAM ${TMPLR_PROGRAM} -P_tmpl -b 1000)
    add_subdirectory(template)
    add_subdirectory(test)
    add_subdirectory(verify)
    add_subdirectory(examples)
    if(LIBVSYNC_ADDRESS_SANITIZER)
        target_compile_options(vsync INTERFACE -fsanitize=address)
        target_compile_definitions(vsync INTERFACE VSYNC_ADDRESS_SANITIZER)
        target_link_options(vsync INTERFACE -fsanitize=address)
    endif()
    if(LIBVSYNC_CODECHECK_BINSCOPE)
        target_compile_definitions(vsync INTERFACE _FORTIFY_SOURCE=2)
        target_compile_options(
            vsync INTERFACE -fstack-protector-strong -fstack-protector-all
                            -fPIE -fPIC -O2)
        target_link_options(vsync INTERFACE -s -pie -Wl,-z,relro,-z,now)
    endif()
endif()
