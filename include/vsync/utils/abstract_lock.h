/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_ABSTRACT_LOCK
#define VSYNC_ABSTRACT_LOCK

/*******************************************************************************
 * @file  abstract_lock.h
 * @brief Wrapper to abstract locks during verification.
 *
 * Abstracts a lock implementation such that in compilations for verification,
 * ie, with VSYNC_VERIFICATION defined, the lock will be replaced by a
 * pthread_mutex, which greatly helps the model checkers. On normal compilation
 * the given concrete lock will be used.
 *
 * @example
 *
 * ```c
 * #include <vsync/spinlock/ticketlock.h>
 * #include <vsync/utils/abstract_lock.h>
 *
 * DEF_ABSTRACT_LOCK (
 *	my_lock,             // the prefix for type and functions
 *	ticketlock_t,        // the type of the base lock
 *	ticketlock_init,     // init, acquire and release of the base lock
 *	ticketlock_acquire,
 *	ticketlock_release,
 *	ticketlock_tryacquire
 * )
 * ```
 *
 * This defines `struct my_lock_s` as well as a type `my_lock_t`.
 * It also defines `_init`, `_acquire`, and `_release` functions with the
 * `my_lock` prefix.
 ******************************************************************************/

#include <vsync/atomic.h>
#include <vsync/common/cache.h>
#include <vsync/common/assert.h>

/**
 * Defines an abstract lock type and functions to initialize, acquire, and
 * release it.
 *
 * During normal compilation, the concrete lock type and functions are used.
 * During verification, ie, when VSYNC_VERIFICATION is defined, the
 * lock is replaced by pthread_mutex. This can greatly speed up the verification
 * of some algorithms.
 *
 * @param name abstract lock name
 * @param lock_type concrete-lock type
 * @param lock_init concrete-lock init function
 * @param lock_acquire concrete-lock acquire function
 * @param lock_release concrete-lock release function
 * @param lock_tryacquire concrete-lock tryacquire function
 */
#define DEF_ABSTRACT_LOCK(name, lock_type, lock_init, lock_acquire,            \
                          lock_release, lock_tryacquire)                       \
    DEF_ABSTRACT_LOCK_TYPE(name, lock_type);                                   \
    DEF_ABSTRACT_LOCK_INIT(name, lock_init)                                    \
    DEF_ABSTRACT_LOCK_ACQUIRE(name, lock_acquire)                              \
    DEF_ABSTRACT_LOCK_RELEASE(name, lock_release)                              \
    DEF_ABSTRACT_LOCK_TRYACQUIRE(name, lock_tryacquire)

/** @cond */
#ifdef VSYNC_VERIFICATION
    #define DEF_ABSTRACT_LOCK_TYPE(name, lock_type)                            \
        typedef pthread_mutex_t name##_t

    #define DEF_ABSTRACT_LOCK_INIT(name, lock_init)                            \
        static inline void name##_init(name##_t *l)                            \
        {                                                                      \
            pthread_mutex_init(l, 0);                                          \
        }

    #define DEF_ABSTRACT_LOCK_ACQUIRE(name, lock_acquire)                      \
        static inline void name##_acquire(name##_t *l)                         \
        {                                                                      \
            int val = pthread_mutex_lock(l);                                   \
            ASSERT(val == 0);                                                  \
        }

    #define DEF_ABSTRACT_LOCK_RELEASE(name, lock_release)                      \
        static inline void name##_release(name##_t *l)                         \
        {                                                                      \
            int val = pthread_mutex_unlock(l);                                 \
            ASSERT(val == 0);                                                  \
        }

    #define DEF_ABSTRACT_LOCK_TRYACQUIRE(name, lock_tryacquire)                \
        static inline int name##_tryacquire(name##_t *l)                       \
        {                                                                      \
            return pthread_mutex_trylock(l) == 0;                              \
        }
#else
    #define DEF_ABSTRACT_LOCK_TYPE(name, lock_type) typedef lock_type name##_t

    #define DEF_ABSTRACT_LOCK_INIT(name, lock_init)                            \
        static inline void name##_init(name##_t *l)                            \
        {                                                                      \
            lock_init(l);                                                      \
        }

    #define DEF_ABSTRACT_LOCK_ACQUIRE(name, lock_acquire)                      \
        static inline void name##_acquire(name##_t *l)                         \
        {                                                                      \
            lock_acquire(l);                                                   \
        }

    #define DEF_ABSTRACT_LOCK_RELEASE(name, lock_release)                      \
        static inline void name##_release(name##_t *l)                         \
        {                                                                      \
            lock_release(l);                                                   \
        }

    #define DEF_ABSTRACT_LOCK_TRYACQUIRE(name, lock_tryacquire)                \
        static inline int name##_tryacquire(name##_t *l)                       \
        {                                                                      \
            return lock_tryacquire(l);                                         \
        }
#endif

/** @endcond */

#endif
