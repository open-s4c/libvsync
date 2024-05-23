/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_EXCHANGER_H
#define VSYNC_EXCHANGER_H
#include <vsync/vtypes.h>
#include <vsync/atomic/atomicptr_markable.h>
#include <vsync/common/dbg.h>
#include <vsync/common/verify.h>
/*******************************************************************************
 * @file  exchanger.h
 * @brief Lock-free object exchanger
 *
 * Using the lock-free exchanger two threads can rendezvous to exchange objects
 * The exchange protocol is lock-free and it is deployed in lock-free stack
 * implementations.
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 11.4.1]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 ******************************************************************************/
typedef struct exchanger_s {
    vatomicptr_t slot;
} exchanger_t;

typedef enum exchanger_slot_state_s {
    SLOT_STATE_EMPTY   = 1,
    SLOT_STATE_WAITING = 2,
    SLOT_STATE_BUSY    = 3
} exchanger_slot_state_t;

/**
 * The following code is used only for debugging purposes and is not part of
 * production code.
 *
 */
#ifdef VEXCHANGER_DBG
    #include <test/utils/exchanger_dbg.h>
#else
static inline void
ghost_set_check_point(vsize_t ck)
{
    V_UNUSED(ck);
}
#endif

#define XCHG_CK_0 0U
#define XCHG_CK_1 1U
#define XCHG_CK_2 2U
#define XCHG_CK_3 3U
#define XCHG_CK_4 4U

/**
 * Private functions
 */
static inline void *_exchanger_slot_get(exchanger_t *exchanger,
                                        exchanger_slot_state_t *out_slot_state);
static inline vbool_t
_exchanger_slot_cmpxchg(exchanger_t *exchanger, void *expected_addr,
                        exchanger_slot_state_t expected_state, void *new_addr,
                        exchanger_slot_state_t new_state);
/**
 * Resets the given exchanger object to initial state.
 *
 * @note use it to initialize the object.
 * @param exchanger address of exchanger_t object.
 */
static inline void
exchanger_reset(exchanger_t *exchanger)
{
    vatomicptr_write(&exchanger->slot, (void *)(vuintptr_t)SLOT_STATE_EMPTY);
}
/**
 * Attempts to exchange the given object for a specified number of trials.
 *
 * When Two threads rendezvous via this function, they will atomically exchange
 * the provided pointers/objects. To increase the probability of two threads
 * meeting at the same time, this thread will repeatedly try to find another
 * thread for a specified number of trials.
 *
 * @param exchanger address of exchanger_t object.
 * @param my_item address of the object to exchange. Can be NULL，if the calling
 * thread wants to only consume.
 * @param max_trials the maximum number of exchange trials.
 * @param success output parameter.
 *      true: exchange was successful.
 *      false: exchange failed within the given number of trials.
 * @return void* address of exchanged object on successful exchange.
 * @return NULL if no exchange could be conducted within `max_trials` bound.
 */
