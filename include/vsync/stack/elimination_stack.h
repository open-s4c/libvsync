/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSTACK_ELIMINATION_H
#define VSTACK_ELIMINATION_H
/******************************************************************************
 * @file  elimination_stack.h
 * @brief Unbounded lock-free stack with elimination backoff.
 * @ingroup requires_smr lock_free linearizable
 *
 * When an attempt to pop/push fails, the calling thread
 * will attempt to rendezvous with another thread. A thread that pushes may
 * exchange the node that is trying to push with a thread that pops. A
 * successful exchange happens when a pusher exchanges its node with a popper
 * within a limited number of trials. If the exchange does not complete within
 * the number of attempts/trials specified in `VSTACK_XCHG_MAX_TRIALS` the
 * normal push/pop operation is reattempted.
 *
 *
 * ## Configuration
 * - `-DVSTACK_XCHG_SLOTS_COUNT=3`
 * - `-DVSTACK_XCHG_MAX_TRIALS=10000`
 *
 * Note that the right configuration depends on the contention, number of
 * threads etc. We highly recommend you to benchmark the stack within your
 * application with different configurations and choose the configuration
 * resulting in the best performance.
 *
 * @example
 * @include eg_stack_elimination.c
 *
 * @cite
 * Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 11]
 * (https://dl.acm.org/doi/pdf/10.5555/2385452)
 *****************************************************************************/
#include <vsync/common/assert.h>
#include <vsync/atomic.h>
#include <vsync/utils/exchanger.h>
#include <vsync/utils/backoff.h>
#include <vsync/stack/internal/range_policy.h>
#include <vsync/stack/internal/stack_core.h>
#include <vsync/common/verify.h>

/*******************************************************************************
 * @def VSTACK_XCHG_SLOTS_COUNT
 * @brief Configures the stack to have the given number of slots.
 *
 * @note the default value is `-DVSTACK_XCHG_SLOTS_COUNT=3`
 ******************************************************************************/
#ifndef VSTACK_XCHG_SLOTS_COUNT
    #define VSTACK_XCHG_SLOTS_COUNT 3U
#endif
/*******************************************************************************
 * @def VSTACK_XCHG_MAX_TRIALS
 * @brief Configures the stack to try to exchange for the given number of trials
 * max.
 *
 * @note the default value is `-DVSTACK_XCHG_MAX_TRIALS=10000`
 ******************************************************************************/
#ifndef VSTACK_XCHG_MAX_TRIALS
    #define VSTACK_XCHG_MAX_TRIALS 10000U
#endif

/** @cond DO_NOT_DOCUMENT */
#if defined(VSYNC_VERIFICATION)
    #undef VSTACK_XCHG_MAX_TRIALS
    #define VSTACK_XCHG_MAX_TRIALS 5U
#endif
/** @endcond */

typedef struct vstack_s {
    vstack_core_t core;
    backoff_rand_fun_t rand_fun;
    exchanger_t elimination_array[VSTACK_XCHG_SLOTS_COUNT];
} vstack_t;

static inline vstack_node_t *_vstack_rendezvous(vstack_t *stack,
                                                vstack_node_t *node,
                                                vuint32_t max,
                                                vbool_t *out_success);
/**
 * Initializes the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @param rand_fun a function pointer to a function that generates a random
 * number.
 */
static inline void
vstack_init(vstack_t *stack, backoff_rand_fun_t rand_fun)
{
    vsize_t i = 0;
    ASSERT(stack);
    ASSERT(rand_fun);
    stack->rand_fun = rand_fun;
    /* initialize stack core */
    vstack_core_init(&stack->core);
    /* initialize exchanger slots */
    for (i = 0; i < VSTACK_XCHG_SLOTS_COUNT; i++) {
        exchanger_reset(&stack->elimination_array[i]);
    }
}
/**
 * Pushes the given `node` to the top of the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @param node  address of vstack_node_t object.
 * @note this operation always succeeds.
 */
