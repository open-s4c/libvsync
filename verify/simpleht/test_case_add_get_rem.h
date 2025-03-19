/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TEST_CASE_H
#define VSYNC_TEST_CASE_H
#define MIN_KEY 1U
#define MAX_KEY VSIMPLE_HT_CAPACITY

#define CONFLICT_KEY (MIN_KEY + 1U)
#define NEW_VAL      999U

vbool_t g_added      = false;
vbool_t g_got_nw_val = false;

void
pre(void)
{
    for (vuintptr_t k = MIN_KEY; k <= MAX_KEY; k++) {
        vbool_t success = imap_add(MAIN_TID, k, k);
        ASSERT(success);
    }
}
void
t0(vsize_t tid)
{
    g_added = imap_add(tid, CONFLICT_KEY, NEW_VAL);
}
void
t1(vsize_t tid)
{
    vbool_t success = imap_rem(tid, CONFLICT_KEY);
    ASSERT(success);
}
void
t2(vsize_t tid)
{
    data_t *data = imap_get(tid, CONFLICT_KEY);
    if (data) {
        if (data->val == NEW_VAL) {
            g_got_nw_val = true;
        }
    }
}
void
post(void)
{
    for (vuintptr_t k = MIN_KEY; k <= MAX_KEY; k++) {
        data_t *d = imap_get(MAIN_TID, k);
        if (k == CONFLICT_KEY) {
            if (g_added) {
                ASSERT(d);
                ASSERT(d->val == NEW_VAL);
            } else if (d) {
                ASSERT(d->val == k);
            }
        } else {
            ASSERT(d);
        }
    }
    if (g_got_nw_val) {
        ASSERT(g_added);
    }
}
#endif
