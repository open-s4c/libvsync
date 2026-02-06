file(GLOB EXAMPLES eg_*.c)

# exclude examples that rely on unreleased algos.
set(EXCLUDE_LIST
    eg_hashtable_evictable #
    eg_hashtable_standard #
    eg_listset_coarse #
    eg_listset_fine #
    eg_listset_lazy #
    eg_listset_lf #
    eg_listset_opt #
    eg_queue_prio_skiplist #
    eg_queue_prio_stack_array #
    eg_queue_prio_stack_tree #
    eg_skiplist_lf #
    eg_stack_elimination #
    eg_stack_xbo #
    eg_taslock # TODO release
    eg_unbounded_queue_lf #
)

set(EXAMPLES_BUILD_TARGET examples)
add_custom_target(${EXAMPLES_BUILD_TARGET})
# compile each c file in examples folder
foreach(EG IN ITEMS ${EXAMPLES})
    get_filename_component(EXAMPLE ${EG} NAME_WE)
    if(EXAMPLE IN_LIST EXCLUDE_LIST)
        continue()
    endif()
    add_executable(${EXAMPLE} ${EG})
    # you should not add additional libs here
    target_link_libraries(${EXAMPLE} vsync pthread)
    add_dependencies(${EXAMPLES_BUILD_TARGET} ${EXAMPLE})
    # add it as a test
    add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE})
endforeach()
