/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_COMPILER_H
#define VSYNC_COMPILER_H

#include <vsync/vtypes.h>

#ifndef likely
    #define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
    #define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#if defined(offsetof)
    #define V_OFFSET_OF(_st_, _m_) offsetof(_st_, _m_)
#else
    #define V_OFFSET_OF(_st_, _m_) __builtin_offsetof(_st_, _m_)
#endif

#ifndef barrier
    #define barrier()                                                          \
        {                                                                      \
            __asm__ __volatile__("" ::: "memory");                             \
        }
#endif

#if defined(container_of)
    #define V_CONTAINER_OF(_ptr_, _type_, _member_)                            \
        container_of(_ptr_, _type_, _member_)
#else
    /**
     *  @brief Calculate address of struct that contains a certain member from
     *  the address of the member.
     *
     *  Similar macros can be found in other kernels, here we:
     *  - Don't use void-pointer arithmetic (GCC extension).
     *  - Use the C-Standard macro 'offsetof' for the address offset; returned
     *    type is 'vsize_t'.
     *
     *  @param _ptr_ Pointer to the member of the container structure.
     *  @param _type_ Type of the container structure.
     *  @param _member_ Name of the struct member.
     *  @return Pointer to struct of type, which contains ptr.
     */
    #define V_CONTAINER_OF(_ptr_, _type_, _member_)                            \
        ((_type_ *)((((vuintptr_t)(_ptr_)) - V_OFFSET_OF(_type_, _member_))))
#endif

#endif
