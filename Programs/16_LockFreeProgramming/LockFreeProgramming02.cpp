// ===========================================================================
// LockFreeProgramming02.cpp // Lock-Free Programming
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <atomic>
#include <thread>

namespace LockFreeProgrammingIncrementDecrement {

#ifdef _DEBUG
    static constexpr size_t NumIterations = 10'000'000;     // debug
#else
    static constexpr size_t NumIterations = 10'000'000;     // release
#endif

    static void test_lock_free_programming_01()
    {
        std::atomic<long> value{};

        Logger::log(std::cout, "std::atomic: before: ", value.load());

        ScopedTimer watch{};

        std::jthread t1 { 
            [&]() {
                for (size_t n{}; n != NumIterations; ++n) {
                    ++value;
                }
            }
        };

        std::jthread t2{
        [&]() {
            for (size_t n{}; n != NumIterations; ++n) {
                --value;
            }
            }
        };

        t1.join();
        t2.join();

        Logger::log(std::cout, "std::atomic: after: ", value.load());
    }

    // =======================================================================

    template <typename T>
    static void increment(std::atomic<T>& count)
    {
        T value{ count.load(std::memory_order_relaxed) };

        while (!count.compare_exchange_weak(
            value,
            value + 1,
            std::memory_order_release,
            std::memory_order_relaxed))
        {
        }
    }

    template <typename T>
    static void decrement(std::atomic<T>& count)
    {
        T value{ count.load(std::memory_order_relaxed) };

        while (!count.compare_exchange_weak(
            value,
            value - 1,
            std::memory_order_release,
            std::memory_order_relaxed))
        {
        }
    }

    static void test_lock_free_programming_02()
    {
        std::atomic<long> value{};

        Logger::log(std::cout, "compare_exchange_weak: before: ", value.load());

        ScopedTimer watch{};

        std::jthread t1{
            [&]() mutable {
                for (size_t n{}; n != NumIterations; ++n) {
                    increment(value);
                }
            }
        };

        std::jthread t2{
        [&]() mutable {
            for (size_t n{}; n != NumIterations; ++n) {
                decrement(value);
            }
            }
        };

        t1.join();
        t2.join();

        Logger::log(std::cout, "compare_exchange_weak: after: ", value.load());
    }
}

void test_lock_free_programming_increment_decrement() {

    using namespace LockFreeProgrammingIncrementDecrement;

    test_lock_free_programming_01();
    test_lock_free_programming_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
