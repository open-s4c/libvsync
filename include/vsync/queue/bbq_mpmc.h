/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BBQ_MPMC_H
#define VSYNC_BBQ_MPMC_H
/*******************************************************************************
 * @file bbq_mpmc.h
 * @brief Block-based Bounded Queue multi-producer/multi-consumer
 *
 * A highly performant bounded queue that splits the buffer in multiple blocks.
 *
 * ### Remarks:
 *
 * In this implementations, values have the fixed size (pointer size).
 * This implementation does not support DROP_OLD mode as described in the
 * original paper.
 *
 * @cite [BBQ: A Block-based Bounded Queue for Exchanging Data and
 * Profiling](https://www.usenix.org/conference/atc22/presentation/wang-jiawei)
 *
 * @example
 * @include eg_bbq_mpmc.c
 *
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <vsync/common/assert.h>
#include <vsync/utils/string.h>

/**
 * @def BBQ_BLOCK_NUM_LOG
 *
 * Define this macro with `-DBBQ_BLOCK_NUM_LOG=N`
 * to define the total number of blocks equals to  `2^N`
 *
 * @note default value is `3U` -> (`8` blocks)
 *
 */
#ifndef BBQ_BLOCK_NUM_LOG
    #define BBQ_BLOCK_NUM_LOG 3U
#endif

/**
 * @def BBQ_ENTRY_SIZE_LOG
 *
 * Define this macro with `-BBQ_ENTRY_SIZE_LOG=N` to
 * define an entry size equals to `2^N`
 *
 * @note default value is `log2(sizeof(vuintptr_t))`
 *
 */
#ifndef BBQ_ENTRY_SIZE_LOG
    #define BBQ_ENTRY_SIZE_LOG v_log2(sizeof(vuintptr_t))
#endif

#include <vsync/queue/internal/bbq/common.h>

typedef struct bbq_mpmc_block_s {
    vatomic64_t allocated VSYNC_CACHEALIGN;
    vatomic64_t committed VSYNC_CACHEALIGN;
    vatomic64_t reserved VSYNC_CACHEALIGN;
    vatomic64_t consumed VSYNC_CACHEALIGN;
    vuint8_t entry[] VSYNC_CACHEALIGN;
} bbq_mpmc_block_t;

typedef struct bbq_mpmc_s {
    bbq_config_t config VSYNC_CACHEALIGN;
    vatomic64_t widx VSYNC_CACHEALIGN;
    vatomic64_t ridx VSYNC_CACHEALIGN;
    vuint8_t blk[] VSYNC_CACHEALIGN;
} bbq_mpmc_t;

/* block cursor init value */
#define BBQ_BLOCK_MPMC_INIT_VALUE BBQ_BLOCK_INIT_VALUE(bbq_mpmc_block_t)

/* Note:The following macros are used inside bbq/common.h in BBQ_COUNT
 * definition */
#define BBQ_MPMC_WRITE_PROD(k, v) (vatomic64_write(&(k), v))
#define BBQ_MPMC_WRITE_CONS(k, v) (vatomic64_write(&(k), v))
#define BBQ_MPMC_READ_PROD(k)     (vatomic64_read(&(k)))
#define BBQ_MPMC_READ_CONS(k)     (vatomic64_read(&(k)))
#define BBQ_MPMC_COUNT(q)         BBQ_COUNT(q, mpmc, MPMC)

static inline vbool_t _bbq_mpmc_enqueue(struct bbq_mpmc_s *q, vuintptr_t **buf,
                                        vuint32_t *count);
static inline vbool_t _bbq_mpmc_dequeue(bbq_mpmc_t *q, vuintptr_t **buf,
                                        vuint32_t *count);
static inline void _bbq_mpmc_block_init(bbq_mpmc_block_t *blk, vsize_t idx,
                                        vuint16_t block_size);

