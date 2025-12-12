/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
extern "C" {
#include <vsync/atomic.h>
}
namespace vsync
{
    template <> struct atomic<vint32_t> {
        atomic(const atomic &)                     = delete;
        atomic &operator=(const atomic &)          = delete;
        atomic &operator=(const atomic &) volatile = delete;
        atomic() : _a()
        {
        }
        atomic(vint32_t v) : _a(static_cast<vuint32_t>(v))
        {
        }
        vint32_t
        load(memory_order order = memory_order_seq_cst) volatile const noexcept
        {
            return _a.load(order);
        }
        void store(vint32_t v,
                   memory_order order = memory_order_seq_cst) volatile noexcept
        {
            _a.store(static_cast<vuint32_t>(v), order);
        }

        vint32_t operator=(vint32_t v) volatile noexcept
        {
            store(v);
            return v;
        }

        operator vint32_t() volatile const noexcept
        {
            return load();
        }

        vint32_t
        exchange(vint32_t v,
                 memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.exchange(static_cast<vuint32_t>(v), order));
        }


        bool compare_exchange_strong(
            vint32_t &expected, vint32_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) volatile noexcept
        {
            // TODO: find a way to make the cast on expected safe!
            return _a.compare_exchange_strong((vuint32_t &)(expected),
                                              static_cast<vuint32_t>(desired),
                                              order, failure);
        }
        bool compare_exchange_weak(
            vint32_t &expected, vint32_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_strong(expected, desired, order, failure);
        }

        vint32_t
        fetch_add(vint32_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_add(static_cast<vuint32_t>(v), order));
        }
        vint32_t operator+=(vint32_t v) volatile noexcept
        {
            return fetch_add(v);
        }
        // v++
        vint32_t operator++(int) volatile noexcept
        {
            return static_cast<vint32_t>(_a++);
        }
        // ++v
        vint32_t operator++() volatile noexcept
        {
            return static_cast<vint32_t>(++_a);
        }

        vint32_t
        fetch_sub(vint32_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_sub(static_cast<vuint32_t>(v), order));
        }
        vint32_t operator-=(vint32_t v) volatile noexcept
        {
            return fetch_sub(v);
        }
        // v--
        vint32_t operator--(int) volatile noexcept
        {
            return static_cast<vint32_t>(_a--);
        }
        // --v
        vint32_t operator--() volatile noexcept
        {
            return static_cast<vint32_t>(--_a);
        }

        vint32_t
        fetch_and(vint32_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_and(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator&=(vint32_t v) volatile noexcept
        {
            return fetch_and(v);
        }

        vint32_t
        fetch_or(vint32_t v,
                 memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_or(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator|=(vint32_t v) volatile noexcept
        {
            return fetch_or(v);
        }

        vint32_t
        fetch_xor(vint32_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_xor(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator^=(vint32_t v) volatile noexcept
        {
            return fetch_xor(v);
        }
        vint32_t load(memory_order order = memory_order_seq_cst) const noexcept
        {
            return _a.load(order);
        }
        void store(vint32_t v,
                   memory_order order = memory_order_seq_cst) noexcept
        {
            _a.store(static_cast<vuint32_t>(v), order);
        }

        vint32_t operator=(vint32_t v) noexcept
        {
            store(v);
            return v;
        }

        operator vint32_t() const noexcept
        {
            return load();
        }

        vint32_t exchange(vint32_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.exchange(static_cast<vuint32_t>(v), order));
        }


        bool compare_exchange_strong(
            vint32_t &expected, vint32_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) noexcept
        {
            // TODO: find a way to make the cast on expected safe!
            return _a.compare_exchange_strong((vuint32_t &)(expected),
                                              static_cast<vuint32_t>(desired),
                                              order, failure);
        }
        bool compare_exchange_weak(
            vint32_t &expected, vint32_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) noexcept
        {
            return compare_exchange_strong(expected, desired, order, failure);
        }

        vint32_t fetch_add(vint32_t v,
                           memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_add(static_cast<vuint32_t>(v), order));
        }
        vint32_t operator+=(vint32_t v) noexcept
        {
            return fetch_add(v);
        }
        // v++
        vint32_t operator++(int) noexcept
        {
            return static_cast<vint32_t>(_a++);
        }
        // ++v
        vint32_t operator++() noexcept
        {
            return static_cast<vint32_t>(++_a);
        }

        vint32_t fetch_sub(vint32_t v,
                           memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_sub(static_cast<vuint32_t>(v), order));
        }
        vint32_t operator-=(vint32_t v) noexcept
        {
            return fetch_sub(v);
        }
        // v--
        vint32_t operator--(int) noexcept
        {
            return static_cast<vint32_t>(_a--);
        }
        // --v
        vint32_t operator--() noexcept
        {
            return static_cast<vint32_t>(--_a);
        }

        vint32_t fetch_and(vint32_t v,
                           memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_and(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator&=(vint32_t v) noexcept
        {
            return fetch_and(v);
        }

        vint32_t fetch_or(vint32_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_or(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator|=(vint32_t v) noexcept
        {
            return fetch_or(v);
        }

        vint32_t fetch_xor(vint32_t v,
                           memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint32_t>(
                _a.fetch_xor(static_cast<vuint32_t>(v), order));
        }

        vint32_t operator^=(vint32_t v) noexcept
        {
            return fetch_xor(v);
        }

      private:
        vsync::atomic<vuint32_t> _a;
    };
} // namespace vsync
