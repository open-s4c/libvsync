/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BOILERPLATE_READER_WRITER_H
#define VSYNC_BOILERPLATE_READER_WRITER_H
/*******************************************************************************
 * Boilerplate for testing reader-writer locks
 *
 * Constants:
 *
 * - NWRITERS  : number of writer threads
 * - NREADERS  : number of reader threads
 * - WITH_INIT : define init()
 * - WITH_FINI : define fini()
 * - WITH_POST : define post()
 * - WITH_CS   : define reader_cs(tid) and writer_cs(tid)
 ******************************************************************************/
#include <vsync/atomic.h>

#ifndef NREADERS
    #ifdef VSYNC_LOTTO
        #define NREADERS 8U
    #else
        #define NREADERS 2U
    #endif
#endif

#ifndef NWRITERS
    #ifdef VSYNC_LOTTO
        #define NWRITERS 4U
    #else
        #define NWRITERS 1U
    #endif
#endif

#ifndef NTHREADS
    #define NTHREADS (NWRITERS + NREADERS)
#else
    #error "NTHREADS should not be defined"
#endif

/*******************************************************************************
 * client code
 ******************************************************************************/
#include <stdio.h>
#include <vsync/vtypes.h>
#include <pthread.h>
#include <vsync/common/assert.h>
#include <vsync/common/verify.h>

#define FINAL_COUNT NWRITERS

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
void fini(void);
#else
void
fini(void)
{
}
#endif

#ifdef WITH_CS
void reader_cs(vuint32_t tid);
void writer_cs(vuint32_t tid);
void
check(void)
{
}
void
writer_acquire(vuint32_t tid)
{
    V_UNUSED(tid);
}
void
writer_release(vuint32_t tid)
{
    V_UNUSED(tid);
}
void
reader_acquire(vuint32_t tid)
{
    V_UNUSED(tid);
}
void
reader_release(vuint32_t tid)
{
    V_UNUSED(tid);
}
#else
void writer_acquire(vuint32_t tid);
void writer_release(vuint32_t tid);
void reader_acquire(vuint32_t tid);
void reader_release(vuint32_t tid);

static vuint32_t g_cs_x = 0;
static vuint32_t g_cs_y = 0;

void
writer_cs(vuint32_t tid)
{
    V_UNUSED(tid);
    g_cs_x++;
    g_cs_y++;
}
void
reader_cs(vuint32_t tid)
{
    V_UNUSED(tid);
    unsigned a = (g_cs_x == g_cs_y);
    ASSERT(a && "g_cs_x and g_cs_y differ");
}
void
check(void)
{
    ASSERT(g_cs_x == g_cs_y);
    ASSERT(g_cs_x == FINAL_COUNT);
}
#endif


static void *
writer(void *arg)
{
    vuint32_t tid = (vuint32_t)(vuintptr_t)arg;
    writer_acquire(tid);
    writer_cs(tid);
    writer_release(tid);
    return NULL;
}

static void *
reader(void *arg)
{
    vuint32_t tid = (vuint32_t)(vuintptr_t)arg;
    reader_acquire(tid);
    reader_cs(tid);
    reader_release(tid);

    return NULL;
}

int
main(void)
{
    pthread_t t[NTHREADS];
#ifdef DEBUG
    printf("TEST %s\n", __FILE__);
    printf("NWRITERS: %u\n", NWRITERS);
    printf("NREADERS: %u\n", NREADERS);
#endif

    init();

    for (vuintptr_t i = 0; i < NWRITERS; i++) {
        (void)pthread_create(&t[i], 0, writer, (void *)i);
    }

    for (vuintptr_t i = NWRITERS; i < NTHREADS; i++) {
        (void)pthread_create(&t[i], 0, reader, (void *)i);
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
