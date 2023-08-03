project(vatomic_cpp LANGUAGES CXX)
# ##############################################################################
# compilation with C++
# ##############################################################################

add_executable(vatomic_compile_cxx vatomic_empty.cpp)
target_link_libraries(vatomic_compile_cxx vsync)
v_add_bin_test(NAME vatomic-compile-cxx COMMAND vatomic_compile_cxx)
