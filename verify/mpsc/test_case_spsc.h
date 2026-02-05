/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef TEST_CASE_H
#define TEST_CASE_H

/***
 * Single producer vs. single consumer.
 *
 * Enqueue 4 values. Dequeue max 4.
 * Verify dequeue order is FIFO at the end.
 */

#define MAX_DEQUEUE 3U
vuintptr_t vals[MAX_DEQUEUE] = {0};
vsize_t len                  = 0;

void pre(void){};
void
consumer(void)
{
    for (vsize_t i = 0; i < MAX_DEQUEUE; i++) {
        vuintptr_t value = deq();
        if (value) {
            vals[len++] = value;
        }
    }
}

void
p1(void)
{
    for (vuintptr_t v = 1; v <= 4; v++) {
        enq(0, v);
    }
}

void
p2(void)
{
}

void
post(void)
{
    ASSERT(len <= MAX_DEQUEUE);
    vuintptr_t last   = 0;
    vbool_t violation = false;
    for (vsize_t i = 0; i < len; i++) {
        if (vals[i] != last + 1U) {
            violation = true;
            break;
        }
        last = vals[i];
    }
    if (violation) {
        printf("Dequeue Order = {");
        for (vsize_t i = 0; i < len; i++) {
            if (i == len - 1) {
                printf("%lu", vals[i]);
            } else {
                printf("%lu, ", vals[i]);
            }
        }
        printf("}\n");
        ASSERT(0 && "Single thread FIFO violated!");
    }
}

#endif
