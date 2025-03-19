/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef QUEUE_PRIO_DISPATCH_H
#define QUEUE_PRIO_DISPATCH_H

#if defined(VQUEUE_PRIO_STACK_ARRAY_BASED)
    #include <vsync/queue/vqueue_prio_stack_array_based.h>
typedef vsize_t vpriority_t;
    #define BM_TARGET_NAME "stack_arr_based"
#elif defined(VQUEUE_PRIO_STACK_TREE_BASED)
    #include <vsync/queue/vqueue_prio_stack_tree_based.h>
typedef vsize_t vpriority_t;
    #define BM_TARGET_NAME "stack_tree_based"
#elif defined(VQUEUE_PRIO_SKIPLIST_BASED)
    #include <vsync/queue/vqueue_prio_skiplist_based.h>
typedef vskiplist_key_t vpriority_t;
    #define BM_TARGET_NAME "skiplist_based"
#elif defined(VQUEUE_PRIO_HEAP_BASED)
    #include <vsync/queue/vqueue_prio_heap_based.h>
typedef vuint32_t vpriority_t;
    #define BM_TARGET_NAME "heap_based"
    #define SMR_NONE
#else
    #error "QUEUE_PRIO implementation is not defined"
#endif

#endif
