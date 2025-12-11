/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
extern "C" {
#include <vsync/atomic.h>
}
namespace vsync
{
    template <> struct atomic<vint8_t> {
        atomic(const atomic &)                     = delete;
        atomic &operator=(const atomic &)          = delete;
        atomic &operator=(const atomic &) volatile = delete;
        atomic() : _a()
        {
        }
        atomic(vint8_t v) : _a(static_cast<vuint8_t>(v))
        {
        }
        vint8_t
        load(memory_order order = memory_order_seq_cst) volatile const noexcept
        {
            return _a.load(order);
        }
        void store(vint8_t v,
                   memory_order order = memory_order_seq_cst) volatile noexcept
        {
            _a.store(static_cast<vuint8_t>(v), order);
        }

        vint8_t operator=(vint8_t v) volatile noexcept
        {
            store(v);
            return v;
        }

        operator vint8_t() volatile const noexcept
        {
            return load();
        }

        vint8_t
        exchange(vint8_t v,
                 memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.exchange(static_cast<vuint8_t>(v), order));
        }


        bool compare_exchange_strong(
            vint8_t &expected, vint8_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) volatile noexcept
        {
            // TODO: find a way to make the cast on expected safe!
            return _a.compare_exchange_strong((vuint8_t &)(expected),
                                              static_cast<vuint8_t>(desired),
                                              order, failure);
        }
        bool compare_exchange_weak(
            vint8_t &expected, vint8_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_strong(expected, desired, order, failure);
        }

        vint8_t
        fetch_add(vint8_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_add(static_cast<vuint8_t>(v), order));
        }
        vint8_t operator+=(vint8_t v) volatile noexcept
        {
            return fetch_add(v);
        }
        // v++
        vint8_t operator++(int) volatile noexcept
        {
            return static_cast<vint8_t>(_a++);
        }
        // ++v
        vint8_t operator++() volatile noexcept
        {
            return static_cast<vint8_t>(++_a);
        }

        vint8_t
        fetch_sub(vint8_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_sub(static_cast<vuint8_t>(v), order));
        }
        vint8_t operator-=(vint8_t v) volatile noexcept
        {
            return fetch_sub(v);
        }
        // v--
        vint8_t operator--(int) volatile noexcept
        {
            return static_cast<vint8_t>(_a--);
        }
        // --v
        vint8_t operator--() volatile noexcept
        {
            return static_cast<vint8_t>(--_a);
        }

        vint8_t
        fetch_and(vint8_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_and(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator&=(vint8_t v) volatile noexcept
        {
            return fetch_and(v);
        }

        vint8_t
        fetch_or(vint8_t v,
                 memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_or(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator|=(vint8_t v) volatile noexcept
        {
            return fetch_or(v);
        }

        vint8_t
        fetch_xor(vint8_t v,
                  memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_xor(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator^=(vint8_t v) volatile noexcept
        {
            return fetch_xor(v);
        }
        vint8_t load(memory_order order = memory_order_seq_cst) const noexcept
        {
            return _a.load(order);
        }
        void store(vint8_t v,
                   memory_order order = memory_order_seq_cst) noexcept
        {
            _a.store(static_cast<vuint8_t>(v), order);
        }

        vint8_t operator=(vint8_t v) noexcept
        {
            store(v);
            return v;
        }

        operator vint8_t() const noexcept
        {
            return load();
        }

        vint8_t exchange(vint8_t v,
                         memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.exchange(static_cast<vuint8_t>(v), order));
        }


        bool compare_exchange_strong(
            vint8_t &expected, vint8_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) noexcept
        {
            // TODO: find a way to make the cast on expected safe!
            return _a.compare_exchange_strong((vuint8_t &)(expected),
                                              static_cast<vuint8_t>(desired),
                                              order, failure);
        }
        bool compare_exchange_weak(
            vint8_t &expected, vint8_t desired,
            memory_order order   = memory_order_seq_cst,
            memory_order failure = memory_order_seq_cst) noexcept
        {
            return compare_exchange_strong(expected, desired, order, failure);
        }

        vint8_t fetch_add(vint8_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_add(static_cast<vuint8_t>(v), order));
        }
        vint8_t operator+=(vint8_t v) noexcept
        {
            return fetch_add(v);
        }
        // v++
        vint8_t operator++(int) noexcept
        {
            return static_cast<vint8_t>(_a++);
        }
        // ++v
        vint8_t operator++() noexcept
        {
            return static_cast<vint8_t>(++_a);
        }

        vint8_t fetch_sub(vint8_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_sub(static_cast<vuint8_t>(v), order));
        }
        vint8_t operator-=(vint8_t v) noexcept
        {
            return fetch_sub(v);
        }
        // v--
        vint8_t operator--(int) noexcept
        {
            return static_cast<vint8_t>(_a--);
        }
        // --v
        vint8_t operator--() noexcept
        {
            return static_cast<vint8_t>(--_a);
        }

        vint8_t fetch_and(vint8_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_and(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator&=(vint8_t v) noexcept
        {
            return fetch_and(v);
        }

        vint8_t fetch_or(vint8_t v,
                         memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_or(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator|=(vint8_t v) noexcept
        {
            return fetch_or(v);
        }

        vint8_t fetch_xor(vint8_t v,
                          memory_order order = memory_order_seq_cst) noexcept
        {
            return static_cast<vint8_t>(
                _a.fetch_xor(static_cast<vuint8_t>(v), order));
        }

        vint8_t operator^=(vint8_t v) noexcept
        {
            return fetch_xor(v);
        }

      private:
        vsync::atomic<vuint8_t> _a;
    };
} // namespace vsync
