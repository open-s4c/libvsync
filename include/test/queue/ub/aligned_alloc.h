/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_VERIFICATION_ALIGNED_ALLOC
#define VSYNC_VERIFICATION_ALIGNED_ALLOC

#include <stdlib.h>
#include <vsync/atomic.h>

#ifndef NTHREADS
    #warning "undefined number of threads NTHREADS using default 3"
    #define NTHREADS 3
#endif

#ifndef ALLOC_PER_THREAD
    #warning                                                                   \
        "undefined number of allocations per thread ALLOC_PER_THREAD using default 2"
    #define ALLOC_PER_THREAD 2
#endif

#define TARGET_ALIGNMENT 128

#define NODES_COUNT ((NTHREADS + 1) * ALLOC_PER_THREAD)


typedef struct g_map {
    void *node;
    char *parent;
} map_t;


static vbool_t g_aalloc_md_init = false;
static map_t g_map[NODES_COUNT];
static vatomic32_t g_map_idx = VATOMIC_INIT(0);


static inline void
_aligned_alloc_allocate_node(map_t *loc, vsize_t num_bytes)
{
    const vsize_t len = 2 * TARGET_ALIGNMENT;
    char *p           = malloc(sizeof(char) * len);
    for (vsize_t i = 0; i < len; i++) {
        if (V_ATOMICPTR_STAMPED_IS_ALIGNED(&p[i])) {
            loc->parent = p;
            loc->node   = &p[i];
            // double check remaining bytes are enough to cover the required
            // node size
            ASSERT((len - i) >= num_bytes &&
                   "insufficient memory consider increasing len ");
            V_UNUSED(num_bytes);
            return;
        }
    }
    ASSERT(0 && "could not find aligned byte");
}


static inline void
aligned_alloc_init(vsize_t num_bytes)
{
    for (vsize_t i = 0; i < NODES_COUNT; i++) {
        _aligned_alloc_allocate_node(&g_map[i], num_bytes);
    }
    g_aalloc_md_init = true;
}

static inline void *
aligned_alloc_allocate(void)
{
    ASSERT(g_aalloc_md_init);
    vsize_t idx = vatomic32_get_inc(&g_map_idx);
    ASSERT(idx < NODES_COUNT);
    return g_map[idx].node;
}

static inline void
aligned_alloc_free(void *node)
{
    ASSERT(g_aalloc_md_init);
    for (vsize_t i = 0; i < NODES_COUNT; i++) {
        if (g_map[i].node == node) {
            free(g_map[i].parent);
            return;
        }
    }
    ASSERT(0 && "node was not found");
}

#endif
