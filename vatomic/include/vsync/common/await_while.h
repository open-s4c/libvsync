/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_AWAIT_WHILE_H
#define VSYNC_AWAIT_WHILE_H

#if defined(VSYNC_VERIFICATION) && !defined(VSYNC_DISABLE_SPIN_ANNOTATION)
    #include <vsync/common/internal/await_while.h>
#elif defined(VSYNC_LOTTO) && !defined(LOTTO_DISABLE_SPIN_ANNOTATION)
    #include <lotto/await_while.h>
#else
    #define await_while(cond) while (cond)
    #define await_do          do
    #define while_await(cond) while (cond)
    #define await_break       break
#endif

#endif /* VSYNC_AWAIT_WHILE_H */
