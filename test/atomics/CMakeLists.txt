# ##############################################################################
# vatomicptr markable and stamped
# ##############################################################################

add_executable(atomicptr_markable atomicptr_markable_test.c)
target_link_libraries(atomicptr_markable vsync pthread)
v_add_bin_test(NAME atomicptr_markable COMMAND atomicptr_markable)

add_executable(atomicptr_stamped atomicptr_stamped_test.c)
target_link_libraries(atomicptr_stamped vsync pthread)
v_add_bin_test(NAME atomicptr_stamped COMMAND atomicptr_stamped)
