/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_PRIO_RENTRANT_LOCK_H
#define VQUEUE_PRIO_RENTRANT_LOCK_H

#ifdef VSYNC_VERIFICATION
    #include <vsync/atomic.h>
    #define REC_LOCK_NUM_BUCKETS 4

    /* @TODO: this is duplicated in recursive_lock.h - refactor into common
     * include!!! */
    /** Invalid CPU ID used to represent a released lock. */
    #define RECURSIVE_LOCK_NOCPU VUINT32_MAX

typedef struct rec_spinlock_s {
    vatomic8_t lock;
    vatomic32_t owner[REC_LOCK_NUM_BUCKETS];
    uint32_t count;
} rec_spinlock_t;

/**
 * Initializes the recursive spinlock
 *
 * @param l address of recursive spinlock lock object
 */
static inline void
rec_spinlock_init(struct rec_spinlock_s *l)
{
    vatomic8_write(&l->lock, 0);
    l->count = 0;
    for (vsize_t i = 0; i < REC_LOCK_NUM_BUCKETS; i++) {
        vatomic32_init(&l->owner[i], RECURSIVE_LOCK_NOCPU);
    }
}

__thread vuint32_t last_acq_id;
/**
 * Tries to acquire the recursive spinlock
 *
 * @param l address of recursive spinlock lock object
 * @param id thread ID or core ID
 * @return 1 on success, 0 on failure
 */
static inline int
rec_spinlock_tryacquire(struct rec_spinlock_s *l, vuint32_t id)
{
    vatomic32_t *p_owner = &l->owner[id % REC_LOCK_NUM_BUCKETS];
    vuint32_t owner      = vatomic32_read_rlx(p_owner);

    last_acq_id = id;
    ASSERT(id != RECURSIVE_LOCK_NOCPU && "this value is reserved");

    if (owner == id) {
        l->count++;
        return 1;
    }

    if (owner != RECURSIVE_LOCK_NOCPU)
        return 0;

    if (vatomic8_cmpxchg(&l->lock, 0, 1) != 0)
        return 0;

    vatomic32_write_rlx(p_owner, id);
    return 1;
}

/**
 * Acquires the recursive spinlock
 *
 * @param l  address of recursive spinlock lock object
 * @param id thread ID or core ID
 */
static inline void
rec_spinlock_acquire(struct rec_spinlock_s *l, vuint32_t id)
{
    last_acq_id          = id;
    vatomic32_t *p_owner = &l->owner[id % REC_LOCK_NUM_BUCKETS];
    ASSERT(id != RECURSIVE_LOCK_NOCPU && "this value is reserved");
    if (vatomic32_read_rlx(p_owner) == id) {
        l->count++;
        return;
    }

    await_while (vatomic8_cmpxchg(&l->lock, 0, 1) != 0)
        ;
    vatomic32_write_rlx(p_owner, id);
}

/**
 * Releases the recursive spinlock
 *
 * @param l address of recursive spinlock lock object
 */
static inline void
rec_spinlock_release(struct rec_spinlock_s *l)
{
    vuint32_t id         = last_acq_id;
    vatomic32_t *p_owner = &l->owner[id % REC_LOCK_NUM_BUCKETS];
    if (l->count == 0) {
        vatomic32_write_rlx(p_owner, RECURSIVE_LOCK_NOCPU);
        vatomic8_write(&l->lock, 0);
    } else {
        l->count--;
    }
}
    #undef REC_LOCK_NUM_BUCKETS
#else
    #include <vsync/spinlock/rec_spinlock.h>
#endif

typedef rec_spinlock_t vqueue_prio_lock_t;

static inline void
vqueue_prio_lock_init(vqueue_prio_lock_t *lock)
{
    rec_spinlock_init(lock);
}

static inline void
vqueue_prio_lock_acquire(vqueue_prio_lock_t *lock, vuint32_t tid)
{
    rec_spinlock_acquire(lock, tid);
}

static inline void
vqueue_prio_lock_release(vqueue_prio_lock_t *lock, vuint32_t tid)
{
    V_UNUSED(tid);
    rec_spinlock_release(lock);
}

static inline void
vqueue_prio_lock_destroy(vqueue_prio_lock_t *lock)
{
    V_UNUSED(lock);
}
#endif