static inline void *
exchanger_xchg(exchanger_t *exchanger, void *my_item, vuint32_t max_trials,
               vbool_t *success)
{
    exchanger_slot_state_t slot_state = SLOT_STATE_EMPTY;
    void *your_item                   = NULL;
    vuint32_t trial                   = 0;

    while (true) {
        if (trial >= max_trials) {
            /* exceeded the allowed exchange duration, exchange failed! */
            *success = false;
            ghost_set_check_point(XCHG_CK_0);
            verification_ignore();
            return NULL;
        }
        trial++;
        /* Read the slot state and the object address residing in it */
        your_item = _exchanger_slot_get(exchanger, &slot_state);
        /* Inspect the slot state */
        switch (slot_state) {
            case SLOT_STATE_EMPTY:
                /* the slot is empty no one using it, attempt to place your
                 * object and switch the status to waiting */
                if (_exchanger_slot_cmpxchg(exchanger, your_item,
                                            SLOT_STATE_EMPTY, my_item,
                                            SLOT_STATE_WAITING)) {
                    /* spin as long as within time bound waiting for time to
                     * elapse or a thread to consume the object you placed */
                    while (trial < max_trials) {
                        /* as long as within time bound pull the slot_state */
                        your_item = _exchanger_slot_get(exchanger, &slot_state);
                        /* if state is busy it means another thread consumed our
                         * object and placed his */
                        if (slot_state == SLOT_STATE_BUSY) {
                            /* exchange succeeded, we can reset the slot now for
                             * others to use */
                            exchanger_reset(exchanger);
                            *success = true;
                            ghost_set_check_point(XCHG_CK_1);
                            return your_item;
                        }
                        trial++;
                    }
                    /* time bound exceeded no one showed up, we try to reset the
                     * state, we need to use cas because someone could show up
                     * this second and switch the state to busy */
                    if (_exchanger_slot_cmpxchg(exchanger, my_item,
                                                SLOT_STATE_WAITING, NULL,
                                                SLOT_STATE_EMPTY)) {
                        /* we managed to abort, no one showed up. The exchange
                         * failed */
                        *success = false;
                        ghost_set_check_point(XCHG_CK_2);
                        return NULL;
                    } else {
                        /* luckily someone showed up last minute and completed
                         * the exchange, we consume his object */
                        your_item = _exchanger_slot_get(exchanger, &slot_state);
                        ASSERT(slot_state == SLOT_STATE_BUSY);

                        /* exchange succeeded, we reset the slot for someone
                         * else to use */
                        exchanger_reset(exchanger);
                        *success = true;
                        ghost_set_check_point(XCHG_CK_3);
                        return your_item;
                    }
                }
                break;
            /* we found the slot in waiting state, someone already placed an
             * object for an exchange */
            case SLOT_STATE_WAITING:
                /* we try to consume the object by setting the state to BUSY,
                 * because someone else can be competing we use cas. */
                if (_exchanger_slot_cmpxchg(exchanger, your_item,
                                            SLOT_STATE_WAITING, my_item,
                                            SLOT_STATE_BUSY)) {
                    /* we were the fasted it is all ours, exchange succeeded */
                    *success = true;
                    ghost_set_check_point(XCHG_CK_4);
                    return your_item;
                }
                /* hard luck! someone else beat us to it, we have to try again
                 */
                break;
            case SLOT_STATE_BUSY:
                /* exchange between two threads is ongoing, we try again */
                break;
            default:
                ASSERT(0 && "Unknown STATE!");
        }
    }
}
/**
 * Returns the object and the state of the given `exchanger`'s slot.
 *
 * @param exchanger address of exchanger_t object.
 * @param out_slot_state output parameter the state of the slot.
 * @return void* the object address occupying the slot.
 */
static inline void *
_exchanger_slot_get(exchanger_t *exchanger,
                    exchanger_slot_state_t *out_slot_state)
{
    void *ptr = vatomicptr_read(&exchanger->slot);

    *out_slot_state =
        (exchanger_slot_state_t)(((vuintptr_t)ptr) & ((vuintptr_t)0x3));

    return V_ATOMICPTR_MARKABLE_GET_ADDRESS(ptr);
}
/**
 * Compares and exchanges the slot state and object.
 *
 * @note this is an atomic operation.
 *
 * @param exchanger address of exchanger_t object.
 * @param expected_addr expected address of object currently occupying the slot.
 * @param expected_state expected slot state.
 * @param new_addr address of the new object to place.
 * @param new_state the new state of the slot.
 *
 * @return true compare exchange succeeded.
 * @return false compare exchange failed.
 */
static inline vbool_t
_exchanger_slot_cmpxchg(exchanger_t *exchanger, void *expected_addr,
                        exchanger_slot_state_t expected_state, void *new_addr,
                        exchanger_slot_state_t new_state)
{
    vbool_t success = false;

    ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(expected_addr));
    ASSERT(V_ATOMICPTR_MARKABLE_IS_ALIGNED(new_addr));

    void *exp_val = V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(expected_addr,
                                                              expected_state);
    void *new_val =
        V_ATOMICPTR_MARKABLE_COMBINE_ADDRESS_MARK(new_addr, new_state);

    void *old_val = vatomicptr_cmpxchg(&exchanger->slot, exp_val, new_val);

    success = exp_val == old_val;

    DBG_BLUE("Exchange %u to %u %s", expected_state, new_state,
             success ? "succeeded" : "failed");

    return success;
}

#undef XCHG_CK_0
#undef XCHG_CK_1
#undef XCHG_CK_2
#undef XCHG_CK_3
#undef XCHG_CK_4
#endif
