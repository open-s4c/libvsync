/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BOILERPLATE_LOCK_H
#define VSYNC_BOILERPLATE_LOCK_H
/*******************************************************************************
 * Boilerplate for testing spinlocks and mutexes
 *
 * Constants:
 *
 * - NTHREADS  : total number of threads
 * - REACQUIRE : first threads enters the critical section twice.
 * - REENTRANT : first threads acquire (and release) lock twice.
 * - WITH_INIT : define init()
 * - WITH_FINI : define fini()
 * - WITH_POST : define post()
 ******************************************************************************/
#include <vsync/atomic.h>
#include <vsync/common/verify.h>

#ifdef VSYNC_LOTTO
    #ifdef REACQUIRE
        #undef REACQUIRE
    #endif
    #define REACQUIRE 2048
    #ifdef NTHREADS
        #undef NTHREADS
    #endif
    #define NTHREADS 7
#endif

void acquire(vuint32_t tid);
void release(vuint32_t tid);

#ifndef NTHREADS
    #define NTHREADS 3
#endif

#ifndef REACQUIRE
    #define REACQUIRE 0
#endif

#ifndef REENTRANT
    #define REENTRANT 0
#endif

#define REACQUIRE_LOOP_BOUND (REACQUIRE + 2)
#define REENTRANT_LOOP_BOUND (REENTRANT + 2)

/*******************************************************************************
 * client code
 ******************************************************************************/
#include <stdio.h>
#include <vsync/vtypes.h>
#include <pthread.h>
#include <vsync/common/assert.h>

#ifdef VSYNC_LOTTO
    #define FINAL_COUNT                                                        \
        (NTHREADS * (REACQUIRE + 1) + REENTRANT * (REACQUIRE + 1))
    #define SELECT(k, t, C) ((k) < C || ((k) == C && ((t) + 1 < (C) + 1)))
#else
    #define FINAL_COUNT     (NTHREADS + REACQUIRE + REENTRANT)
    #define SELECT(k, t, C) ((k) == 1 && ((t) + 1 < (C) + 1))
#endif

#ifdef WITH_INIT
void init(void);
#else
void
init(void)
{
}
#endif

#ifdef WITH_POST
void post(void);
#else
void
post(void)
{
}
#endif

#ifdef WITH_FINI
voidfini(void);
#else
void
fini(void)
{
}
#endif

#ifdef WITH_CS
void cs(void);

void
check(void)
{
}
#else

static vuint32_t g_cs_x = 0;
static vuint32_t g_cs_y = 0;

void
cs(void)
{
    g_cs_x++;
    g_cs_y++;
}
void
check(void)
{
    ASSERT(g_cs_x == g_cs_y);
    ASSERT(g_cs_x == FINAL_COUNT);
}
#endif


static void *
run(void *arg)
{
    vuint32_t tid = (vuintptr_t)arg;

    // By default thread 0 runs the following loop twice othread threads
    // run it only once.
    for (int i = 0; i == 0 || SELECT(i, tid, REACQUIRE); i++) {
        // By default NRECURSION == 1 and the loop executes only
        // once. If REENTRANT is defined, thread 0 will reenter the
        // critial section.
        for (int j = 0; j == 0 || SELECT(j, tid, REENTRANT); j++) {
            acquire(tid);
            cs();
        }
        for (int j = 0; j == 0 || SELECT(j, tid, REENTRANT); j++) {
            release(tid);
        }
    }
    return NULL;
}

int
main(void)
{
    pthread_t t[NTHREADS];
#ifdef DEBUG
    printf("TEST %s\n", __FILE__);
    printf("NTHREADS: %u\n", NTHREADS);
    printf("REACQUIRE: %u\n", REACQUIRE);
    printf("REENTRANT: %u\n", REENTRANT);
#endif
    init();

    for (vuintptr_t i = 0; i < NTHREADS; i++) {
        (void)pthread_create(&t[i], 0, run, (void *)i);
    }

    post();

    for (vuintptr_t i = 0; i < NTHREADS; i++) {
        (void)pthread_join(t[i], NULL);
    }

#ifdef DEBUG
    printf("FINAL: g_cs_x = %d  g_cs_y = %d  (expected %d)\n", g_cs_x, g_cs_y,
           FINAL_COUNT);
#endif

    check();
    fini();

    return 0;
}
#endif
