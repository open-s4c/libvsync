/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_EBR_H
#define VSYNC_EBR_H

/******************************************************************************
 * @file ebr.h
 * @brief Epoch Based Reclamation (EBR) SMR scheme.
 *
 * EBR relies on global and local epoch counters to determine when it is
 * safe to reclaim memory.
 *
 * - global epoch = `e+1` => all active threads must have a local epoch in
 *  {`e`, `e+1`}
 * - global epoch = `e+1` => nodes retired on epoch `e-1` are safe
 * to recycle.
 *
 * @example
 * @include eg_vebr.c
 *
 * @cite
 * Keir Fraser - [Practical
 * lock-freedom](https://www.cl.cam.ac.uk/techreports/UCAM-CL-TR-579.pdf)
 *****************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/dbg.h>
#include <vsync/utils/math.h>
#include <vsync/common/assert.h>
#include <vsync/common/verify.h>
#include <vsync/common/compiler.h>
#include <vsync/smr/internal/smr_lock.h>
#include <vsync/smr/internal/dbl_list.h>
#include <vsync/smr/internal/smr_nodes_list.h>

#define V_EBR_EPOCH_COUNT (vuint64_t)3
/*
 * Calculate the queue index associated with the given epoch
 * e = 1 => 1
 * e = 2 => 2
 * e = 3 => 0
 * e = 4 => 1
 * e = 5 => 2
 * e = 6 => 0
 * .. etc
 */
#define V_EBR_GET_EPOCH_IDX(_e_) ((vsize_t)((_e_) % V_EBR_EPOCH_COUNT))
/*
 * Calculate the queue index associated with the previous epoch
 * e = 1 => 0
 * e = 2 => 1
 * e = 3 => 2
 * e = 4 => 0
 * e = 5 => 1
 * e = 6 => 2
 * .. etc
 */
#define V_EBR_GET_PREV_EPOCH_IDX(_e_)                                          \
    ((vsize_t)(((_e_) + 2U) % V_EBR_EPOCH_COUNT))

/**
 * Per-thread EBR object
 */
typedef struct vebr_instance_s {
    dbl_list_node_t lst_node; /* !!! keep as first field !!! */
    vatomic64_t epoch_local;
    vuint32_t enter_count;
} vebr_thread_t;

/**
 * The global EBR object
 */
typedef struct vebr_s {
    vatomic64_t epoch_global;
    smr_nodes_list_t retired_lst[V_EBR_EPOCH_COUNT];
    dbl_list_t threads;
    smr_lock_lib_t lock;
} VSYNC_CACHEALIGN vebr_t;

static inline vuint64_t _vebr_sync_epochs(vebr_t *ebr, vuint64_t epoch,
                                          vbool_t blocking);
/**
 * Initializes the given object `ebr`.
 *
 * @param ebr address of vebr_t object.
 * @param lock_lib smr_lock_lib_t object.
 */
static inline void
vebr_init(vebr_t *ebr, smr_lock_lib_t lock_lib)
{
    ASSERT(ebr);
    dbl_list_init(&ebr->threads);
    ASSERT(ebr->threads.head == NULL);

    for (vsize_t i = 0; i < V_EBR_EPOCH_COUNT; i++) {
        smr_nodes_list_init(&ebr->retired_lst[i]);
    }
    vatomic64_write(&ebr->epoch_global, 1);
    ASSERT(smr_lock_lib_is_set(&lock_lib));
#if defined(VSYNC_VERIFICATION)
    ebr->lock.arg = lock_lib.arg;
    ebr->lock.acq = lock_lib.acq;
    ebr->lock.rel = lock_lib.rel;
#else
    ebr->lock = lock_lib;
#endif
}
/**
 * Destroys the resources associated with given `ebr` object.
 *
 * @param ebr address of vebr_t object.
 */
