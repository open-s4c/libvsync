/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include <vsync/common/assert.h>

#include <test/thread_launcher.h>
#include <test/trace_manager.h>
#include <vsync/common/verify.h>
#include <vsync/common/dbg.h>
#include <test/listset/listset_test_interface.h>

#ifndef NTHREADS
    #error define NTHREADS
#endif

#ifndef IT
    #error define IT
#endif

#define DUMMY_LENGTH (NTHREADS * IT)

vlistset_key_t g_keys[DUMMY_LENGTH];
/******************************************************************************
 * functions
 *****************************************************************************/
void init(void);
void *run(void *args);

/******************************************************************************
 * main
 *****************************************************************************/
int
main(void)
{
    init();

    launch_threads(NTHREADS, run);

    lst_verify_traces(0);

    lsts_destroy();
}
/******************************************************************************
 * init
 *****************************************************************************/
void
init(void)
{
    vuintptr_t i = 0;
    for (i = 0; i < DUMMY_LENGTH; i++) {
        g_keys[i] = i;
    }
    lsts_init();
}
/******************************************************************************
 * run
 *****************************************************************************/
void *
run(void *args)
{
    vsize_t tid        = (vsize_t)(vuintptr_t)args;
    vsize_t success    = true;
    vsize_t i          = 0;
    vsize_t succ_count = 0;

    lst_reg(tid);
    for (i = 0; i < IT; i++) {
        if (lst_con(tid, 0, g_keys[i])) {
            success = lst_rem(tid, 0, g_keys[i]);
        } else {
            success = lst_add(tid, 0, g_keys[i]);
        }
        if (success) {
            succ_count++;
        }
    }
    lst_dereg(tid);
    DBG_GREEN("T%zu, success rate %zu/%d \n", tid, succ_count, IT);
    return NULL;
}
