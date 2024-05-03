/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VATOMIC_X86_H
#define VATOMIC_X86_H

#if !defined(VATOMIC_DISABLE_POLITE_AWAIT)
    #define vatomic_cpu_pause() __asm__ volatile("pause\n" ::: "memory")
#endif

#endif /* VATOMIC_X86_H */
