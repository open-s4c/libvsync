/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_SEQCOUNT_H
#define VSYNC_SEQCOUNT_H
/*******************************************************************************
 * @file  seqcount.h
 * @brief Lightweight single-writer multi-reader optimistic lock.
 *
 * A seqcount allows a writer to modify shared variables without being blocked
 * by readers. Readers optimistically read the shared variables and subsequently
 * check their consistency. If the shared variables were read while a write
 * occurred, the readers must retry.
 *
 * @note this algorithm is suitable for one writer, multiple readers. See
 * vsync/spinlock/seqlock.h for multiple writer support.
 *
 * @example
 * @include eg_seqcount.c
 *
 * @cite [The seqcount latch lock type](https://lwn.net/Articles/831540/)
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/vtypes.h>

typedef vatomic32_t seqcount_t;
typedef vuint32_t seqvalue_t;

#define SEQCOUNT_STEP 2U

/** Initializer of `seqcount_t`. */
#define SEQCOUNT_INIT() VATOMIC_INIT(0)

/**
 * Initializes seqcount.
 *
 * @param sc address of seqcount_t object.
 *
 * @note alternatively use `SEQCOUNT_INIT`.
 */
static inline void
seqcount_init(seqcount_t *sc)
{
    vatomic32_init(sc, 0);
}
/**
 * Begins writer critical section.
 *
 * This function can only be called by a single thread. If multiple writers are
 * necessary, the user has to protect this call with an additional lock.
 * Alternatively vsync/spinlock/seqlock.h can be used instead.
 *
 * @param sc address of seqcount_t object.
 * @return sequence value.
 *
 * @note Can be called by a single thread.
 */
static inline seqvalue_t
seqcount_wbegin(seqcount_t *sc)
{
    seqvalue_t s = vatomic32_read_rlx(sc);
    vatomic32_write_rlx(sc, s + 1U);
    vatomic_fence_rel();
    return s;
}
/**
 * Ends writer critical section.
 *
 * @param sc address of seqcount_t object.
 * @param s sequence value returned by `seqcount_wbegin`.
 */
static inline void
seqcount_wend(seqcount_t *sc, seqvalue_t s)
{
    vatomic32_write_rel(sc, s + SEQCOUNT_STEP);
}
/**
 * Begins reader critical section.
 *
 * @note This function can be called by multiple reader threads.
 *
 * @param sc address of seqcount_t object.
 * @return sequence value.
 */
static inline seqvalue_t
seqcount_rbegin(seqcount_t *sc)
{
    seqvalue_t s = vatomic32_read_acq(sc) & ~1U;
    return s;
}
/**
 * Tries to end the reader critical section.
 *
 * @param sc address of seqcount_t object.
 * @param s sequence value returned by `seqcount_rbegin`.
 * @return true, reader CS can be ended, read data is consistent.
 * @return false, a write took place, data is inconsistent and reader need to
 * repeat the CS.
 */
static inline vbool_t
seqcount_rend(seqcount_t *sc, seqvalue_t s)
{
    vatomic_fence_acq();
    return vatomic32_read_rlx(sc) == s;
}
#undef SEQCOUNT_STEP
#endif
