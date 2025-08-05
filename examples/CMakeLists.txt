file(GLOB EXAMPLES eg_*.c)

add_custom_target(test_example)
# compile each c file in examples folder
foreach(example IN ITEMS ${EXAMPLES})
    get_filename_component(EXAMPLE ${example} NAME_WE)
    add_executable(${EXAMPLE} ${example})
    # you should not add additional libs here
    target_link_libraries(${EXAMPLE} vsync pthread)
    add_dependencies(test_example ${EXAMPLE})
    # add it as a test
    add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE})
endforeach()
