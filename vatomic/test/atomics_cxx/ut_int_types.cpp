/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/atomic.hpp>
#include <atomic>
#include <vector>
#include <cassert>
#include <limits>
#include <typeinfo>
#include <iostream>
#include <type_traits>

/* Success criteria is to match the behavior of std::atomic */
template <typename TT, bool IsVolatile> struct TestAtomics {
    /* we define the subject(vsync::atomic), and the mirror(std::atomic) with
     * volatile qualifier or not based on template parameter IsVolatile*/
    using SubjectType =
        typename std::conditional<IsVolatile, volatile vsync::atomic<TT>,
                                  vsync::atomic<TT>>::type;
    using MirrorType =
        typename std::conditional<IsVolatile, volatile std::atomic<TT>,
                                  std::atomic<TT>>::type;

    void ut_rw()
    {
        for (TT v : vals) {
            mirror  = v;
            subject = v;
            assert(mirror == subject);
        }

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (TT v : vals) {
                mirror.store(v, static_cast<std::memory_order>(order));
                subject.store(v, static_cast<vsync::memory_order>(order));
                assert(mirror.load(static_cast<std::memory_order>(order)) ==
                       subject.load(static_cast<vsync::memory_order>(order)));
            }
        }
    }
    void ut_xchg()
    {
        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (TT v : vals) {
                TT mirror_r =
                    mirror.exchange(v, static_cast<std::memory_order>(order));
                TT subject_r = subject.exchange(
                    v, static_cast<vsync::memory_order>(order));
                assert(mirror_r == subject_r);
                assert(mirror == subject);
            }
        }
    }
    void ut_cmpxchg()
    {
        constexpr vsize_t repeat = 3;
        bool r_var               = false;
        bool r_mirror            = false;
        TT v_var                 = 0;
        TT v_mirror              = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            for (TT val : vals) {
                for (vsize_t i = 0; i < repeat; i++) {
                    r_mirror = mirror.compare_exchange_strong(
                        v_var, val, static_cast<std::memory_order>(order),
                        static_cast<std::memory_order>(order));
                    r_var = subject.compare_exchange_strong(
                        v_mirror, val, static_cast<vsync::memory_order>(order),
                        static_cast<vsync::memory_order>(order));
                    assert(subject == mirror);
                    assert(r_mirror == r_var);
                    assert(v_var == v_mirror);
                }
            }

            for (TT val : vals) {
                for (vsize_t i = 0; i < repeat; i++) {
                    r_mirror = mirror.compare_exchange_weak(
                        v_var, val, static_cast<std::memory_order>(order),
                        static_cast<std::memory_order>(order));
                    r_var = subject.compare_exchange_weak(
                        v_mirror, val, static_cast<vsync::memory_order>(order),
                        static_cast<vsync::memory_order>(order));
                    assert(subject == mirror);
                    assert(r_mirror == r_var);
                    assert(v_var == v_mirror);
                }
            }
        }
    }

    /* We do not add arithmetic tests for bool, since arithmetic functions are
     * not defined for it */
    template <typename T = TT>
    /* enable only if the type is not bool */
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_dec()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (TT val : vals) {
            mirror = subject = val;
            r_mirror         = mirror--;
            r_var            = subject--;
            assert(mirror == subject);
            assert(r_mirror == r_var);
        }

        for (TT val : vals) {
            mirror = subject = val;
            r_mirror         = --mirror;
            r_var            = --subject;
            assert(mirror == subject);
            assert(r_mirror == r_var);
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_inc()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (TT val : vals) {
            mirror = subject = val;
            r_mirror         = mirror++;
            r_var            = subject++;
            assert(mirror == subject);
            assert(r_mirror == r_var);
        }

        for (TT val : vals) {
            mirror = subject = val;
            r_mirror         = ++mirror;
            r_var            = ++subject;
            assert(mirror == subject);
            assert(r_mirror == r_var);
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_fetch_xor()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            subject = mirror = max;
            for (TT val : vals) {
                r_mirror = mirror.fetch_xor(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_xor(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_fetch_or()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            subject = mirror = min;
            for (TT val : vals) {
                r_mirror =
                    mirror.fetch_or(val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_or(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_fetch_and()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            subject = mirror = max;
            // TODO: reverse vals start from end and go to begin
            for (TT val : vals) {
                r_mirror = mirror.fetch_and(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_and(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_add_overload()
    {
        mirror = subject = min;
        for (TT v : vals) {
            mirror += v;
            subject += v;
            assert(mirror == subject);
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_sub_overload()
    {
        mirror = subject = max;
        for (TT v : vals) {
            mirror -= v;
            subject -= v;
            assert(mirror == subject);
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_fetch_add()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            mirror = subject = min;
            for (TT val : vals) {
                r_mirror = mirror.fetch_add(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_add(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_fetch_sub()
    {
        TT r_var    = 0;
        TT r_mirror = 0;

        for (int order = vsync::memory_order_relaxed;
             order <= vsync::memory_order_seq_cst; order++) {
            mirror = subject = max;
            for (TT val : vals) {
                r_mirror = mirror.fetch_sub(
                    val, static_cast<std::memory_order>(order));
                r_var = subject.fetch_sub(
                    val, static_cast<vsync::memory_order>(order));
                assert(subject == mirror);
                assert(r_mirror == r_var);
            }
        }
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    ut_bitwise()
    {
        mirror = subject = max;
        for (TT val : vals) {
            mirror ^= val;
            subject ^= val;
            assert(mirror == subject);

            mirror |= val;
            subject |= val;
            assert(mirror == subject);

            mirror &= val;
            subject &= val;
            assert(mirror == subject);
        }
    }

    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    run_arithmetic(TestAtomics<TT, IsVolatile> &ins)
    {
        std::cout << "Arithmetic tests are active for the given type."
                  << std::endl;
        ins.ut_inc();
        ins.ut_fetch_add();
        ins.ut_add_overload();
        ins.ut_dec();
        ins.ut_fetch_sub();
        ins.ut_sub_overload();
        ins.ut_fetch_xor();
        ins.ut_fetch_or();
        ins.ut_fetch_and();
        ins.ut_bitwise();
    }
    template <typename T = TT>
    typename std::enable_if<false == std::is_same<T, bool>::value>::type
    run_arithmetic_if_supported(TestAtomics<TT, IsVolatile> &ins)
    {
        ins.run_arithmetic(ins);
    }

    template <typename T = TT>
    typename std::enable_if<std::is_same<T, bool>::value>::type
    run_arithmetic_if_supported(TestAtomics<TT, IsVolatile> &)
    {
    }

    static void run_tests()
    {
        static TestAtomics<TT, IsVolatile> ins;
        std::cout << "Testing type [" << typeid(TT).name() << "] Max = " << max
                  << " Min = " << min << " with size = " << sizeof(TT)
                  << "byte(s)" << std::endl;

        assert(ins.mirror == ins.subject);
        ins.ut_rw();
        ins.ut_xchg();
        ins.ut_cmpxchg();
        ins.run_arithmetic_if_supported(ins);
        assert(ins.mirror == ins.subject);
    }

    static constexpr TT max = std::numeric_limits<TT>::max();
    static constexpr TT min = std::numeric_limits<TT>::min();

  private:
    std::vector<TT> vals = {min, (max / 4), (max / 2), max};
    MirrorType mirror;
    SubjectType subject;
};

int
main(void)
{
    /* Run with all c++ primitive types*/
    TestAtomics<signed char, false>::run_tests();
    TestAtomics<signed char, true>::run_tests();
    TestAtomics<unsigned char, false>::run_tests();
    TestAtomics<short, false>::run_tests();
    TestAtomics<short int, false>::run_tests();
    TestAtomics<signed short, false>::run_tests();
    TestAtomics<signed short int, false>::run_tests();
    TestAtomics<unsigned short, false>::run_tests();
    TestAtomics<unsigned short int, false>::run_tests();
    TestAtomics<int, false>::run_tests();
    TestAtomics<signed, false>::run_tests();
    TestAtomics<signed int, false>::run_tests();
    TestAtomics<unsigned, false>::run_tests();
    TestAtomics<unsigned int, false>::run_tests();
#if !(defined(__APPLE__) || defined(__ARM_ARCH))
    TestAtomics<long, false>::run_tests();
    TestAtomics<long int, false>::run_tests();
    TestAtomics<signed long, false>::run_tests();
    TestAtomics<signed long int, false>::run_tests();
    TestAtomics<unsigned long, false>::run_tests();
    TestAtomics<unsigned long int, false>::run_tests();
#endif
    // TODO: TestAtomics<long long>::run_tests();
    // TODO: TestAtomics<long long int>::run_tests();
    // TODO: TestAtomics<signed long long>::run_tests();
    // TODO: TestAtomics<signed long long int>::run_tests();
    // TODO: TestAtomics<unsigned long long>::run_tests();
    // TODO: TestAtomics<unsigned long long int>::run_tests();
    TestAtomics<size_t, false>::run_tests();
    TestAtomics<bool, false>::run_tests();

    /* Run with all vatomic types*/
    TestAtomics<vint8_t, false>::run_tests();
    TestAtomics<vint16_t, false>::run_tests();
    TestAtomics<vint32_t, false>::run_tests();
    TestAtomics<vint64_t, false>::run_tests();
    TestAtomics<vuint8_t, false>::run_tests();
    TestAtomics<vuint16_t, false>::run_tests();
    TestAtomics<vuint32_t, false>::run_tests();
    TestAtomics<vsize_t, false>::run_tests();
    TestAtomics<vbool_t, false>::run_tests();

    /* Run with all vatomic types with volatile */
    TestAtomics<vint8_t, true>::run_tests();
    TestAtomics<vint16_t, true>::run_tests();
    TestAtomics<vint32_t, true>::run_tests();
    TestAtomics<vint64_t, true>::run_tests();
    TestAtomics<vuint8_t, true>::run_tests();
    TestAtomics<vuint16_t, true>::run_tests();
    TestAtomics<vuint32_t, true>::run_tests();
    TestAtomics<vsize_t, true>::run_tests();
    TestAtomics<vbool_t, true>::run_tests();
}
