/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BBQ_COMMON_H
#define VSYNC_BBQ_COMMON_H

#include <vsync/utils/math.h>
#include <vsync/utils/string.h>
#include <vsync/common/compiler.h>
#include <vsync/common/cache.h>

/* utils */

/* size related */
/* mininum and maxinum entry size, block size and block number */
#define BBQ_ENTRY_SIZE         (1ULL << BBQ_ENTRY_SIZE_LOG)
#define BBQ_MIN_BLOCK_NUM_LOG  0U
#define BBQ_MAX_BLOCK_NUM_LOG  16U
#define BBQ_MIN_ENTRY_SIZE     4U
#define BBQ_MAX_ENTRY_SIZE     (1ULL << (BBQ_MAX_BLOCK_SIZE_LOG - 1))
#define BBQ_MIN_BLOCK_SIZE_LOG 4U
#define BBQ_MAX_BLOCK_SIZE_LOG 16U

/* offset and mask */
/* global var related */
#define BBQ_GLOBA_VERSION_BIT    44U /* 64 - 20 */
#define BBQ_GLOBA_VERSION_MASK   ((1ULL << BBQ_GLOBA_VERSION_BIT) - 1)
#define BBQ_GLOBAL_IDX(v)        ((v) & ((1ULL << BBQ_BLOCK_NUM_LOG) - 1))
#define BBQ_GLOBAL_VSN(v)        (((v) >> BBQ_BLOCK_NUM_LOG) & BBQ_GLOBA_VERSION_MASK)
#define BBQ_GLOBAL_COMPOSE(h, l) (((h) << BBQ_BLOCK_NUM_LOG) | (l))

/* local var related */
#define BBQ_LOCAL_SPACE_BIT     20U /* >16 to provent the FAA overflow */
#define BBQ_LOCAL_SPACE_MASK    ((1ULL << BBQ_LOCAL_SPACE_BIT) - 1)
#define BBQ_LOCAL_IDX(v)        ((v)&BBQ_LOCAL_SPACE_MASK)
#define BBQ_LOCAL_VSN(v)        ((v) >> BBQ_LOCAL_SPACE_BIT)
#define BBQ_LOCAL_COMPOSE(h, l) (((h) << BBQ_LOCAL_SPACE_BIT) | (l))

typedef struct bbq_config_s {
    vuint16_t blk_size_log; /* total size of each block (in log) */
    vuint16_t blk_size;     /* total size of each block */
} bbq_config_t;

#define BBQ_BLOCK_INIT_VALUE(S)                                                \
    ((sizeof(S) / BBQ_ENTRY_SIZE + 1) * BBQ_ENTRY_SIZE)

#ifdef VSYNC_VERIFICATION
static vuint32_t g_verify_bbq_count = 0;
    #define BBQ_VERIFY_BLK_COUNT(count) g_verify_bbq_count = (count)
    #define BBQ_BLK_SZ_VERIFICATION(name)                                      \
        do {                                                                   \
            ASSERT(g_verify_bbq_count != 0 && "must set BBQ_COUNT");           \
            vuint32_t count_per_blk =                                          \
                (g_verify_bbq_count) >> BBQ_BLOCK_NUM_LOG;                     \
            blk_size = BBQ_BLOCK_INIT_VALUE(struct bbq_##name##_block_s) +     \
                       (count_per_blk << BBQ_ENTRY_SIZE_LOG);                  \
        } while (0)
#else
    #define BBQ_VERIFY_BLK_COUNT(count)                                        \
        do {                                                                   \
        } while (0)
    #define BBQ_BLK_SZ_VERIFICATION(name)                                      \
        do {                                                                   \
        } while (0)
#endif

#define BBQ_GET_BLOCK(rb, idx)                                                 \
    (void *)((rb)->blk + ((idx) << (rb)->config.blk_size_log))
#define BBQ_GET_NEXT_BLOCK(rb, idx)                                            \
    (BBQ_GET_BLOCK(rb, BBQ_GLOBAL_IDX((idx) + 1)))
#define BBQ_GET_ENTRY(blk, offset)                                             \
    ((void *)(((vuintptr_t)(blk)) + (vuintptr_t)(offset)))

#define BBQ_BLOCK_FULLY_CONSUMED_WITH_VSN(blk, sz, vsn)                        \
    ({                                                                         \
        vuint64_t consumed = vatomic64_read_acq(&(blk)->consumed);             \
        (BBQ_LOCAL_IDX(consumed) == (sz) &&                                    \
         BBQ_LOCAL_VSN(consumed) == (vsn)) ||                                  \
            BBQ_LOCAL_VSN(consumed) > (vsn);                                   \
    })

#define BBQ_RESET_BLOCK_CURSOR_LIGHT(v, new_vsn, init_v)                       \
    vatomic64_write_rlx(v, BBQ_LOCAL_COMPOSE(new_vsn, init_v))

static inline void
bbq_reset_block_cursor_heavy(vatomic64_t *v, vuint64_t new_vsn,
                             vuintptr_t init_v)
{
    vuint64_t new_cursor = BBQ_LOCAL_COMPOSE(new_vsn, init_v);
    vatomic64_max(v, new_cursor);
}

#define BBQ_ADVANCE_HEAD(v, old, new) vatomic64_max(v, new)

#define BBQ_COUNT(rb, name, name_uc)                                           \
    ({                                                                         \
        vuint64_t ridx = BBQ_##name_uc##_READ_CONS((rb)->ridx);                \
        struct bbq_##name##_block_s *rblk =                                    \
            BBQ_GET_BLOCK(rb, BBQ_GLOBAL_IDX(ridx));                           \
        vuintptr_t consumed_index =                                            \
            BBQ_LOCAL_IDX(vatomic64_read_rlx(&rblk->consumed));                \
                                                                               \
        vuint64_t widx = BBQ_##name_uc##_READ_PROD((rb)->widx);                \
        struct bbq_##name##_block_s *wblk =                                    \
            BBQ_GET_BLOCK(rb, BBQ_GLOBAL_IDX(widx));                           \
        vuintptr_t committed_index =                                           \
            BBQ_LOCAL_IDX(vatomic64_read_rlx(&wblk->committed));               \
                                                                               \
        vuintptr_t block_size = (rb)->config.blk_size;                         \
        ((((widx - ridx) *                                                     \
           (block_size - BBQ_BLOCK_INIT_VALUE(struct bbq_##name##_block_s))) + \
          (committed_index - consumed_index)) >>                               \
         BBQ_ENTRY_SIZE_LOG);                                                  \
    })

#endif /* VSYNC_BBQ_COMMON_H */
