/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_RECURSIVE_LOCK_H
#define VSYNC_RECURSIVE_LOCK_H
/*******************************************************************************
 * @file  recursive_lock.h
 * @brief Wrapper for defining recursive locks.
 *
 * Recursive embeds libvsync locks, making them recursive (or reentrant).
 * Recursive employs a set of macros, which declare a new recursive lock type
 * and functions to initialize, acquire, and release the lock.
 *
 * @example
 *
 * The following example shows how to make a recursive ticketlock. The `name`
 * of the new lock is `rec_ticketlock`.
 *
 * ```c
 * #include <vsync/spinlock/ticketlock.h>
 * #include <vsync/utils/recursive_lock.h>
 *
 * DEF_RECURSIVE_LOCK (
 *	rec_ticketlock,      // the prefix for type and functions
 *	struct ticketlock_s, // the type of the base lock
 *	ticketlock_init,     // init, acquire and release of the base lock
 *	ticketlock_acquire,
 *	ticketlock_release,
 *	WITHOUT_TRYACQUIRE,  // whether or not to define tryacquire
 *	WITHOUT_CONTEXT      // whether or not the base lock needs context
 * )
 * ```
 *
 * This defines `struct rec_ticketlock_s` as well as a type `rec_ticketlock_t`.
 * It also defines `_init`, `_acquire`, and `_release` functions with the
 * `rec_ticketlock` prefix.
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/assert.h>
#include <vsync/vtypes.h>

#define REC_MACRO_EVAL(...) __VA_ARGS__

/**
 * Defines a recursive lock type and functions to initialize, acquire, and
 * release it.
 *
 * @param name recursive lock name
 * @param lock_type base lock type
 * @param lock_init initialization function of the base lock
 * @param lock_acquire base lock acquire function
 * @param lock_release base lock release function
 * @param tryacquire_sel either #WITHOUT_TRYACQUIRE or
 *        #WITH_TRYACQUIRE(lock_tryacquire)
 * @param context_sel either #WITHOUT_CONTEXT or #WITH_CONTEXT(ctx_type)
 */
#define DEF_RECURSIVE_LOCK(name, lock_type, lock_init, lock_acquire,           \
                           lock_release, tryacquire_sel, context_sel)          \
    DEF_RECURSIVE_LOCK_TYPE(name, lock_type);                                  \
    DEF_RECURSIVE_LOCK_INIT(name, lock_init)                                   \
    DEF_RECURSIVE_LOCK_RELEASE(name, lock_release, context_sel)                \
    DEF_RECURSIVE_LOCK_ACQUIRE(name, lock_acquire, context_sel)                \
    DEF_RECURSIVE_LOCK_TRYACQUIRE(name, tryacquire_sel, context_sel)

/**
 * Defines `struct name_s` and `name_t` as recursive lock embedding `lock_type`.
 *
 * @param name recursive lock name
 * @param lock_type base lock type
 */
#define DEF_RECURSIVE_LOCK_TYPE(name, lock_type)                               \
    typedef struct name##_s {                                                  \
        lock_type lock;                                                        \
        vatomic32_t owner;                                                     \
        vuint32_t count;                                                       \
    } VSYNC_CACHEALIGN name##_t

/** Invalid CPU ID used to represent a released lock. */
#define RECURSIVE_LOCK_NOCPU VUINT32_MAX

/**
 * Initializer of the recursive and the base lock.
 *
 * @param LOCK_INIT initializer of the base lock.
 */
#define RECURSIVE_LOCK_INIT(LOCK_INIT)                                         \
    {                                                                          \
        .lock  = REC_MACRO_EVAL(LOCK_INIT),                                    \
        .owner = VATOMIC_INIT(RECURSIVE_LOCK_NOCPU), .count = 0,               \
    }

/**
 * Defines the init function of the recursive lock.
 *
 * @param name recursive lock name
 * @param lock_init initialization function of the base lock
 *
 * The defined init function initializes the recursive lock, also calling
 * `lock_init` of the base lock. The function declaration is as follows:
 *
 * ```c
 * void name##_init(name##_t *l);
 * ```
 *
 * @param l recursive lock
 */
#define DEF_RECURSIVE_LOCK_INIT(name, lock_init)                               \
    static inline void name##_init(name##_t *l)                                \
    {                                                                          \
        lock_init(&l->lock);                                                   \
        vatomic32_init(&l->owner, RECURSIVE_LOCK_NOCPU);                       \
        l->count = 0;                                                          \
    }

