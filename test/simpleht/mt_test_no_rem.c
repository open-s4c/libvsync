/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#define NTHREADS 24U
#define VSIMPLEHT_DISABLE_REMOVE

#include <test/map/isimple.h>
#include <stdlib.h>
#include <test/thread_launcher.h>

#define MIN_KEY 1U
#define MAX_KEY VSIMPLE_HT_CAPACITY
#define TID     MAIN_TID

vatomicsz_t g_cnt = VATOMIC_INIT(0);

void
run_operation(vsize_t tid, vuintptr_t key)
{
    vbool_t added = false;
    data_t *data  = imap_get(tid, key);
    if (!data) {
        added = imap_add(tid, key, key);
        if (added) {
            vatomicsz_inc_rlx(&g_cnt);
        }
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    for (vuintptr_t key = MIN_KEY; key <= MAX_KEY; key++) {
        run_operation(tid, key);
    }
    return NULL;
}

int
main(void)
{
    imap_init();
    launch_threads(NTHREADS, run);
    imap_destroy();
    vsize_t cnt = vatomicsz_read(&g_cnt);
    ASSERT(cnt == VSIMPLE_HT_CAPACITY);
    return 0;
}
