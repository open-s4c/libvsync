/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/vtypes.h>
#include <test/thread_launcher.h>
#include <test/skiplist/skiplist_interface.h>
#include <test/trace_manager.h>
#include <test/rand.h>

#ifndef NTHREADS
    #define NTHREADS 12
#endif

#ifndef IT
    #define IT 1000
#endif

#define KEY_RANGE     ((IT >= 100) ? ((vuintptr_t)IT / 100) : IT)
#define DS_IDX        0
#define FLIP_A_COIN() (random_thread_safe_get_next(0, 1))

void *run(void *args);
void init(vskiplist_key_t, vskiplist_key_t);
void destroy(void);

#define DS_IDX 0

int
main(void)
{
    skip_init();
    launch_threads(NTHREADS, run);
    skip_destroy();
    return 0;
}

void *
run(void *args)
{
    vsize_t tid            = (vsize_t)(vuintptr_t)args;
    vsize_t i              = 0;
    vskiplist_key_t key    = tid;
    vskiplist_node_t *node = NULL;

    skip_reg(tid);
    for (i = 0; i < IT; i++) {
        key = key % KEY_RANGE;
        skip_enter(tid);
        node = skip_lookup(tid, DS_IDX, key);
        if (node) {
            if (FLIP_A_COIN()) {
                skip_rem(tid, DS_IDX, key);
            } else {
                /* extends the lookup scope, works iff cleanup is used */
                skip_rem_node(tid, DS_IDX, key, node);
            }
        } else {
            skip_add(tid, DS_IDX, key, NULL);
        }
        skip_exit(tid);
        key++;
    }
    skip_dereg(tid);
    return NULL;
}
