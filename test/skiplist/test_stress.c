/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <vsync/vtypes.h>
#include <test/thread_launcher.h>
#include <test/skiplist/skiplist_interface.h>
#include <test/trace_manager.h>

#ifndef NTHREADS
    #define NTHREADS 12
#endif

#ifndef IT
    #define IT 1000
#endif
#define START_KEY 0
#define INIT_SIZE IT

void *run(void *args);
void init(vskiplist_key_t, vskiplist_key_t);
void destroy(void);

#define DS_IDX              0
#define GET_NEXT_KEY(_key_) (((_key_) + 1) % INIT_SIZE)

int
main(void)
{
    init(START_KEY, INIT_SIZE);
    launch_threads(NTHREADS, run);
    skip_destroy();
    return 0;
}

void
init(vskiplist_key_t start_key, vskiplist_key_t end_key)
{
    vskiplist_key_t key    = 0;
    vbool_t success        = false;
    vsize_t tid            = NTHREADS;
    vskiplist_node_t *node = NULL;

    skip_init();
    for (key = start_key; key <= end_key; key++) {
        success = skip_add(tid, DS_IDX, key, &node);
        ASSERT(success);
    }
}

void
adder(vsize_t tid)
{
    vsize_t i           = 0;
    vskiplist_key_t key = START_KEY;

    for (i = 0; i < IT; i++) {
        skip_enter(tid);
        skip_add(tid, DS_IDX, key, NULL);
        skip_exit(tid);
        key = GET_NEXT_KEY(key);
    }
}

void
remover(vsize_t tid)
{
    vsize_t i           = 0;
    vskiplist_key_t key = START_KEY;

    for (i = 0; i < IT; i++) {
        skip_enter(tid);
        skip_rem(tid, DS_IDX, key);
        skip_exit(tid);
        key = GET_NEXT_KEY(key);
    }
}

void
finder(vsize_t tid)
{
    vsize_t i           = 0;
    vskiplist_key_t key = START_KEY;

    for (i = 0; i < IT; i++) {
        skip_enter(tid);
        skip_lookup(tid, DS_IDX, key);
        skip_exit(tid);
        key = GET_NEXT_KEY(key);
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    skip_reg(tid);
    switch (tid % 3) {
        case 0:
            adder(tid);
            break;
        case 1:
            remover(tid);
            break;
        case 2:
            finder(tid);
            break;
        default:
            break;
    }
    skip_dereg(tid);
    return NULL;
}
