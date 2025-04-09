/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 * Author: Huawei Dresden Research Center
 */
#ifndef VATOMIC_DOC_H
#define VATOMIC_DOC_H
/*******************************************************************************
 * @dir vsync/atomic
 * @brief Rich interface of atomic operations and fences.
 *
 * VSync atomics (VAtomics) implements a rich interface of atomic operations
 * and fences. It employs efficient custom assembly for supported architectures,
 * and falls back to compiler builtins otherwise.
 *
 * ### Atomic types
 *
 * core.h implements the following atomic types:
 *
 * | Atomic type (A) | Related type (T) |
 * | --------------- | ---------------- |
 * | vatomic8_t      | vuint8_t         |
 * | vatomic16_t     | vuint16_t        |
 * | vatomic32_t     | vuint32_t        |
 * | vatomic64_t     | vuint64_t        |
 * | vatomicptr_t    | void*            |
 *
 * Functions are always prefixed with the atomic type, e.g., vatomic32_read(),
 * vatomic64_read(), vatomicptr_read(). Arguments or return values typically are
 * of related types, e.g., `vatomic64_write(vatomic64_t *a, vuint64_t v)`.
 *
 * The types `vuint32_t` and `vuint64_t` map to equivalent types from <stdint.h>
 * unless VSYNC_ENABLE_FREESTANDING is defined.
 *
 * Note that a few functions are not defined for vatomicptr_t, e.g., add, sub,
 * etc.
 *
 * ### Memory orders
 *
 * By default, atomic functions are *seq_cst* (sequentially consistent). To
 * specify another memory order, add the corresponding suffix to the function
 * name:
 *
 * | Mode    | Suffix |
 * | ------- | ------ |
 * | acquire | `_acq` |
 * | release | `_rel` |
 * | relaxed | `_rlx` |
 *
 * @note Not all functions support every memory order. See the function
 * documentation for more information.
 *
 * ### Components
 *
 * VSync atomics are divided in several components.
 *
 * For the declaration and definition of the core atomic operations and
 * fences, include core.h (as in `#include <vsync/atomic/core.h>`).
 *
 * For a set of advanced atomic operations optimized for politely waiting
 * (spinning), include @ref await.h.
 *
 * For readability and ease of use, include @ref dispatch.h. It introduces
 * several macro dispatchers prefixed with vatomic_. These dispatchers map to
 * the call to the respective vatomic8_, vatomic16_, vatomic32_ or vatomi64_
 * functions depending on the type/size of the arguments. For example,
 * vatomic_read(a) is the same as vatomic32_read(a) if the type of `a` is
 * vatomic32_t.
 *
 * More advanced stamped and marked atomic pointers are available in
 * @ref atomicptr_stamped.h and @ref atomicptr_markable.h.
 *
 * ### Implementation variants
 *
 * Optimized atomic implementations are available for ARMv7 (32 and 64 bits) and
 * ARMv8 (32 and 64 bits). For all other architectures, including x86_64, the
 * fallback is `__atomic` compiler builtins. To force the use of compiler
 * builtins, define VATOMIC_BUILTINS.
 *
 * For further configuration flags, please refer to @ref config.h.
 *
 * @example
 * @include eg_core.c
 ******************************************************************************/

#endif /* VATOMIC_DOC_H */