static inline void
vebr_destroy(vebr_t *ebr)
{
    vsize_t i        = 0;
    smr_node_t *head = NULL;

    ASSERT(ebr);
    /* destroy all remaining nodes */
    for (i = 0; i < V_EBR_EPOCH_COUNT; i++) {
        head = smr_nodes_list_get_and_empty(&ebr->retired_lst[i]);
        smr_nodes_list_destroy(head);
    }
}
/**
 * Registers and initializes the given `ebr_thrd`.
 *
 * @note each thread must be associated with a unique `ebr_thrd` that lives till
 * the thread deregisters.
 *
 * @param ebr address of vebr_t object.
 * @param ebr_thrd address of vebr_thread_t object.
 */
static inline void
vebr_register(vebr_t *ebr, vebr_thread_t *ebr_thrd)
{
    ASSERT(ebr);
    ASSERT(ebr_thrd);
    vatomic64_init(&ebr_thrd->epoch_local, 0);
    ebr_thrd->enter_count = 0;
    /* the threads list is protected by a lock */
    ebr->lock.acq(ebr->lock.arg);
    dbl_list_add(&ebr->threads, &ebr_thrd->lst_node);
    ebr->lock.rel(ebr->lock.arg);
}
/**
 * Deregisters the given `ebr_thrd`.
 *
 * @note the thread is associated with the given `ebr_thrd` should not attempt
 * to access the given `ebr` instance after this call.
 * @pre `vebr_register`.
 * @param ebr address of vebr_t object.
 * @param ebr_thrd address of vebr_thread_t object.
 */
static inline void
vebr_deregister(vebr_t *ebr, vebr_thread_t *ebr_thrd)
{
    ASSERT(ebr);
    ASSERT(ebr_thrd);

    ASSERT(vatomic64_read_rlx(&ebr_thrd->epoch_local) == 0);

    ebr->lock.acq(ebr->lock.arg);
    dbl_list_rem(&ebr->threads, &ebr_thrd->lst_node);
    ebr->lock.rel(ebr->lock.arg);
}
/**
 * Marks the beginning of a critical section, indicates that a reader is active.
 *
 * @post `vebr_exit`
 * @param ebr address of vebr_t object.
 * @param ebr_thrd address of vebr_thread_t object.
 */
static inline void
vebr_enter(vebr_t *ebr, vebr_thread_t *ebr_thrd)
{
    vuint64_t glb_epoch = 0;

    ASSERT(ebr);
    ASSERT(ebr_thrd);
    /**
     * The fact that the thread does not render itself active before
     * reading the global epoch means that technically the invariant
     * of there only being 2 epochs at a given moment is violated.
     * However, since we recycle at the global epoch, that is not an
     * issue.
     */
    ebr_thrd->enter_count++;
    if (ebr_thrd->enter_count == 1) {
        glb_epoch = vatomic64_read(&ebr->epoch_global);
        vatomic64_write_rel(&ebr_thrd->epoch_local, glb_epoch);
        vatomic_fence();
    }
}
/**
 * Marks the end of a critical section, indicates that a reader is inactive.
 *
 * @pre `vebr_enter`
 * @param ebr address of vebr_t object.
 * @param ebr_thrd address of vebr_thread_t object.
 */
static inline void
vebr_exit(vebr_t *ebr, vebr_thread_t *ebr_thrd)
{
    ASSERT(ebr_thrd);
    ASSERT(ebr_thrd->enter_count);
    if (ebr_thrd->enter_count == 1U) {
        vatomic64_write_rel(&ebr_thrd->epoch_local, 0U);
    }
    ebr_thrd->enter_count--;
    V_UNUSED(ebr);
}
/**
 * Defers reclamation of objects until it is safe to do so.
 *
 * @note must be called within the critical section. This is important to avoid
 * data race on the local queue when recycle is being called by another thread.
 * @param ebr address of vebr_t object.
 * @param ebr_thrd address of vebr_thread_t object.
 * @param smr_node address of smr_node_t object.
 * @param dest address of callback function used for destroying the retired
 * `smr_node`.
 */
