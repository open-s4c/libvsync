/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef TEST_CASE_H
#define TEST_CASE_H

/**
 * Two producers vs. single consumer.
 *
 * Ensure message passing is sound.
 * Ensure FIFO at the end.
 *
 */

#define MAX_DEQUEUE 2U
vuintptr_t vals[MAX_DEQUEUE] = {0};
vsize_t len                  = 0;

int msg = 0;

#define VALID_COMB_LEN 7U
vuintptr_t valid_com[VALID_COMB_LEN][MAX_DEQUEUE] = {
    {3, 4}, {1, 2}, {1, 3}, {3, 1}, {0, 0}, {1, 0}, {3, 0}};

vbool_t
is_valid(void)
{
    for (vsize_t i = 0; i < VALID_COMB_LEN; i++) {
        vbool_t match = true;
        for (vsize_t j = 0; j < MAX_DEQUEUE; j++) {
            if (valid_com[i][j] != vals[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}

int
_deq(void)
{
    vuintptr_t v = deq();
    if (v) {
        vals[len++] = v;
    }
    return v;
}

void
pre(void)
{
}

void
consumer(void)
{
    vuintptr_t a = _deq();
    vuintptr_t b = _deq();
    if (b == 4) {
        assert(msg == 11);
    }
    V_UNUSED(a);
}

void
p1(void)
{
    vsize_t tid = 0;
    enq(tid, 1);
    enq(tid, 2);
}

void
p2(void)
{
    vsize_t tid = 1;
    enq(tid, 3);
    msg = 11;
    enq(tid, 4);
}

void
post(void)
{
    vbool_t fifo_valid = is_valid();
    if (!fifo_valid) {
        printf("{");
        for (vsize_t i = 0; i < len; i++) {
            printf("%lu, ", vals[i]);
        }
        printf("}\n");
        ASSERT(0 && "FIFO violated!");
    }
}
#endif
