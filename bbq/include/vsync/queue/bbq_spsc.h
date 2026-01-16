/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2020-2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_BBQ_SPSC_H
#define VSYNC_BBQ_SPSC_H

/*******************************************************************************
 * @file bbq_spsc.h
 * @brief Block-based Bounded Queue single-producer/single-consumer
 * @ingroup linearizable lock_free
 *
 * A highly performant bounded queue that splits the buffer in multiple blocks.
 *
 *
 * ### Remarks:
 *
 * In this implementations, values have a fixed size equal to pointer size.
 * This implementation does not support `DROP_OLD` mode as described in the
 * original paper.
 *
 * @cite [BBQ: A Block-based Bounded Queue for Exchanging Data and
 * Profiling](https://www.usenix.org/conference/atc22/presentation/wang-jiawei)
 *
 * @example
 * ### Multi-threaded
 * @include eg_bbq_spsc.c
 * ### Multi-process
 * @include eg_bbq_spsc_m_proc.c
 *
 ******************************************************************************/

#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <vsync/common/dbg.h>
#include <vsync/common/assert.h>
#include <vsync/utils/string.h>
#include <vsync/utils/math.h>

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

typedef struct bbq_spsc_block_s {
    vatomic64_t committed VSYNC_CACHEALIGN;
    vatomic64_t consumed VSYNC_CACHEALIGN;
    vuint8_t entry[] VSYNC_CACHEALIGN;
} bbq_spsc_block_t;

typedef struct bbq_spsc_s {
    bbq_config_t config VSYNC_CACHEALIGN;
    vuint64_t widx VSYNC_CACHEALIGN;
    vuint64_t ridx VSYNC_CACHEALIGN;
    vuint8_t blk[] VSYNC_CACHEALIGN;
} bbq_spsc_t;

/* block cursor init value */
#define BBQ_BLOCK_SPSC_INIT_VALUE BBQ_BLOCK_INIT_VALUE(struct bbq_spsc_block_s)
/* Note:The following macros are used inside bbq/common.h in BBQ_COUNT
 * definition */
#define BBQ_SPSC_WRITE_PROD(k, v) ((k) = v)
#define BBQ_SPSC_WRITE_CONS(k, v) ((k) = v)
#define BBQ_SPSC_READ_PROD(k)     (k)
#define BBQ_SPSC_READ_CONS(k)     (k)
#define BBQ_SPSC_COUNT(q)         BBQ_COUNT(q, spsc, SPSC)

/* prototypes of internal functions */
static inline vbool_t _bbq_spsc_enqueue(bbq_spsc_t *q, vuintptr_t **buf,
                                        vuint32_t *count);
static inline vbool_t _bbq_spsc_dequeue(bbq_spsc_t *q, vuintptr_t **buf,
                                        vuint32_t *count);
static inline void _bbq_spsc_block_init(bbq_spsc_block_t *blk, vsize_t idx,
                                        vuint16_t block_size);
/**
 * Enqueues one or more entries.
 *
 * Multiple entries can be enqueued if `src` points to an array. Use `count` to
 * indicate how many entries should be enqueueed, starting from `src`.
 *
 * @param q     address of bbq_spsc_t object.
 * @param buf   address of the first entry.
 * @param count number of entries to enqueue.
 * @param wait  true/false when set to true it waits (blocks) till space becomes
 * available. Otherwise, it quits retrying.
 *
 * @return number of enqueued entries.
 */