/**
 * Enqueues one or more entries.
 *
 * Multiple entries can be enqueued if `src` points to an array. Use `count` to
 * indicate how many entries should be enqueueed, starting from `src`.
 *
 * @param q     pointer to bbq_mpmc data structure
 * @param src   pointer to first entry
 * @param count number of entries to enqueue
 * @param wait  should wait for space to be available
 *
 * @return number of enqueued entries
 */
static inline vuint32_t
bbq_mpmc_enqueue(bbq_mpmc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
{
    vuint32_t rest       = count;
    vuintptr_t *rest_buf = buf;
    vbool_t retry;

    /* the following is equivalent to
     * while(bbq_mpmc_enqueue_internal(q, &rest_buf, &rest) || (wait && rest));
     */
    do {
        retry = _bbq_mpmc_enqueue(q, &rest_buf, &rest);

        /* Help model checker in case the queue is empty
         * The condition to leave the loop is
         *   retry' == true or rest' == 0
         *   rest' == 0 => rest # 0 /\ retry' = true
         *
         * It is sufficient to observe retry only
         *
         */
        await_while (!retry && wait && rest)
            retry = _bbq_mpmc_enqueue(q, &rest_buf, &rest);
    } while (retry);

    return count - rest;
}

/**
 * Dequeues one or more entries.
 *
 * Multiple entries can be dequeued if `src` points to an array. Use `count` to
 * indicate how many entries should be dequeued.
 *
 * @param q     pointer to bbq_mpmc data structure
 * @param src   pointer to preallocated memory for the first entry
 * @param count number of entries to dequeue
 * @param wait  should wait for entries to be available
 *
 * @return number of dequeued entries
 */
static inline vuint32_t
bbq_mpmc_dequeue(bbq_mpmc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
{
    vuint32_t rest       = count;
    vuintptr_t *rest_buf = buf;
    vbool_t retry;

    /* the following is equivalent to
     * while(bbq_mpmc_dequeue_internal(q, &rest_buf, &rest) || (wait && rest));
     */
    do {
        retry = _bbq_mpmc_dequeue(q, &rest_buf, &rest);

        /* Help model checker in case the queue is empty
         * The condition to leave the loop is
         *   retry' == true or rest' == 0
         *   rest' == 0 => rest # 0 /\ retry' = true
         *
         * It is sufficient to observe retry only
         *
         */
        await_while (!retry && wait && rest)
            retry = _bbq_mpmc_dequeue(q, &rest_buf, &rest);
    } while (retry);

    return count - rest;
}

/**
 * Calculates the size of the bbq queue.
 *
 * @param capacity maximum number of entries that can fit in the queue
 * @return size to be allocated in bytes
 */
static inline vsize_t
bbq_mpmc_memsize(vsize_t capacity)
{
    vsize_t cnt_each_blk = (capacity) >> BBQ_BLOCK_NUM_LOG;
    if (cnt_each_blk == 0) {
        cnt_each_blk = 1;
    }
    vsize_t mem_each_blk =
        sizeof(bbq_mpmc_block_t) + (cnt_each_blk << BBQ_ENTRY_SIZE_LOG);
    vsize_t mem_each_blk_log =
        v_pow2_round_down(mem_each_blk * 2 - 1); /* align up */
    vsize_t mem_buf =
        sizeof(bbq_mpmc_t) + (mem_each_blk_log << BBQ_BLOCK_NUM_LOG);
    return mem_buf;
}
/**
 * Initializes a bbq data structure.
 *
 * @param buf pointer to bbq data structure
 * @param size number of bytes allocated for bbq data structure
 * @return true initialization succeeded.
 * @return false initialization failed.
 */
static inline vbool_t
bbq_mpmc_init(bbq_mpmc_t *rb, vsize_t size)
{
    if (unlikely(rb == NULL) || unlikely(BBQ_ENTRY_SIZE < BBQ_MIN_ENTRY_SIZE) ||
        unlikely(BBQ_ENTRY_SIZE > BBQ_MAX_ENTRY_SIZE) ||
        unlikely(BBQ_BLOCK_NUM_LOG < BBQ_MIN_BLOCK_NUM_LOG) ||
        unlikely(BBQ_BLOCK_NUM_LOG > BBQ_MAX_BLOCK_NUM_LOG)) {
        return false;
    }

    vsize_t blks_total_size = (size) - sizeof(bbq_mpmc_t);
    vsize_t blk_size = v_pow2_round_down(blks_total_size >> BBQ_BLOCK_NUM_LOG);
    if (unlikely(blk_size <= BBQ_BLOCK_INIT_VALUE(bbq_mpmc_block_t))) {
        return false;
    }
    vsize_t blk_size_log = v_log2(blk_size);
    BBQ_BLK_SZ_VERIFICATION(mpmc);
    if (unlikely(blk_size_log < BBQ_MIN_BLOCK_SIZE_LOG) ||
        unlikely(blk_size_log >= BBQ_MAX_BLOCK_SIZE_LOG)) {
        return false;
    }
    (rb)->config.blk_size     = blk_size;
    (rb)->config.blk_size_log = blk_size_log;
    BBQ_MPMC_WRITE_PROD((rb)->widx, 0);
    BBQ_MPMC_WRITE_CONS((rb)->ridx, 0);
    for (vsize_t i = 0; i < (1UL << BBQ_BLOCK_NUM_LOG); i++) {
        _bbq_mpmc_block_init(
            (bbq_mpmc_block_t *)((rb)->blk + (i << blk_size_log)), i, blk_size);
    }
    return true;
}

static inline void
_bbq_mpmc_block_init(bbq_mpmc_block_t *blk, vsize_t idx, vuint16_t block_size)
{
    /* if it is not the first block, set to invalid state */
    vuint16_t init_value = likely(idx) ? block_size : BBQ_BLOCK_MPMC_INIT_VALUE;
    vatomic64_write(&blk->allocated, init_value);
    vatomic64_write(&blk->committed, init_value);
    vatomic64_write(&blk->reserved, init_value);
    vatomic64_write(&blk->consumed, init_value);
}

/* return means retry */
static inline vbool_t
_bbq_mpmc_enqueue(bbq_mpmc_t *q, vuintptr_t **buf, vuint32_t *count)
{
    if (*count == 0) {
        return false;
    }
    /* get the address of the alloc block */
    vuint64_t widx        = vatomic64_read(&q->widx);
    vuint16_t block_idx   = BBQ_GLOBAL_IDX(widx);
    bbq_mpmc_block_t *blk = BBQ_GET_BLOCK(q, block_idx);
    /* precheck once */
    vuint16_t block_size      = q->config.blk_size;
    vuint64_t allocated       = vatomic64_read(&blk->allocated);
    vuint64_t allocated_space = BBQ_LOCAL_IDX(allocated);
    vsize_t entry_total_size  = (*count) << BBQ_ENTRY_SIZE_LOG;
    /* if out of bound, we don't add the space, but help to move the block */
    if (likely(allocated_space < block_size)) {
        /* update the allocated index using FAA */
        vuint64_t old_allocated =
            vatomic64_get_add(&blk->allocated, entry_total_size);
        /* we have some space */
        vuint64_t old_local_space = BBQ_LOCAL_IDX(old_allocated);
        if (likely(old_local_space < block_size)) {
            vuint16_t space =
                VMIN(entry_total_size, block_size - old_local_space);
            void *entry = BBQ_GET_ENTRY(blk, old_local_space);
            int r       = memcpy_s(entry, space, *buf, space);
            BUG_ON(r != 0);
            vatomic64_add(&blk->committed, space);
            vuint16_t offset = space >> BBQ_ENTRY_SIZE_LOG;
            *buf += offset;
            *count -= offset;
            return true;
        }
    }
    /* slow path, all writers help to move to next block */
    bbq_mpmc_block_t *nblk = BBQ_GET_NEXT_BLOCK(q, block_idx);
    vuint64_t global_vsn   = BBQ_GLOBAL_VSN(widx);
    if (unlikely(
            !BBQ_BLOCK_FULLY_CONSUMED_WITH_VSN(nblk, block_size, global_vsn))) {
        return false;
    }
    /* reset cursor and advance block */
    bbq_reset_block_cursor_heavy(&nblk->committed, global_vsn + 1,
                                 BBQ_BLOCK_MPMC_INIT_VALUE);
    bbq_reset_block_cursor_heavy(&nblk->allocated, global_vsn + 1,
                                 BBQ_BLOCK_MPMC_INIT_VALUE);
    BBQ_ADVANCE_HEAD(&q->widx, widx, widx + 1);
    return true;
}
/* return means retry */
static inline vbool_t
_bbq_mpmc_dequeue(bbq_mpmc_t *q, vuintptr_t **buf, vuint32_t *count)
{
    if (*count == 0) {
        return false;
    }
    /* get the address of the occupy block */
    vuint64_t ridx        = vatomic64_read(&q->ridx);
    vuint16_t block_idx   = BBQ_GLOBAL_IDX(ridx);
    bbq_mpmc_block_t *blk = BBQ_GET_BLOCK(q, block_idx);
    /* check if the block is fully reserved */
    vuint16_t block_size     = q->config.blk_size;
    vuint64_t reserved       = vatomic64_read(&blk->reserved);
    vuint64_t reserved_space = BBQ_LOCAL_IDX(reserved);
    if (likely(reserved_space < block_size)) {
        vuint64_t committed = vatomic64_read(&blk->committed);
        /* check if we have an entry to occupy */
        vuint64_t committed_space = BBQ_LOCAL_IDX(committed);
        if (unlikely(reserved_space >= committed_space)) {
            /* Note: the version is strictly monotic and may not wrap. */
            ASSERT(reserved <= committed && "reserved must be <= committed");
            return false;
        }
        vuint16_t entry_total_size = VMIN((*count) << BBQ_ENTRY_SIZE_LOG,
                                          committed_space - reserved_space);
        if (unlikely(committed_space != block_size)) {
            vuint64_t allocated       = vatomic64_read(&blk->allocated);
            vuint64_t allocated_space = BBQ_LOCAL_IDX(allocated);
            if (likely(allocated_space != committed_space)) {
                return false;
            }
        }
        if (vatomic64_cmpxchg(&blk->reserved, reserved,
                              reserved + entry_total_size) != reserved) {
            return true;
        }
        /* we got the entry */
        void *entry = BBQ_GET_ENTRY(blk, BBQ_LOCAL_IDX(reserved));
        int r       = memcpy_s(*buf, entry_total_size, entry, entry_total_size);
        BUG_ON(r != 0);
        /* consume after copy the data back */
        vatomic64_add(&blk->consumed, entry_total_size);
        vuint16_t offset = entry_total_size >> BBQ_ENTRY_SIZE_LOG;
        *buf += offset;
        *count -= offset;
        return true;
    }
    /* need to advance the block */
    bbq_mpmc_block_t *nblk = BBQ_GET_NEXT_BLOCK(q, block_idx);
    /* r_head never pass the w_head and r_tail */
    vuint64_t next_consumer_vsn = BBQ_LOCAL_VSN(reserved) - (block_idx != 0);
    vuint64_t next_producer_vsn =
        BBQ_LOCAL_VSN(vatomic64_read(&nblk->committed));
    if (next_producer_vsn != next_consumer_vsn + 1) {
        return false;
    }
    /* reset the cursor */
    bbq_reset_block_cursor_heavy(&nblk->consumed, next_consumer_vsn + 1,
                                 BBQ_BLOCK_MPMC_INIT_VALUE);
    bbq_reset_block_cursor_heavy(&nblk->reserved, next_consumer_vsn + 1,
                                 BBQ_BLOCK_MPMC_INIT_VALUE);
    BBQ_ADVANCE_HEAD(&q->ridx, ridx, ridx + 1);
    return true;
}
#endif
