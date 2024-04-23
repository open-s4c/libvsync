add_custom_target(vatomic-test-generate)

file(GLOB TEMPLATES *.in)

set(ut_test_TY_TYPES u8 u16 u32 u64 sz ptr)
set(ut_test_await_TY_TYPES u32 u64 ptr)
set(mt_test_await_TY_TYPES u32 u64 ptr)
set(mt_test_rmw_TY_TYPES u32 u64 sz ptr)

foreach(TEMPLATE_FILE IN ITEMS ${TEMPLATES})
    get_filename_component(TEMPLATE_NAME ${TEMPLATE_FILE} NAME_WE)
    foreach(TY ${${TEMPLATE_NAME}_TYPES})
        string(REPLACE TY ${TY} TEST_FILE ${TEMPLATE_NAME}.c)
        add_custom_target(
            ${TEST_FILE} COMMAND ${PROJECT_SOURCE_DIR}/tmplr/tmplr.sh
                                 -DTY=${TY} ${TEMPLATE_FILE} > ../${TEST_FILE})
        add_dependencies(vatomic-test-generate ${TEST_FILE})
    endforeach()
endforeach()

# make sure files stay formatted
add_custom_command(
    TARGET vatomic-test-generate
    POST_BUILD
    COMMAND cmake --build "${CMAKE_BINARY_DIR}" --target clang-format-apply
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