static inline void
vstack_push(vstack_t *stack, vstack_node_t *node)
{
    vbool_t success      = false;
    vuint32_t max        = 0;
    void *exchanged_node = NULL;
    range_policy_t policy;

    ASSERT(stack);
    ASSERT(node);

    /* @note in the book this is a thread local policy, however at the moment we
     * use a new instance for each operation, this is because we thread local
     * storage might degrade performance, benchmarking required for this.
     * Determines the elimination array subrange to be used */
    range_policy_record_init(&policy, VSTACK_XCHG_SLOTS_COUNT - 1);

    while (true) {
        /* try the normal path of pushing first */
        if (vstack_core_try_push(&stack->core, node)) {
            /* successful push */
            return;
        } else {
            max = range_policy_get_range(&policy);
            /* try to exchange with another some thread */
            exchanged_node = _vstack_rendezvous(stack, node, max, &success);
            if (success) {
                if (exchanged_node == NULL) {
                    /* if the returned node is NULL it means we exchanged
                     * with a popper */
                    range_policy_record_success(&policy);
                    /* successful exchange */
                    return;
                }
                /* if the exchanged node is not NULL, it means we exchanged with
                 * a pusher, in that case we need to reattempt the operation */
                verification_ignore();
            } else {
                // we only record fail on timeout
                verification_ignore();
                range_policy_record_fail(&policy);
            }
        }
    }
}
/**
 * Pops the top of the given `stack`.
 *
 * @param stack address of vstack_t object.
 * @return address of vstack_node_t object. If the stack is not empty.
 * @return NULL on empty stack.
 * @note must be called inside an SMR critical section.
 */
static inline vstack_node_t *
vstack_pop(vstack_t *stack)
{
    vstack_node_t *node = NULL;
    vbool_t success     = false;
    vbool_t pop_success = false;
    vuint32_t max       = 0;
    range_policy_t policy;

    ASSERT(stack);

    range_policy_record_init(&policy, VSTACK_XCHG_SLOTS_COUNT - 1);

    while (true) {
        node = vstack_core_try_pop(&stack->core, &pop_success);
        if (pop_success) {
            /* successful pop */
            break;
        } else {
            max  = range_policy_get_range(&policy);
            node = _vstack_rendezvous(stack, NULL, max, &success);
            if (success) {
                /* success means the exchange is complete is successful */
                if (node) {
                    /* if the returned node is not NULL it means we exchanged
                     * with a pusher */
                    range_policy_record_success(&policy);
                    /* successful exchange */
                    break;
                }
                /* @note: if exchanged_node is NULL this means we exchanged with
                 * a pop and thus we need to repeat the operation */
                verification_ignore();
            } else {
                ASSERT(!node);
                verification_ignore();
                /* timeout we update the policy */
                range_policy_record_fail(&policy);
            }
        }
    }
    return node;
}
/**
 * Attempts to rendezvous with another thread to exchange the given `node`.
 *
 * @param stack address of vstack_t object
 * @param node address of vstack_node_t object if the calling thread is a
 * pusher, NULL otherwise
 * @param max maximum index of the exchange slot that can be used in this
 * exchange
 * @param out_success output parameter indicating if the exchange was successful
 *      *out_success == true: successful exchange
 *      *out_success == false: failed exchange
 * @return address of vstack_node_t* object.
 */
static inline vstack_node_t *
_vstack_rendezvous(vstack_t *stack, vstack_node_t *node, vuint32_t max,
                   vbool_t *out_success)
{
    exchanger_t *exchanger        = NULL;
    vstack_node_t *exchanged_node = NULL;
    vsize_t slot_idx              = 0;

    /* randomly pick a slot with index in [0, VSTACK_XCHG_SLOTS_COUNT] */
    slot_idx = stack->rand_fun(0, max);
    ASSERT(slot_idx < VSTACK_XCHG_SLOTS_COUNT);
    exchanger = &stack->elimination_array[slot_idx];
    exchanged_node =
        exchanger_xchg(exchanger, node, VSTACK_XCHG_MAX_TRIALS, out_success);
    return exchanged_node;
}
/**
 * Pops all remaining nodes in the stack and calls `destroy` on them.
 *
 * @param stack address of vstack_t object.
 * @param destroy function address of type vstack_node_handler_t.
 * @param arg second argument of `destroy`, can be NULL.
 */
static inline void
vstack_destroy(vstack_t *stack, vstack_node_handler_t destroy, void *arg)
{
    vstack_core_destroy(&stack->core, destroy, arg);
}
/**
 * Calls the given `visitor` function on each stack node.
 *
 * @param stack address of vstack_t object.
 * @param visitor function address of type vstack_node_handler_t.
 * @param arg second argument of `visitor`, can be NULL.
 */
static inline void
_vstack_visit(vstack_t *stack, vstack_node_handler_t visitor, void *arg)
{
    vstack_core_visit(&stack->core, visitor, arg);
}
#endif
