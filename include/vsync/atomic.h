/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_ATOMIC_H
#define VSYNC_ATOMIC_H
/*******************************************************************************
 * @file atomic.h
 * @brief Rich interface of atomic operations and fences
 *
 * Atomics are implemented in custom assembly or compiler builtins.
 * See atomic/core.h for implementation the documentation of the core
 * interface. An additional dispatch layer allows for a more flexible use of the
 * interface. Refer to atomic/dispatch.h for details.
 ******************************************************************************/

#include <vsync/atomic/core.h>
#include <vsync/atomic/await.h>

#endif /* VSYNC_ATOMIC_H */
