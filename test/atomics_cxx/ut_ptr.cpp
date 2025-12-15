/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/atomic.hpp>
#include <iostream>
#include <atomic>
#include <cassert>
#include <vector>

struct Student {
    static constexpr size_t length = 20;
    int id;
    int age;
    char name[length];
};

template <typename TT> struct TestPtrAtomic {
    void ut_rw()
    {
        mirror = subject = nullptr;
        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (size_t i = 0; i < length; i++) {
                mirror.store(&arr[i], static_cast<std::memory_order>(order));
                subject.store(&arr[i], static_cast<vsync::memory_order>(order));

                assert(mirror.load(static_cast<std::memory_order>(order)) ==
                       subject.load(static_cast<vsync::memory_order>(order)));
            }
        }
    }
    void ut_xchg()
    {
        TT *r_var    = 0;
        TT *r_mirror = 0;
        subject = mirror = nullptr;
        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (size_t i = 0; i < length; i++) {
                r_mirror = mirror.exchange(
                    &arr[i], static_cast<std::memory_order>(order));
                r_var = subject.exchange(
                    &arr[i], static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    void ut_cmpxchg()
    {
        constexpr size_t repeat = 3;
        bool r_var              = false;
        bool r_mirror           = false;
        TT *v_var               = nullptr;
        TT *v_mirror            = nullptr;

        mirror = subject = nullptr;
        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (size_t i = 0; i < length; i++) {
                v_var = v_mirror = nullptr;
                for (vsize_t i = 0; i < repeat; i++) {
                    r_mirror = mirror.compare_exchange_strong(
                        v_var, &arr[i], static_cast<std::memory_order>(order),
                        static_cast<std::memory_order>(order));
                    r_var = subject.compare_exchange_strong(
                        v_mirror, &arr[i],
                        static_cast<vsync::memory_order>(order),
                        static_cast<vsync::memory_order>(order));
                    assert(subject == mirror);
                    assert(r_mirror == r_var);
                    assert(v_var == v_mirror);
                }
            }
        }

        mirror = subject = nullptr;
        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (size_t i = 0; i < length; i++) {
                v_var = v_mirror = nullptr;
                for (vsize_t i = 0; i < repeat; i++) {
                    r_mirror = mirror.compare_exchange_weak(
                        v_var, &arr[i], static_cast<std::memory_order>(order),
                        static_cast<std::memory_order>(order));
                    r_var = subject.compare_exchange_weak(
                        v_mirror, &arr[i],
                        static_cast<vsync::memory_order>(order),
                        static_cast<vsync::memory_order>(order));
                    assert(subject == mirror);
                    assert(r_mirror == r_var);
                    assert(v_var == v_mirror);
                }
            }
        }
    }
    void ut_fetch_add()
    {
        TT *r_var    = 0;
        TT *r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            mirror = subject = &arr[0];
            for (auto val : offsets) {
                r_mirror = mirror.fetch_add(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_add(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    void ut_fetch_sub()
    {
        TT *r_var    = 0;
        TT *r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            mirror = subject = &arr[length - 1];
            for (auto val : offsets) {
                r_mirror = mirror.fetch_sub(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_sub(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    void ut_inc()
    {
        mirror = subject = &arr[0];
        for (size_t i = 0; i < length; i++) {
            auto r_m = mirror++;
            auto r_s = subject++;
            assert(r_m == r_s);
            assert(subject == mirror);
            if (i < length - 1) {
                assert(mirror == &arr[i + 1]);
            }
        }
        mirror = subject = &arr[0];
        for (size_t i = 0; i < length; i++) {
            auto r_m = ++mirror;
            auto r_s = ++subject;
            assert(r_m == r_s);
            assert(subject == mirror);
            if (i < length - 1) {
                assert(mirror == &arr[i + 1]);
            }
        }
    }
    void ut_dec()
    {
        mirror = subject = &arr[length - 1];
        for (size_t i = length - 1; i < length; i--) {
            auto r_m = mirror--;
            auto r_s = subject--;
            assert(r_m == r_s);
            assert(subject == mirror);
            if (i > 0) {
                assert(mirror == &arr[i - 1]);
            }
        }
        mirror = subject = &arr[length - 1];
        for (size_t i = length - 1; i < length - 1; i--) {
            auto r_m = --mirror;
            auto r_s = --subject;
            assert(r_m == r_s);
            assert(subject == mirror);
            if (i > 0) {
                assert(mirror == &arr[i - 1]);
            }
        }
    }
    void ut_add_overload()
    {
        constexpr size_t step = 2;
        for (size_t i = 0; i < length; i += step) {
            mirror = subject = &arr[0];
            mirror += i;
            subject += i;
            assert(mirror == &arr[i]);
        }
    }
    void ut_sub_overload()
    {
        constexpr size_t step = 2;
        for (size_t i = length - 1; i < length - 1; i -= step) {
            mirror = subject = &arr[length - 1];
            mirror -= i;
            subject -= i;
            assert(subject == mirror);
            assert(mirror == &arr[i]);
        }
    }
    static void run_tests()
    {
        static TestPtrAtomic<TT> ins;
        std::cout << "Testing atomicptr with type [" << typeid(TT).name()
                  << " with size = " << sizeof(TT) << "byte(s)" << std::endl;

        assert(ins.mirror == ins.subject);
        ins.ut_rw();
        ins.ut_xchg();
        ins.ut_inc();
        ins.ut_fetch_add();
        ins.ut_add_overload();
        ins.ut_dec();
        ins.ut_fetch_sub();
        ins.ut_sub_overload();
        ins.ut_cmpxchg();
        assert(ins.mirror == ins.subject);
    }

  private:
    static constexpr size_t length = 1000;
    std::atomic<TT *> mirror;
    vsync::atomic<TT *> subject;
    TT arr[length]{};
    std::vector<ptrdiff_t> offsets = {0, 1, 2, 4, 7};
};

int
main(void)
{
    TestPtrAtomic<int *>::run_tests();
    TestPtrAtomic<char *>::run_tests();
    TestPtrAtomic<Student *>::run_tests();
    return 0;
}
