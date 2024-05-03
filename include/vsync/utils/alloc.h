/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ALLOC_H
#define VSYNC_ALLOC_H

#include <vsync/vtypes.h>
#include <vsync/utils/math.h>

typedef void (*vmem_free_fun_t)(void *ptr, void *arg);
typedef void *(*vmem_malloc_fun_t)(vsize_t sz, void *arg);

typedef struct vmem_lib_s {
    vmem_free_fun_t free_fun;
    vmem_malloc_fun_t malloc_fun;
    void *arg;
} vmem_lib_t;

static inline vbool_t
vmem_lib_not_null(vmem_lib_t *vmem)
{
    if (!vmem->free_fun) {
        return false;
    }
    if (!vmem->malloc_fun) {
        return false;
    }
    return true;
}

static inline void
vmem_lib_copy(vmem_lib_t *des, vmem_lib_t *src)
{
#if defined(VSYNC_VERIFICATION)
    des->free_fun   = src->free_fun;
    des->malloc_fun = src->malloc_fun;
    des->arg        = src->arg;
#else
    *des = *src;
#endif
}
/**
 * Returns an aligned address that >= to `a`.
 *
 * @param a memory address.
 * @param alignment alignment in bytes, must be power of two.
 * @return void* aligned address.
 */
static inline void *
vmem_align_addr(void *a, vsize_t alignment)
{
    vuintptr_t addr = (vuintptr_t)a;
    if (addr == 0) {
        return NULL;
    }
    ASSERT(V_IS_POWER_OF_TWO(alignment));
    addr = (addr - 1) & ~((vuintptr_t)alignment - 1);
    addr = addr + alignment;
    return (void *)addr;
}
/**
 * Checks if the given address is in [start, end].
 *
 * @param addr address.
 * @param start range start.
 * @param end range end.
 * @return true addr is in [start, end].
 * @return false addr is outside [start, end].
 */
static inline vbool_t
vmem_addr_within_range(void *addr, void *start, void *end)
{
    vbool_t in_range = ((vuintptr_t)addr >= (vuintptr_t)start) &&
                       ((vuintptr_t)addr <= (vuintptr_t)end);
    return in_range;
}
/**
 * Checks if the given address is `alignment` number of bytes aligned.
 *
 * @param a memory address.
 * @param alignment alignment in bytes.
 * @return true aligned.
 * @return false not aligned.
 *
 * @note alignment must be power of two.
 */
static inline vbool_t
vmem_is_addr_aligned(void *a, vsize_t alignment)
{
    vuintptr_t addr = (vuintptr_t)a;
    ASSERT(V_IS_POWER_OF_TWO(alignment));
    vbool_t aligned = (addr & (vuintptr_t)(alignment - 1U)) == 0U;
    return aligned;
}
#endif