static inline vuint32_t
bbq_spsc_enqueue(bbq_spsc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
{
    vuint32_t rest       = count;
    vuintptr_t *rest_buf = buf;
    vbool_t retry;

    /* The following two are equivalent.
     * Which one is better depends on the model checker.
     */
#if defined(VSYNC_VERIFICATION_DAT3M)
    while (_bbq_spsc_enqueue(q, &rest_buf, &rest) || (wait && rest))
        ;
#else
    do {
        retry = _bbq_spsc_enqueue(q, &rest_buf, &rest);

        /* The condition to leave the loop is
         *   retry' == true or rest' == 0
         *   rest' == 0 => rest # 0 /\ retry' = true
         *
         * It is sufficient to observe retry only
         *
         */
        await_while (!retry && wait && rest) {
            retry = _bbq_spsc_enqueue(q, &rest_buf, &rest);
        }
    } while (retry);
#endif

    return count - rest;
}

/**
 * Dequeues one or more entries.
 *
 * Multiple entries can be dequeued if `buf` points to an array. Use `count` to
 * indicate how many entries should be dequeued.
 *
 * @param q     address of bbq_spsc_t object.
 * @param buf   address of the first entry of the preallocated memory.
 * @param count number of entries to dequeue.
 * @param wait  true/false. When set to true the API waits/blocks for entries to
 * be available
 *
 * @return number of dequeued entries.
 */
static inline vuint32_t
bbq_spsc_dequeue(bbq_spsc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
{
    vuint32_t rest       = count;
    vuintptr_t *rest_buf = buf;
    vbool_t retry;

    /* The following two are equivalent.
     * Which one is better depends on the model checker.
     */
#if defined(VSYNC_VERIFICATION_DAT3M)
    while (_bbq_spsc_dequeue(q, &rest_buf, &rest) || (wait && rest))
        ;
#else
    do {
        retry = _bbq_spsc_dequeue(q, &rest_buf, &rest);

        /* The condition to leave the loop is
         *   retry' == true or rest' == 0
         *   rest' == 0 => rest # 0 /\ retry' = true
         *
         * It is sufficient to observe retry only
         *
         */
        await_while (!retry && wait && rest) {
            retry = _bbq_spsc_dequeue(q, &rest_buf, &rest);
        }
    } while (retry);
#endif

    return count - rest;
}
/**
 * Calculates the size of bbq_spsc_t object based on the given capacity.
 *
 * @param capacity maximum number of entries that can fit in the queue.
 * @return size of bbq_spsc_t object with the given capacity.
 */
static inline vsize_t
bbq_spsc_memsize(vsize_t capacity)
{
    vsize_t cnt_each_blk = (capacity) >> BBQ_BLOCK_NUM_LOG;
    if (cnt_each_blk == 0) {
        cnt_each_blk = 1;
    }
    vsize_t mem_each_blk =
        sizeof(bbq_spsc_block_t) + (cnt_each_blk << BBQ_ENTRY_SIZE_LOG);

    vsize_t mem_each_blk_log =
        v_pow2_round_down(mem_each_blk * 2 - 1); /* align up */

    vsize_t mem_buf =
        sizeof(bbq_spsc_t) + (mem_each_blk_log << BBQ_BLOCK_NUM_LOG);

    return mem_buf;
}

/**
 * Initializes a bbq data structure.
 *
 * @param rb address of bbq_spsc_t object.
 * @param size size of the given bbq_spsc_t object `rb`.
 *
 * @return true initialization succeeded.
 * @return false initialization failed.
 */
static inline vbool_t
bbq_spsc_init(bbq_spsc_t *rb, vsize_t size)
{
    // we shift vuint16_t by BBQ_ENTRY_SIZE_LOG, we need to make sure the
    // behavior is defined
    ASSERT(
        BBQ_ENTRY_SIZE_LOG < 16U &&
        "must have width less than vuint16_t because to be able to shift it");

    if (unlikely(rb == NULL) || unlikely(BBQ_ENTRY_SIZE < BBQ_MIN_ENTRY_SIZE) ||
        unlikely(BBQ_ENTRY_SIZE > BBQ_MAX_ENTRY_SIZE) ||
        unlikely(BBQ_BLOCK_NUM_LOG < BBQ_MIN_BLOCK_NUM_LOG) ||
        unlikely(BBQ_BLOCK_NUM_LOG > BBQ_MAX_BLOCK_NUM_LOG)) {
        return false;
    }

    vsize_t blks_total_size = (size) - sizeof(bbq_spsc_t);
    vsize_t blk_size = v_pow2_round_down(blks_total_size >> BBQ_BLOCK_NUM_LOG);
    if (unlikely(blk_size <= BBQ_BLOCK_INIT_VALUE(struct bbq_spsc_block_s))) {
        return false;
    }
    vsize_t blk_size_log = v_log2(blk_size);
    BBQ_BLK_SZ_VERIFICATION(spsc);
    if (unlikely(blk_size_log < BBQ_MIN_BLOCK_SIZE_LOG) ||
        unlikely(blk_size_log >= BBQ_MAX_BLOCK_SIZE_LOG)) {
        return false;
    }
    (rb)->config.blk_size     = blk_size;
    (rb)->config.blk_size_log = blk_size_log;
    BBQ_SPSC_WRITE_PROD((rb)->widx, 0);
    BBQ_SPSC_WRITE_CONS((rb)->ridx, 0);

    for (vsize_t i = 0; i < (1UL << BBQ_BLOCK_NUM_LOG); i++) {
        _bbq_spsc_block_init(
            (bbq_spsc_block_t *)((rb)->blk + (i << blk_size_log)), i, blk_size);
    }
    return true;
}

static inline void
_bbq_spsc_block_init(bbq_spsc_block_t *blk, vsize_t idx, vuint16_t block_size)
{
    /* if it is not the first block, set to invalid state */
    vuint16_t init_value = likely(idx) ? block_size : BBQ_BLOCK_SPSC_INIT_VALUE;
    vatomic64_write(&blk->committed, init_value);
    vatomic64_write(&blk->consumed, init_value);
}

/* return means retry */
static inline vbool_t
_bbq_spsc_enqueue(bbq_spsc_t *q, vuintptr_t **buf, vuint32_t *count)
{
    if (*count == 0) {
        return false;
    }

    /* get the address of the alloc block */
    vuint64_t widx        = q->widx;
    vuint16_t block_idx   = BBQ_GLOBAL_IDX(widx);
    bbq_spsc_block_t *blk = BBQ_GET_BLOCK(q, block_idx);
    /* precheck once */
    vuint16_t block_size      = q->config.blk_size;
    vuint64_t committed       = vatomic64_read_rlx(&blk->committed);
    vuint64_t committed_space = BBQ_LOCAL_IDX(committed);
    vsize_t entry_total_size  = (*count) << BBQ_ENTRY_SIZE_LOG;
    /* if out of bound, we don't add the space, but help to move the block */
    if (likely(committed_space < block_size)) {
        vuint16_t space = VMIN(entry_total_size, block_size - committed_space);
        void *entry     = BBQ_GET_ENTRY(blk, committed_space);
        int r           = memcpy_s(entry, space, *buf, space);
        BUG_ON(r != 0);
        vuint64_t new_committed = BBQ_LOCAL_COMPOSE(BBQ_LOCAL_VSN(committed),
                                                    committed_space + space);
        vatomic64_write_rel(&blk->committed, new_committed);
        vuint16_t offset = space >> BBQ_ENTRY_SIZE_LOG;
        *buf += offset;
        *count -= offset;
        return true;
    }

    /* slow path, all writers help to move to next block */
    bbq_spsc_block_t *nblk = BBQ_GET_NEXT_BLOCK(q, block_idx);
    vuint64_t global_vsn   = BBQ_GLOBAL_VSN(widx);
    if (unlikely(
            !BBQ_BLOCK_FULLY_CONSUMED_WITH_VSN(nblk, block_size, global_vsn))) {
        return false;
    }

    /* reset cursor and advance block */
    BBQ_RESET_BLOCK_CURSOR_LIGHT(&nblk->committed, global_vsn + 1,
                                 BBQ_BLOCK_SPSC_INIT_VALUE);
    q->widx++;
    return true;
}

/* return means retry */
static inline vbool_t
_bbq_spsc_dequeue(bbq_spsc_t *q, vuintptr_t **buf, vuint32_t *count)
{
    if (*count == 0) {
        return false;
    }

    /* get the address of the occupy block */
    vuint64_t ridx        = q->ridx;
    vuint16_t block_idx   = BBQ_GLOBAL_IDX(ridx);
    bbq_spsc_block_t *blk = BBQ_GET_BLOCK(q, block_idx);

    /* check if the block is fully reserved */
    vuint16_t block_size     = q->config.blk_size;
    vuint64_t consumed       = vatomic64_read_rlx(&blk->consumed);
    vuint64_t consumed_space = BBQ_LOCAL_IDX(consumed);
    if (likely(consumed_space < block_size)) {
        vuint64_t committed = vatomic64_read_acq(&blk->committed);
        /* check if we have an entry to occupy */
        vuint64_t committed_space = BBQ_LOCAL_IDX(committed);
        if (unlikely(consumed_space >= committed_space)) {
            ASSERT(consumed_space == committed_space &&
                   "Consumed should be <= committed");
            return false;
        }

        /* we got the entry */
        vuint16_t space = VMIN((*count) << BBQ_ENTRY_SIZE_LOG,
                               committed_space - consumed_space);
        void *entry     = BBQ_GET_ENTRY(blk, consumed_space);
        int r           = memcpy_s(*buf, space, entry, space);
        BUG_ON(r != 0);
        vuint64_t new_consumed = consumed + space;
        vatomic64_write_rel(&blk->consumed, new_consumed);
        vuint16_t offset = space >> BBQ_ENTRY_SIZE_LOG;
        *buf += offset;
        *count -= offset;
        return true;
    }

    /* need to advance the block */
    bbq_spsc_block_t *nblk = BBQ_GET_NEXT_BLOCK(q, block_idx);
    vuint64_t global_vsn   = BBQ_GLOBAL_VSN(ridx);

    /* r_head never pass the w_head and r_tail */
    vuint64_t next_block_vsn =
        BBQ_LOCAL_VSN(vatomic64_read_rlx(&nblk->committed));
    if (unlikely(next_block_vsn != global_vsn + 1)) {
        return false;
    }
    /* reset the cursor */
    BBQ_RESET_BLOCK_CURSOR_LIGHT(&nblk->consumed, global_vsn + 1,
                                 BBQ_BLOCK_SPSC_INIT_VALUE);
    q->ridx++;
    return true;
}
#endif