/**
 * Defines the acquire function of the recursive lock.
 *
 * @param name recursive lock name
 * @param lock_acquire base lock acquire function
 * @param context_sel either #WITHOUT_CONTEXT or #WITH_CONTEXT(ctx_type)
 *
 * The defined acquire function calls `lock_acquire` of the base lock if the
 * caller thread is not already the owner of the lock. A counter is used to keep
 * track of the number of recursions.
 *
 * When defined #WITHOUT_CONTEXT, the acquire declaration is as follows:
 * ```c
 * void name##_acquire(name##_t *l, vuint32_t id)
 * ```
 *
 * When defined #WITH_CONTEXT(ctx_type), the acquire declaration is as follows:
 * ```c
 * void name##_acquire(name##_t *l, vuint32_t id, ctx_type *ctx)
 * ```
 *
 * @param l recursive lock
 * @param id thread ID or core ID of the caller
 * @param ctx context of the caller thread/core
 */
#define DEF_RECURSIVE_LOCK_ACQUIRE(name, lock_acquire, context_sel)            \
    V_DEF_RECURSIVE_LOCK_ACQUIRE(                                              \
        name, SELECT_RECURSIVE_LOCK_CALL_CONTEXT(lock_acquire, context_sel),   \
        SELECT_RECURSIVE_LOCK_CONTEXT(context_sel))

/**
 * Defines the release function of the recursive lock.
 *
 * @param name recursive lock name
 * @param lock_release base lock release function
 * @param context_sel either #WITHOUT_CONTEXT or #WITH_CONTEXT(ctx_type)
 *
 * The defined release function decreases the recursion counter, calling
 * `lock_release` of the base lock once the counter is zero.
 *
 * When defined #WITHOUT_CONTEXT, the release declaration is as follows:
 * ```c
 * void name##_release(name##_t *l)
 * ```
 *
 * When defined #WITH_CONTEXT(ctx_type), the release declaration is as follows:
 * ```c
 * void name##_release(name##_t *l, ctx_type *ctx)
 * ```
 *
 * @param l recursive lock
 * @param ctx context of the caller thread/core
 */
#define DEF_RECURSIVE_LOCK_RELEASE(name, lock_release, context_sel)            \
    V_DEF_RECURSIVE_LOCK_RELEASE(                                              \
        name, SELECT_RECURSIVE_LOCK_CALL_CONTEXT(lock_release, context_sel),   \
        SELECT_RECURSIVE_LOCK_CONTEXT(context_sel))

/**
 * Defines the tryacquire function of the recursive lock.
 *
 * @param name recursive lock name
 * @param tryacquire_sel either #WITHOUT_TRYACQUIRE or
 *        #WITH_TRYACQUIRE(lock_tryacquire)
 * @param context_sel either #WITHOUT_CONTEXT or #WITH_CONTEXT(ctx_type)
 *
 * #WITHOUT_TRYACQUIRE disables the definition of the tryacquire function.
 * Selecting #WITH_TRYACQUIRE(lock_tryacquire) defines the tryacquire function,
 * which tries to acquire the recursive lock, calling `lock_tryacquire` of the
 * base lock.  If the caller is already the owner of the lock, the recursion
 * counter is incremented.
 *
 * When defined #WITHOUT_CONTEXT, the tryacquire declaration is as follows:
 * ```c
 * int name##_tryacquire(name##_t *l, vuint32_t id)
 * ```
 * When defined #WITH_CONTEXT(ctx_type), the tryacquire declaration is as
 * follows:
 * ```c
 * int name##_tryacquire(name##_t *l, vuint32_t id, ctx_type *ctx)
 * ```
 * @param l recursive lock
 * @param id thread ID or core ID of the caller
 * @param ctx context of the caller thread/core
 * @return 1 if recursive lock was acquired, otherwise 0.
 *
 * > **Note:** the return values of the tryacquire of the base lock **must** be
 * > the same as described here: 1 for success, 0 for failure.
 */
#define DEF_RECURSIVE_LOCK_TRYACQUIRE(name, tryacquire_sel, context_sel)       \
    RECURSIVE_LOCK_EVAL(SELECT_RECURSIVE_LOCK_TRYACQUIRE, name,                \
                        RECURSIVE_LOCK_PREFIX(tryacquire_sel), context_sel)

/*****************************************************************************
 * internal macros
 *****************************************************************************/
/** @cond DO_NOT_DOCUMENT */