static inline void
vebr_retire(vebr_t *ebr, vebr_thread_t *ebr_thrd, smr_node_t *smr_node,
            smr_node_destroy_fun destructor, void *destructor_args)
{
    vuint64_t epoch = 0;
    vsize_t idx     = 0;

    /**
     * The fence is required for scenarios that are similar to
     * test_case_specific_wrap_around_ebr.h
     *
     */
    vatomic_fence();

    /**
     * Note that we always retire in the global epoch
     */
    epoch = vatomic64_read(&ebr->epoch_global);
    /* retire on the current global epoch */
    idx = V_EBR_GET_EPOCH_IDX(epoch);
    ASSERT(idx <= V_EBR_EPOCH_COUNT);
    smr_nodes_list_add(&ebr->retired_lst[idx], smr_node, destructor,
                       destructor_args);
    V_UNUSED(ebr_thrd);
}
/**
 * Recycles/frees nodes that can safely be freed.
 *
 * Assesses whether all active threads have observed the current
 * epoch_global. If so, advances the global epoch:
 *  `epoch_global' = epoch_global + 1`.
 * And frees nodes associated with with
 *  `epoch_global - 1`, i.e. `epoch_global' * - 2`
 *
 * @note It is recommended that you call this function periodically in its own
 * dedicated thread that does not interfere with the readers. As this function
 * acquires a lock and might block the calling thread for a while.
 * @param ebr address of vebr_t object.
 *
 * @return count of recycled nodes.
 */
static inline vsize_t
vebr_recycle(vebr_t *ebr)
{
    vsize_t count       = 0;
    vsize_t idx         = 0;
    vuint64_t glb_epoch = 0;
    vuint64_t ret_epoch = 0;
    vuint64_t lcl_epoch = 0;
    smr_node_t *head    = NULL;

    vatomic_fence();

    glb_epoch = vatomic64_read(&ebr->epoch_global);
    idx       = V_EBR_GET_PREV_EPOCH_IDX(glb_epoch);
    head      = smr_nodes_list_get_and_empty(&ebr->retired_lst[idx]);
    lcl_epoch = _vebr_sync_epochs(ebr, glb_epoch, true);
    ASSERT(lcl_epoch == 0 || lcl_epoch >= glb_epoch);

    /*
     * Threads are either inactive, or have already observed the global epoch.
     * We advance the global epoch before accessing and recycling the queue
     * associated with epoch `glb_epoch - 1`. If a thread becomes active before
     * this call takes an effect, it will still be safe because then it can only
     * access glb_epoch's queue and not `glb_epoch - 1`
     */
    ret_epoch =
        vatomic64_cmpxchg(&ebr->epoch_global, glb_epoch, glb_epoch + 1U);

    /* either we succeeded or someone else did */
    ASSERT(ret_epoch >= glb_epoch);

    if (head) {
        count = smr_nodes_list_destroy(head);
    }
    return count;
}
/**
 * Synchronizes active threads.
 *
 * The caller can free nodes detached before this call directly after this
 * function returns. In order to give this guarantee. The observed global epoch
 * `global_epoch` must be advanced twice `global_epoch' = global_epoch + 2` or
 * all threads are observed as inactive. If a thread is active it must
 * have already observed `global_epoch + 1` by the return of this function.
 *
 * ```C
 * // detach node
 * vebr_sync(&ebr);
 * free(node);
 * ```
 *
 * @note this function is designed for the above use-case, and it should not be
 * used unless it is absolutely. This function acquires a lock and uses
 * spin-loops. Thus, it is recommended to use `vebr_retire` and `vebr_recycle`
 * instead.
 * @param ebr address of vebr_t object.
 */
