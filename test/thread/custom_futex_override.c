/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#define FUTEX_CUSTOM
#include <vsync/thread/internal/futex.h>

static vatomic32_t g_word = VATOMIC_INIT(0);
static vuint32_t g_wait_count;
static vuint32_t g_wake_count;
static vuint32_t g_wait_value;
static vuint32_t g_wake_value;

void
vfutex_wait(vatomic32_t *m, vuint32_t v)
{
    ASSERT(m == &g_word);
    g_wait_count++;
    g_wait_value = v;
}

void
vfutex_wake(vatomic32_t *m, vuint32_t v)
{
    ASSERT(m == &g_word);
    g_wake_count++;
    g_wake_value = v;
}

int
main(void)
{
    vfutex_wait(&g_word, 7U);
    vfutex_wake(&g_word, FUTEX_WAKE_ONE);

    ASSERT(g_wait_count == 1U);
    ASSERT(g_wake_count == 1U);
    ASSERT(g_wait_value == 7U);
    ASSERT(g_wake_value == FUTEX_WAKE_ONE);

    return 0;
}
