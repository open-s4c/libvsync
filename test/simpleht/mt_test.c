/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define NTHREADS 24U

#include <test/map/isimple.h>
#include <stdlib.h>
#include <test/thread_launcher.h>

#define MIN_KEY 1U
#define MAX_KEY VSIMPLE_HT_CAPACITY
#define TID     MAIN_TID

void
run_operation(vsize_t tid, vuintptr_t key)
{
    data_t *data = NULL;
    data         = imap_get(tid, key);
    if (data) {
        imap_rem(tid, key);
    } else {
        imap_add(tid, key, key);
    }
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;

    imap_reg(tid);

    for (vuintptr_t key = MIN_KEY; key <= MAX_KEY; key++) {
        run_operation(tid, key);
    }

    imap_dereg(tid);

    return NULL;
}

int
main(void)
{
    imap_init();
    launch_threads(NTHREADS, run);
    imap_destroy();
    return 0;
}
