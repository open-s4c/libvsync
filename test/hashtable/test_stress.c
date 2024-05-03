/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <test/hashtable/map.h>
#include <test/thread_launcher.h>

#define EVICT      10
#define EVICT_FREQ 10

void *
run(void *args)
{
    vsize_t tid         = (vsize_t)(vuintptr_t)args;
    vsize_t succ_insert = 0;
    vsize_t succ_remove = 0;

    map_reg(tid);
    for (vsize_t i = 0; i < IT; i++) {
        map_enter(tid);
        user_key_t key = gen_key();
        if (map_get(tid, key)) {
            succ_remove += map_remove(tid, key) ? 1 : 0;
        } else {
            succ_insert += map_insert(tid, key) ? 1 : 0;
            if (succ_insert % EVICT_FREQ == 0) {
                (void)map_evict(tid, EVICT);
            }
        }
        map_exit(tid);
    }
    map_dereg(tid);
    DBG_GREEN("[T%zu] removed %zu, inserted %zu\n", tid, succ_remove,
              succ_insert);

    return NULL;
}

int
main(void)
{
    map_init();
    launch_threads(NTHREADS, run);
    map_destroy();
    return 0;
}
