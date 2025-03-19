/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSKIPLIST_TYPES_H
#define VSKIPLIST_TYPES_H

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/atomic/atomicptr_markable.h>
#include <vsync/common/assert.h>

#ifndef VSKIPLIST_MAX_HEIGHT
    #if defined(VSYNC_VERIFICATION)
        #define VSKIPLIST_MAX_HEIGHT 4U
    #else
        #define VSKIPLIST_MAX_HEIGHT 32U
    #endif
#endif

V_STATIC_ASSERT(VSKIPLIST_MAX_HEIGHT <= VUINT8_MAX,
                "VSKIPLIST_MAX_HEIGHT must be <= VUINT8_MAX");

typedef vuintptr_t vskiplist_key_t;

/**
 * @note when embedding `vskiplist_node_t` in a container struct, place it as
 * the last field.
 *
 */
typedef struct vskiplist_node_s {
    vatomicsz_t non_snipped_level;
    vsize_t height;
    vatomicptr_markable(vskiplist_node_t *) next[];
} vskiplist_node_t;

typedef struct skiplist_seed_s {
    vsize_t max_height;
    vatomicsz_t seed;
} vskiplist_seed_t;

typedef int (*vskiplist_cmp_node_t)(vskiplist_node_t *, vskiplist_key_t);
typedef void (*vskiplist_handle_node_t)(vskiplist_node_t *, void *);

typedef struct vskiplist_s {
    vskiplist_node_t *head;
    vskiplist_node_t *tail;
    vskiplist_seed_t seed;
    vskiplist_cmp_node_t fun_cmp;
    vskiplist_handle_node_t fun_retire;
    void *fun_retire_arg;
} vskiplist_t;

#define VSKIPLIST_SENTINEL_SZ                                                  \
    (sizeof(vskiplist_node_t) +                                                \
     (sizeof(vatomicptr_markable_t) * VSKIPLIST_MAX_HEIGHT))

static inline vskiplist_node_t *
_vskiplist_head(vskiplist_t *lst)
{
    return lst->head;
}

static inline vskiplist_node_t *
_vskiplist_tail(vskiplist_t *lst)
{
    return lst->tail;
}

#endif
