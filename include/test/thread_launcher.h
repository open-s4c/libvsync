/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_THREAD_LAUNCHER
#define VSYNC_THREAD_LAUNCHER

#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vsync/vtypes.h>
#include <vsync/atomic.h>
#include <test/time.h>

#if defined(VSYNC_VERIFICATION)
    #define WAIT_FOR_GO_SIGNAL()
    #define GIVE_START_SIGNAL()
    #define RESET_START_SIGNAL()
#else
    #ifdef __linux__
        #if !defined(_GNU_SOURCE)
            #error add "-D_GNU_SOURCE" to your compile command
        #endif
        #include <sched.h>
        #include <sys/sysinfo.h>
    #else
        #define DISABLE_LAUCHER_AFFINITY
    #endif
static vatomic32_t g_go = VATOMIC_INIT(0);
    #define GIVE_START_SIGNAL()  vatomic32_write(&g_go, 1)
    #define RESET_START_SIGNAL() vatomic32_write(&g_go, 0)
    #define WAIT_FOR_GO_SIGNAL() while (vatomic32_read(&g_go) == 0)
#endif

typedef void *(*thread_fun_t)(void *);


typedef struct {
    pthread_t t;
    vsize_t id;
    vbool_t assign_me_to_core;
    thread_fun_t run_fun;
} run_info_t;

static inline void set_cpu_affinity(vsize_t target_cpu);

static inline void *
common_run(void *args)
{
    run_info_t *run_info = (run_info_t *)args;

    if (run_info->assign_me_to_core)
        set_cpu_affinity(run_info->id);

    WAIT_FOR_GO_SIGNAL();

    return run_info->run_fun((void *)run_info->id);
}

/**
 * binds the calling thread to the target_cpu
 * @ref https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html
 * @param target_cpu
 */
static inline void
set_cpu_affinity(vsize_t target_cpu)
{
#if !defined(VSYNC_VERIFICATION) && !defined(DISABLE_LAUCHER_AFFINITY)
    int number_of_cores = get_nprocs();

    cpu_set_t set;
    CPU_ZERO(&set);

    /* to cover for oversubscription */
    vsize_t core = target_cpu % number_of_cores;

    CPU_SET(core, &set);

    if (sched_setaffinity(0, sizeof(set), &set) < 0) {
        ASSERT(0 && "failed to set affinity");
    }
#else
    V_UNUSED(target_cpu);
#endif
}

static inline void
create_threads(run_info_t *threads, vsize_t num_threads, thread_fun_t fun,
               vbool_t bind)
{
    vsize_t i = 0;
    for (i = 0; i < num_threads; i++) {
        threads[i].id                = i;
        threads[i].run_fun           = fun;
        threads[i].assign_me_to_core = bind;
        pthread_create(&threads[i].t, 0, common_run, &threads[i]);
    }
    GIVE_START_SIGNAL();
}

static inline void
await_threads(run_info_t *threads, vsize_t num_threads)
{
    vsize_t i = 0;
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i].t, NULL);
    }
}
/**
 * Launches as many threads as `thread_count` and waits for them to finish
 * @note the threads start on the same signal, and are bound to different cores
 * @param thread_count number of threads to create
 * @param fun thread function to pass
 */
static inline void
launch_threads(vsize_t thread_count, thread_fun_t fun)
{
    run_info_t *threads = malloc(sizeof(run_info_t) * thread_count);

    create_threads(threads, thread_count, fun, true);

    await_threads(threads, thread_count);

    free(threads);
}
/**
 * Launches threads, waits for a given amount of time, write to stop signal and
 * wait for threads to finish gracefully
 *
 * @param thread_count number of threads to launch
 * @param fun thread function
 * @param wait_in_seconds number of seconds for main thread to sleep after
 * launching threads
 * @param stop_flag address of the stop flag to write to
 * @param bind should the thread be bound to a core or not
 */
static inline cpu_time_t
launch_threads_and_stop_them(vsize_t thread_count, thread_fun_t fun,
                             unsigned int wait_in_seconds,
                             vatomic32_t *stop_flag, vbool_t bind)
{
    cpu_time_t stop_time = {0};
    run_info_t *threads  = malloc(sizeof(run_info_t) * thread_count);

    create_threads(threads, thread_count, fun, bind);

#if !defined(VSYNC_VERIFICATION)
    sleep(wait_in_seconds);
#endif

    vatomic32_write(stop_flag, 1);
    record_time(&stop_time);

    await_threads(threads, thread_count);

    free(threads);
    return stop_time;
}


#endif
