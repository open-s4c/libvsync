/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

void
pre(void)
{
}
void
t0(vsize_t tid)
{
    vbool_t success = imap_add(tid, 1, 1);
    ASSERT(success);
}
void
t1(vsize_t tid)
{
    vbool_t success = imap_add(tid, 2, 2);
    ASSERT(success);
}
void
t2(vsize_t tid)
{
    vbool_t success = imap_add(tid, 3, 3);
    ASSERT(success);
}
void
post(void)
{
    for (vuintptr_t k = 1; k <= 3; k++) {
        data_t *d = imap_get(MAIN_TID, k);
        ASSERT(d);
        ASSERT(d->val == k);
    }
}
