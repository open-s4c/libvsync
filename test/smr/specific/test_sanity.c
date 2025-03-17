/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */


#include <vsync/common/dbg.h>
#include <vsync/common/compiler.h>
#include <test/thread_launcher.h>
#include <test/smr/ismr.h>

/*
 * Ensure everything that was handed in to SMR is freed
 * Ensure retirement kept things safe till exit
 */

void *
run(void *args)
{
    mock_node_t *mock = NULL;
    vsize_t i         = 0;
    vsize_t tid       = (vsize_t)(vuintptr_t)args;

    ismr_reg(tid);

    for (i = 0; i < TST_IT; i++) {
        ismr_enter(tid);

        mock = mock_node_alloc();

        ismr_retire(&mock->smr_node, mock_node_free, false);
        mock->x = tid;

        ismr_exit(tid);

        ismr_recycle(tid);
    }

    ismr_dereg(tid);
    return NULL;
}

int
main(void)
{
    ismr_init();
    launch_threads(SMR_MAX_NTHREADS, run);
    mock_node_no_leak();
    ismr_destroy();
    ASSERT(mock_node_no_leak());
    return 0;
}
