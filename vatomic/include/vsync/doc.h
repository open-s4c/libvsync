/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 * Description: VSync API documention
 * Author: Huawei Dresden Research Center
 */
#ifndef VSYNC_DOC_H
#define VSYNC_DOC_H
/*******************************************************************************
 * @dir vsync
 *
 * libvsync is library of robust atomics, synchronization primitives,
 * concurrent data structures and safe memory reclamation schemes.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/common
 * @brief Macros and helpers used throughout libvsync.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/stack
 * @brief Concurrent stacks.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/bitmap
 * @brief Bitmap implementations.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/queue
 * @brief Queues, priority queues and ringbuffers.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/utils
 * @brief Utilities and supporting algorithms.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/spinlock
 * @brief Spinlocks for kernel and userspace.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/thread
 * @brief Userspace synchronization primitives.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/smr
 * @brief Safe Memory Reclamation Schemes.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/alloc
 * @brief Memory allocators.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/pool
 * @brief Memory pools.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/smr_alloc
 * @brief Memory allocators with integrated SMR schemes.
 ******************************************************************************/

/*******************************************************************************
 * @defgroup numa_aware Numa-aware
 * Group of numa-aware algorithms.
 ******************************************************************************/

/*******************************************************************************
 * @defgroup lock_free Lock-free
 * Group of algorithms with lock-free progress condition.
 ******************************************************************************/

/*******************************************************************************
 * @defgroup requires_smr SMR-required
 * Group of algorithms that require safe memory reclamation.
 * Users are expected to couple the usage of these algorithms with an SMR
 * scheme from vsync/smr e.g. vsync/smr/gdump.h .
 * Detached/retired nodes of these algorithms must be retired to the SMR
 * and not freed/recycled directly. Operations of these algorithms should
 * be called within the critical section of the SMR within enter/exit, unless
 * stated otherwise.
 ******************************************************************************/

/*******************************************************************************
 * @defgroup linearizable Linearizable
 * Group of algorithms linearizable algorithms.
 ******************************************************************************/

/*******************************************************************************
 * @defgroup unbounded_queue Unbounded-Queue
 * Group of Unbounded Queues. These queues have no capacity limit and thus
 * enqueue operations shall always succeed.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/map
 * @brief This is a collection of algos that implement map interface.
 ******************************************************************************/

/*******************************************************************************
 * @dir vsync/thread/mutex
 * @brief Different implementations of user-space mutex.
 ******************************************************************************/

#endif
