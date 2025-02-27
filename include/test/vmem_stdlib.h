/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#ifndef VTEST_VMEM_H
#define VTEST_VMEM_H

#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <stdlib.h>
#include <vsync/common/assert.h>
#include <vsync/utils/alloc.h>
#include <vsync/vtypes.h>

#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
vatomic64_t _g_vmem_alloc_count = VATOMIC_INIT(0);
vatomic64_t _g_vmem_free_count  = VATOMIC_INIT(0);
#endif


static inline void *
vmem_malloc(vsize_t sz)
{
    void *ptr = malloc(sz);
    if (ptr) {
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
        vatomic64_inc_rlx(&_g_vmem_alloc_count);
#endif
    } else {
        ASSERT(0 && "allocation failed");
    }
    return ptr;
}

static inline void *
vmem_malloc_cb(vsize_t sz, void *arg)
{
    V_UNUSED(arg);
    return vmem_malloc(sz);
}

static inline void *
vmem_aligned_malloc(vsize_t alignment, vsize_t sz)
{
    void *ptr = NULL;
    if (posix_memalign(&ptr, alignment, sz) != 0) {
        abort();
    }
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
    vatomic64_inc_rlx(&_g_vmem_alloc_count);
#endif
    return ptr;
}

static inline void *
vmem_calloc(vsize_t nitems, vsize_t size)
{
#ifdef VSYNC_VERIFICATION
    return vmem_malloc(nitems * size);
#else
    void *ptr = calloc(nitems, size);
    if (ptr) {
    #if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
        vatomic64_inc_rlx(&_g_vmem_alloc_count);
    #endif
    } else {
        ASSERT(0 && "calloc failed");
    }
    return ptr;
#endif
}


static inline void
vmem_free(void *ptr)
{
    free(ptr);
    if (ptr) {
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
        vatomic64_inc_rlx(&_g_vmem_free_count);
#endif
    }
}

static inline void
vmem_free_cb(void *ptr, void *arg)
{
    V_UNUSED(arg);
    vmem_free(ptr);
}


static inline vuint64_t
vmem_get_alloc_count(void)
{
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
    vuint64_t alloc_count = vatomic64_read_rlx(&_g_vmem_alloc_count);
    DBG_YELLOW("#allocations = %" VUINT64_FORMAT, alloc_count);
    return alloc_count;
#else
    ASSERT(0 && "Tracking is turned off by VMEM_LIB_ALLOC_TRACKING_OFF");
    return 0;
#endif
}


static inline vuint64_t
vmem_get_free_count(void)
{
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
    vuint64_t free_count = vatomic64_read_rlx(&_g_vmem_free_count);
    DBG_YELLOW("#frees = %" VUINT64_FORMAT, free_count);
    return free_count;
#else
    ASSERT(0 && "Tracking is turned off by VMEM_LIB_ALLOC_TRACKING_OFF");
    return 0;
#endif
}

static inline void
vmem_inc_free_count(void)
{
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
    vatomic64_inc_rlx(&_g_vmem_free_count);
#endif
}

static inline void
vmem_inc_alloc_count(void)
{
#if !defined(VMEM_LIB_ALLOC_TRACKING_OFF)
    vatomic64_inc_rlx(&_g_vmem_alloc_count);
#endif
}

static inline vbool_t
vmem_no_leak(void)
{
    vuint64_t alloc_count = vmem_get_alloc_count();
    vuint64_t free_count  = vmem_get_free_count();
    return (alloc_count == free_count);
}

#define VMEM_LIB_DEFAULT()                                                     \
    (vmem_lib_t)                                                               \
    {                                                                          \
        .free_fun = vmem_free_cb, .malloc_fun = vmem_malloc_cb, .arg = NULL    \
    }

#endif
