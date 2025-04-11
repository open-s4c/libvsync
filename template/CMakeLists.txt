add_custom_target(vatomic-generate)
# after running vatomic-generate, run clang-format-apply, in order to fix format
# in autogenerted files.
add_custom_command(
    TARGET vatomic-generate
    POST_BUILD
    COMMAND cmake --build "${CMAKE_BINARY_DIR}" --target clang-format-apply
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

add_subdirectory(include)
