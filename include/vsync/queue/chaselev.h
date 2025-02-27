/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VDEQUE_H
#define VDEQUE_H
/*******************************************************************************
 * @file chaselev.h
 * @brief Chase-Lev Work-Stealing bounded deque.
 *
 * @ingroup lock_free
 *
 * The deque has a bounded size and returns errors in case the deque is
 * full, empty, or if there is contention while trying to pop/steal.
 *
 *
 * @cite
 * David Chase, Yossi Lev - [Dynamic Circular Work-Stealing Deque]
 * (https://dl.acm.org/doi/pdf/10.1145/1073970.1073974)
 *
 * @example
 * @include eg_chaselev.c
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/assert.h>

typedef struct vdeque_array_s {
    vatomicsz_t size;
    vatomicptr_t buffer[];
} vdeque_array_t;

typedef struct vdeque_s {
    vatomicsz_t top;
    vatomicsz_t bottom;
    vatomicptr(struct vdeque_array_t *)
        array; /* Current implementation provides atomic array pointer in order
                  to accommodate growth of the array buffer in the future. Note:
                  this will cause for some of the barriers proposed in this
                  implementation to be changed, once growth is supported. */
} vdeque_t;

typedef enum vdeque_state_e {
    VDEQUE_STATE_OK,
    VDEQUE_STATE_EMPTY,
    VDEQUE_STATE_FULL,
    VDEQUE_STATE_ABORT
} vdeque_state_t;

/**
 * Initializes chase-lev deque.
 *
 * @param q address of vdeque_t object.
 * @param a address of vdeque_array_t object.
 * @param len length of array `a`.
 */
static inline void
vdeque_init(vdeque_t *q, vdeque_array_t *a, vsize_t len)
{
    ASSERT(q && "deque is NULL");
    ASSERT(a && "array is NULL");
    ASSERT(len != 0 && "array with 0 size");

    vatomicsz_init(&a->size, len);
    vatomicptr_init(&q->array, a);
    vatomicsz_init(&q->top, 0);
    vatomicsz_init(&q->bottom, 0);

    /* initialize the array */
    for (vsize_t i = 0; i < len; i++) {
        vatomicptr_init(&a->buffer[i], NULL);
    }
}
/**
 * Returns array struct memory size.
 *
 * @param cap capacity of buffer.
 * @return size of the array struct with a buffer of capacity `cap`.
 */
static inline vsize_t
vdeque_memsize(vsize_t cap)
{
    return sizeof(vdeque_array_t) + (cap * sizeof(vatomicptr_t));
}
/**
 * Tries to push a value to the bottom.
 *
 * @param q address of vdeque_t object.
 * @param in_obj address of object to push.
 *
 * @return VDEQUE_STATE_OK if successful.
 * @return VDEQUE_STATE_FULL if deque is full.
 */
static inline vdeque_state_t
vdeque_push_bottom(vdeque_t *q, void *in_obj)
{
    ASSERT(q);

    vsize_t bottom    = vatomicsz_read_rlx(&q->bottom);
    vsize_t top       = vatomicsz_read_acq(&q->top);
    vdeque_array_t *a = (vdeque_array_t *)vatomicptr_read_rlx(&q->array);
    vsize_t size      = vatomicsz_read_rlx(&a->size);

    ASSERT(size != 0);

    vsize_t cmp_size = size - 1U;
    /* Check if deque is full. */
    if ((bottom - top) > cmp_size) {
        return VDEQUE_STATE_FULL;
    }
    vsize_t idx = bottom % size;
    vatomicptr_write_rlx(&a->buffer[idx], in_obj);
    vatomicsz_write_rel(&q->bottom, bottom + 1U);
    return VDEQUE_STATE_OK;
}

/**
 * Tries to pop a value from the bottom.
 *
 * @param q address of vdeque_t object.
 * @param out_obj output parameter address of popped object.
 *
 * @return VDEQUE_STATE_OK if successful.
 * @return VDEQUE_STATE_EMPTY if deque is empty, or if it failed to take the
 * last element.
 */
static inline vdeque_state_t
vdeque_pop_bottom(vdeque_t *q, void **out_obj)
{
    ASSERT(q);
    ASSERT(out_obj);

    vsize_t bottom = vatomicsz_read_rlx(&q->bottom);
    /* The original implementation in the paper does not take into account a
     * scenario where the thread might try to take when the deque is empty and
     * at the same time the bottom index is 0. In this case, because we are
     * using unsigned integers, we do not have proper comparison and the
     * program may enter the second if statement, which violates correctness and
     * reads from uninitialized memory. As a workaround, the below if statement
     * has been added, in order to return immediately when this scenario
     * occurs. */
    if (bottom == 0U) {
        /* Empty deque. Return before modifying bottom. */
        return VDEQUE_STATE_EMPTY;
    }
    bottom            = bottom - 1;
    vdeque_array_t *a = (vdeque_array_t *)vatomicptr_read_rlx(&q->array);
    vatomicsz_write(&q->bottom, bottom);
    vsize_t top          = vatomicsz_read(&q->top);
    vdeque_state_t state = VDEQUE_STATE_OK;
    if (top <= bottom) {
        /* Non-empty deque. */
        vsize_t size = vatomicsz_read_rlx(&a->size);
        vsize_t idx  = bottom % size;
        *out_obj     = vatomicptr_read_rlx(&a->buffer[idx]);
        if (top == bottom) {
            /* Single last element in deque. */
            if (vatomicsz_cmpxchg(&q->top, top, top + 1U) != top) {
                /* Failed race. */
                state = VDEQUE_STATE_EMPTY;
            }
            vatomicsz_write_rlx(&q->bottom, bottom + 1U);
        }
    } else {
        /* Empty deque. */
        state = VDEQUE_STATE_EMPTY;
        vatomicsz_write_rlx(&q->bottom, bottom + 1U);
    }
    return state;
}

/**
 * Tries to (steal) pop a value from the top.
 *
 * @param q address of vdeque_t object.
 * @param out_obj output parameter address of popped object.
 *
 * @return VDEQUE_STATE_OK if successful.
 * @return VDEQUE_STATE_EMPTY if deque is empty.
 * @return VDEQUE_STATE_ABORT if failed to pop because of contention.
 */
static inline vdeque_state_t
vdeque_steal(vdeque_t *q, void **out_obj)
{
    vdeque_state_t state = VDEQUE_STATE_EMPTY;

    ASSERT(q);
    ASSERT(out_obj);

    vsize_t top    = vatomicsz_read(&q->top);
    vsize_t bottom = vatomicsz_read(&q->bottom);
    if (top < bottom) {
        /* Non-empty deque. */
        vdeque_array_t *a = (vdeque_array_t *)vatomicptr_read_rlx(&q->array);
        vsize_t size      = vatomicsz_read_rlx(&a->size);
        vsize_t idx       = top % size;
        *out_obj          = vatomicptr_read_rlx(&a->buffer[idx]);
        state             = VDEQUE_STATE_OK;
        if (vatomicsz_cmpxchg(&q->top, top, top + 1U) != top) {
            /* Failed race. */
            return VDEQUE_STATE_ABORT;
        }
    }
    return state;
}

#endif