static inline void
vebr_sync(vebr_t *ebr)
{
    vuint64_t glb_epoch    = 0;
    vuint64_t cur_epoch    = 0;
    vuint64_t target_epoch = 0;
    vuint64_t lcl_epoch    = 0;
    vsize_t i              = 0;

    vatomic_fence();

    glb_epoch = vatomic64_read(&ebr->epoch_global);

    /* if the epoch get advanced to the target then it is safe to free
     * nodes associated with the just read glb_epoch */
    target_epoch = glb_epoch + 2U;

    for (cur_epoch = glb_epoch, i = 0; cur_epoch < target_epoch; i++) {
        /* contains a spin-loop, this is a blocking function */
        lcl_epoch = _vebr_sync_epochs(ebr, cur_epoch, true);
        /**
         * @TODO: it would be ok to change `lcl_epoch >= target_epoch` to
         * `lcl_epoch >= target_epoch - 1` if we make sure we do a cas to move
         * the epoch to the target afterwards, but this might complicate the
         * code.
         *
         */
        if (lcl_epoch == 0 || lcl_epoch >= target_epoch) {
            /* all threads are inactive or have observed the target，then our
             * job is done */
            return;
        } else {
            /* all active threads have observed `cur_epoch` advance to cur_epoch
             * + 1 */
            glb_epoch =
                vatomic64_cmpxchg(&ebr->epoch_global, cur_epoch, cur_epoch + 1);

            /* determine the cur_epoch based on the outcome of the cas */
            cur_epoch = glb_epoch == cur_epoch ? cur_epoch + 1 : glb_epoch;
        }
    } /* as long as target is not reached */
    ASSERT(i <= 2);
}
/**
 * Checks if/ensures every registered thread has observed an epoch >= to the
 * given `epoch`.
 *
 * @note must be called under lock protection. Same lock used to protect
 * threads' registration list.
 *
 * @param ebr address of vebr_t object.
 * @param epoch target epoch.
 * @param blocking a boolean flag determines if the API should exit if one of
 * the active threads local epoch is behind the given `epoch` or spin till the
 * thread local epoch catches up, i.e. `t.local_epoch >= epoch`.
 * @return vuint64_t the minimum local_epoch of active threads, or 0 if all
 * registered threads are inactive.
 *
 */
static inline vuint64_t
_vebr_sync_epochs(vebr_t *ebr, vuint64_t epoch, vbool_t blocking)
{
    dbl_list_node_t *node = NULL;
    vebr_thread_t *thrd   = NULL;
    vuint64_t lcl_epoch   = 0;
    vuint64_t min_epoch   = VUINT64_MAX;

    /* for each registered thread */
    ebr->lock.acq(ebr->lock.arg);
    for (node = ebr->threads.head; node; node = node->next) {
        thrd = (vebr_thread_t *)node;

        do {
            lcl_epoch = vatomic64_read(&thrd->epoch_local);
            if (lcl_epoch == 0) { /* thread is inactive, no need to spin */
                break;
            } else if (lcl_epoch >= epoch) { /* thread's local epoch is what we
                                                want no need to spin */
                /* update minimum local epoch if necessary */
                min_epoch = VMIN(min_epoch, lcl_epoch);
                break;
            } else {
                /* local epoch is behind */
                if (!blocking) { /* we cannot block, return the local epoch, it
                                    must be the minimum we have checked so far
                                    anyways */
                    min_epoch = lcl_epoch;
                    goto EXIT;
                } else {
                    /* spinning  */
                    verification_ignore();
                }
            }
        } while (true);
    }
EXIT:
    ebr->lock.rel(ebr->lock.arg);
    /* min_epoch == 0 => all inactive */
    /* min_epoch >= epoch => all those that are active have at least observed
     * (valid if blocking = false) min_epoch < epoch => there exists at least
     * one active thread with a local epoch that is behind given epoch */
    return min_epoch == VUINT64_MAX ? 0 : min_epoch;
}
#undef V_EBR_EPOCH_COUNT
#undef V_EBR_GET_EPOCH_IDX
#undef V_EBR_GET_PREV_EPOCH_IDX
#endif
