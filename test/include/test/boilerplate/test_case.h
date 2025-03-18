/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VERIFY_TEST_CASE_BOILERPLATE_H
#define VERIFY_TEST_CASE_BOILERPLATE_H
/**
 * @file test_case.h
 * @brief boilerplate for data-struct verification tests case.
 *
 */
#include <vsync/vtypes.h>
#include <pthread.h>
#include <test/thread_launcher.h>

#ifndef NTHREADS
    #define NTHREADS 3U
#endif

#if NTHREADS > 4U
    #error                                                                     \
        "Extend the boilerplate to allow for more than 4threads, and make sure verification can finish within a reasonable time."
#endif

/**
 * Initializes the data structure and necessary resources.
 *
 * @note called before thread create and post.
 */
void init(void);
/**
 * Destroys and cleanup allocated resources by `init`.
 *
 * @note called after thread join and post.
 */
void fini(void);
/**
 * Registers the thread if necessary.
 *
 * @note called by thread function and before thread action.
 */
void reg(vsize_t tid);
/**
 * Deregisters the thread if registered.
 *
 * @note called by thread function and after thread action.
 */
void dereg(vsize_t tid);
/**
 * Action(s) to satisfy the pre-requisites of a specific test-case.
 *
 * @note called before thread create and after init.
 */
void pre(void);
/**
 * Action(s) to satisfy the post-requisites of a specific test-case.
 * Including verification of the final state of the data-struct.
 *
 * @note called after thread join and before fini.
 */
void post(void);
/**
 * Action taken by the first thread.
 */
void t0(vsize_t tid);
/**
 * Action taken by the second thread.
 */
void t1(vsize_t tid);
/**
 * Action taken by the third thread.
 */
void t2(vsize_t tid);

#if NTHREADS > 3U
void t3(vsize_t tid);
#endif

/*
 * includes the test-case file implementing pre, t*, post
 * passed to the compiler by cmake using -DTEST_CASE="<tc>.h"
 */
#ifdef TEST_CASE
    #include TEST_CASE
#else
    #error "no test case was defined"
#endif
/**
 * Thread function, registers the thread dispatches the action based on the tid
 * and deregisters the thread.
 */
void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    ASSERT(tid < NTHREADS);

    reg(tid);
    switch (tid) {
        case 0:
            t0(tid);
            break;
        case 1:
            t1(tid);
            break;
        case 2:
            t2(tid);
            break;
#if NTHREADS > 3U
        case 3:
            t3(tid);
            break;
#endif
        default:
            break;
    }
    dereg(tid);
    return NULL;
}
/**
 * test case main function.
 *
 * @note call from main thread, calls the functions in the right order to run
 * the test case.
 *
 */
void
tc(void)
{
    init();
    pre();
    launch_threads(NTHREADS, run);
    post();
    fini();
}

#endif