#define V_DEF_RECURSIVE_LOCK_TRYACQUIRE(name, lock_tryacquire, context)        \
    static inline vbool_t name##_tryacquire(name##_t *l, vuint32_t id context) \
    {                                                                          \
        vuint32_t owner = vatomic32_read_rlx(&l->owner);                       \
                                                                               \
        ASSERT(id != RECURSIVE_LOCK_NOCPU && "this value is reserved");        \
                                                                               \
        if (owner == id) {                                                     \
            l->count++;                                                        \
            return true;                                                       \
        }                                                                      \
                                                                               \
        if (owner != RECURSIVE_LOCK_NOCPU) {                                   \
            return false;                                                      \
        }                                                                      \
        if (!(lock_tryacquire)) {                                              \
            return false;                                                      \
        }                                                                      \
        vatomic32_write_rlx(&l->owner, id);                                    \
        return true;                                                           \
    }

#define V_DEF_RECURSIVE_LOCK_ACQUIRE(name, lock_acquire, context)              \
    static inline void name##_acquire(name##_t *l, vuint32_t id context)       \
    {                                                                          \
        ASSERT(id != RECURSIVE_LOCK_NOCPU && "this value is reserved");        \
        if (vatomic32_read_rlx(&l->owner) == id) {                             \
            l->count++;                                                        \
            return;                                                            \
        }                                                                      \
                                                                               \
        (lock_acquire);                                                        \
        vatomic32_write_rlx(&l->owner, id);                                    \
    }

#define V_DEF_RECURSIVE_LOCK_RELEASE(name, lock_release, context)              \
    static inline void name##_release(name##_t *l context)                     \
    {                                                                          \
        if (l->count == 0) {                                                   \
            vatomic32_write_rlx(&l->owner, RECURSIVE_LOCK_NOCPU);              \
            (lock_release);                                                    \
        } else {                                                               \
            l->count--;                                                        \
        }                                                                      \
    }

/* context selectors */
#define RECURSIVE_LOCK_WITH_CONTEXT(ctx_type) , REC_MACRO_EVAL(ctx_type) * ctx
#define RECURSIVE_LOCK_WITHOUT_CONTEXT

#define RECURSIVE_LOCK_CALL_WITH_CONTEXT(ctx_type) , ctx
#define RECURSIVE_LOCK_CALL_WITHOUT_CONTEXT

#define SELECT_RECURSIVE_LOCK_CONTEXT(selector) RECURSIVE_LOCK_PREFIX(selector)

#define SELECT_RECURSIVE_LOCK_CALL_CONTEXT(lock_call, selector)                \
    lock_call(&l->lock RECURSIVE_LOCK_PREFIX(CALL_##selector))

/* try acquire selector */
#define RECURSIVE_LOCK_WITH_TRYACQUIRE(lock_tryacquire)                        \
    WITH_TRYACQUIRE, (lock_tryacquire)
#define RECURSIVE_LOCK_WITHOUT_TRYACQUIRE WITHOUT_TRYACQUIRE, none

#define DEF_RECURSIVE_LOCK_WITHOUT_TRYACQUIRE(name, lock_tryacquire, context)
#define DEF_RECURSIVE_LOCK_WITH_TRYACQUIRE(name, lock_tryacquire, context_sel) \
    V_DEF_RECURSIVE_LOCK_TRYACQUIRE(                                           \
        name,                                                                  \
        SELECT_RECURSIVE_LOCK_CALL_CONTEXT(lock_tryacquire, context_sel),      \
        SELECT_RECURSIVE_LOCK_CONTEXT(context_sel))

#define SELECT_RECURSIVE_LOCK_TRYACQUIRE(name, SELECT, lock_tryacquire,        \
                                         context_sel)                          \
    DEF_RECURSIVE_LOCK_##SELECT(name, lock_tryacquire, context_sel)

/* helper */
#define RECURSIVE_LOCK_PREFIX(SUFFIX)   RECURSIVE_LOCK_##SUFFIX
#define RECURSIVE_LOCK_EVAL(A, B, C, D) A(B, C, D)

/** @endcond */

/*****************************************************************************
 * macros for documentation only
 *****************************************************************************/
#ifdef DOCUMENTATION
    /**
     * Option to disable the definition of a tryacquire function.
     */
    #define WITHOUT_TRYACQUIRE

    /**
     * Option to enable the definition of a tryacquire function, calling the
     * base @p lock_tryacquire function.
     */
    #define WITH_TRYACQUIRE(lock_tryacquire)

    /**
     * Option to disable the use of context in acquire and release functions.
     */
    #define WITHOUT_CONTEXT

    /**
     * Option to add a context argument to acquire and release functions.
     */
    #define WITH_CONTEXT(context_type)

#endif /* DOCUMENTATION */

#endif
