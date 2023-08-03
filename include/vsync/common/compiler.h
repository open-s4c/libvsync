/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_COMPILER_H
#define VSYNC_COMPILER_H

#ifndef offsetof
	#define offsetof(st, m) __builtin_offsetof(st, m)
#endif

#ifndef likely
	#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
	#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifndef barrier
	#define barrier()                                                          \
		{                                                                      \
			__asm__ __volatile__("" ::: "memory");                             \
		}
#endif

#ifndef container_of
	/**
	 *  @brief Calculate address of struct that contains a certain member from
	 *  the address of the member.
	 *
	 *  Similar macros can be found in other kernels, here we:
	 *  - Don't use void-pointer arithmetic (GCC extension).
	 *  - Use the C-Standard macro 'offsetof' for the address offset; returned
	 *    type is 'vsize_t'.
	 *
	 *  @param ptr Pointer to the member of the container structure.
	 *  @param type Type of the container structure.
	 *  @param member Name of the struct member.
	 *  @return Pointer to struct of type, which contains ptr.
	 */
	#define container_of(ptr, type, member)                                    \
		((type *)((((uintptr_t)(ptr)) - offsetof(type, member))))
#endif

#endif
