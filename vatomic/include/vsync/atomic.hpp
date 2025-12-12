/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef VSYNC_ATOMIC_HPP
#define VSYNC_ATOMIC_HPP

#include <cassert>
extern "C" {
#include <vsync/atomic.h>
}
namespace vsync
{
    typedef enum memory_order {
        memory_order_relaxed,
        memory_order_consume,
        memory_order_acquire,
        memory_order_release,
        memory_order_acq_rel,
        memory_order_seq_cst
    } memory_order;

    template <typename T> struct atomic;

    void atomic_thread_fence(vsync::memory_order order)
    {
        switch (order) {
            case memory_order_consume:
            case memory_order_acquire:
                vatomic_fence_acq();
                break;
            case memory_order_relaxed:
                vatomic_fence_rlx();
                break;
            case memory_order_release:
                vatomic_fence_rel();
                break;
            case memory_order_acq_rel:
            case memory_order_seq_cst:
                vatomic_fence();
                break;
            default:
                assert(0 && "Order is not supported");
        }
    }
} // namespace vsync

#include "atomic/core_u8.hpp"
#include "atomic/core_bool.hpp"
#include "atomic/core_u16.hpp"
#include "atomic/core_u32.hpp"
#if defined(__APPLE__)
    #include "atomic/core_sz.hpp"
#endif
#include "atomic/core_u64.hpp"
#include "atomic/core_ptr.hpp"
#include "atomic/core_s8.hpp"
#include "atomic/core_s16.hpp"
#include "atomic/core_s32.hpp"
#include "atomic/core_s64.hpp"
#endif
