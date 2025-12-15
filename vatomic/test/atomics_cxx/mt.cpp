/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/atomic.hpp>
#include <thread>
#include <iostream>
#include <functional>
#include <chrono>
#include <atomic>
#include <limits>
#if !defined(__ARM_ARCH) && !defined(__NetBSD__)
    #define USE_BARRIER
#endif
#include <vector>
#if defined(USE_BARRIER)
    #if defined(__cpp_lib_barrier)
        #include <barrier>
using vsync_thread_barrier = std::barrier<>;
    #else
        #include <condition_variable>
        #include <mutex>
class vsync_thread_barrier
{
  public:
    explicit vsync_thread_barrier(size_t thread_count)
        : threshold(thread_count), count(thread_count), generation(0)
    {
    }

    void arrive_and_wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        auto gen = generation;
        if (--count == 0U) {
            generation++;
            count = threshold;
            cv.notify_all();
        } else {
            cv.wait(lock, [this, gen] { return generation != gen; });
        }
    }

  private:
    const size_t threshold;
    size_t count;
    size_t generation;
    std::mutex mutex;
    std::condition_variable cv;
};
    #endif
#endif

template <typename TT, size_t N, size_t IT> class MT_Test
{
#if defined(__APPLE__)
    using time_point = int64_t;
#else
    using time_point = std::chrono::_V2::system_clock::time_point;
#endif


  public:
    static void run_tests()
    {
        static MT_Test<TT, N, IT> ins;
        ins.mt_store();
        ins.mt_xchg();
        ins.mt_inc();
        ins.mt_add();
        ins.mt_dec();
        ins.mt_sub();
        ins.mt_cmpxchg();
    }


  private:
    static constexpr int64_t tolerate = 0;
    static constexpr TT OP_COUNT      = IT * N;
    void info(std::string name, intptr_t vsync_t, intptr_t std_t)
    {
        std::cout << "[" << name << "] vsync: " << vsync_t
                  << "ms, std: " << std_t << "ms";
        if (vsync_t <= std_t) {
            std::cout << " [winner: \033[1;32mvsync_t\033[0m ("
                      << std_t - vsync_t << ")]";
        } else {
            std::cout << " [winner: \033[1;31mstd_t\033[0m(" << vsync_t - std_t
                      << ")]";
        }
        std::cout << " with #threads: " << N << ", #Iter: " << IT << std::endl;
    }
    void mt_store()
    {
        constexpr TT v = std::numeric_limits<TT>::max();
        auto vsync_t   = test([&] { subject = v; });
        auto std_t     = test([&] { mirror = v; });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
    }

    void mt_xchg()
    {
        constexpr TT v = std::numeric_limits<TT>::max();
        auto vsync_t   = test([&] { subject.exchange(v); });
        auto std_t     = test([&] { mirror.exchange(v); });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
    }

    void mt_cmpxchg()
    {
        constexpr TT init_val = std::numeric_limits<TT>::min();

        mirror = subject = init_val;
        auto vsync_t     = test([&] {
            TT e = init_val;
            while (!subject.compare_exchange_strong(e, e + 1)) {}
        });
        auto std_t       = test([&] {
            TT e = init_val;
            while (!mirror.compare_exchange_strong(e, e + 1)) {}
        });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
        assert(mirror = (init_val + OP_COUNT));
    }

    void mt_inc()
    {
        auto constexpr init_val = std::numeric_limits<TT>::min();
        mirror = subject = init_val;
        auto vsync_t     = test([&] { subject++; });
        auto std_t       = test([&] { mirror++; });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
        assert(mirror == (init_val + OP_COUNT));
    }

    void mt_add()
    {
        constexpr TT init_val = std::numeric_limits<TT>::min();
        constexpr TT step     = 3;
        mirror = subject = init_val;
        auto vsync_t     = test([&] { subject += step; });
        auto std_t       = test([&] { mirror += step; });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
        assert(mirror == (init_val + (OP_COUNT * step)));
    }

    void mt_dec()
    {
        auto constexpr init_val = std::numeric_limits<TT>::max();
        mirror = subject = init_val;
        auto vsync_t     = test([&] { subject--; });
        auto std_t       = test([&] { mirror--; });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
        assert(mirror == (init_val - OP_COUNT));
    }


    void mt_sub()
    {
        constexpr TT init_val = std::numeric_limits<TT>::max();
        constexpr TT step     = 3;
        mirror = subject = init_val;
        auto vsync_t     = test([&] { subject -= step; });
        auto std_t       = test([&] { mirror -= step; });
        info(__FUNCTION__, vsync_t, std_t);
        assert(subject == mirror);
        assert(mirror == (init_val - (OP_COUNT * step)));
    }

    inline int64_t diff(time_point start, time_point end)
    {
        auto duration = end - start;
#if defined(__APPLE__)
        return duration;
#else
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count();
#endif
    }
    int64_t test(std::function<void(void)> f)
    {
#ifdef USE_BARRIER
        vsync_thread_barrier barrier(N);
#endif
        std::vector<std::thread> threads;
        auto start = launch(threads, [&] {
#ifdef USE_BARRIER
            barrier.arrive_and_wait();
#endif
            for (auto i = 0; i < IT; i++) {
                f();
            }
        });
        auto end = join(threads);
        return diff(start, end);
    }


    inline time_point now()
    {
#if defined(__APPLE__)
        return 0;
#else
        return std::chrono::high_resolution_clock::now();
#endif
    }
    time_point launch(std::vector<std::thread> &threads,
                      std::function<void(void)> f)
    {
        auto start = now();
        for (size_t i = 0; i < N; i++) {
            threads.push_back(std::thread(f));
        }
        return start;
    }
    time_point join(std::vector<std::thread> &threads)
    {
        for (auto &t : threads) {
            t.join();
        }
        return now();
    }
    std::atomic<TT> subject;
    vsync::atomic<TT> mirror;
};

int
main(void)
{
    constexpr size_t IT = 1000000;
    constexpr size_t N  = 12;
    std::cout << "hardware_concurrency: " << std::thread::hardware_concurrency()
              << " " << std::endl;
    MT_Test<int, N, IT>::run_tests();
    MT_Test<vuint64_t, N, IT>::run_tests();
    return 0;
}
