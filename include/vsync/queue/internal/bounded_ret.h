/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VQUEUE_BOUNDED_RET_H
#define VQUEUE_BOUNDED_RET_H

typedef enum {
    QUEUE_BOUNDED_OK    = 0,
    QUEUE_BOUNDED_FULL  = 1,
    QUEUE_BOUNDED_EMPTY = 2,
    QUEUE_BOUNDED_AGAIN = 3,
} bounded_ret_t;

#endif
