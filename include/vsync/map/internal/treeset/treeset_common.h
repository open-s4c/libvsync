/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_TREESET_COMMON_H
#define VSYNC_TREESET_COMMON_H

#include <vsync/vtypes.h>
#include <vsync/common/compiler.h>
#include <vsync/common/dbg.h>

typedef vuintptr_t treeset_key_t;

struct treeset_node_s;

typedef void (*treeset_visitor)(treeset_key_t key, void *value, void *arg);

#endif
