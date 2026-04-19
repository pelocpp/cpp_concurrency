// ===========================================================================
// Spin Locks // SpinLock.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <atomic>
#include <chrono>
#include <latch>
#include <mutex>
#include <thread>
#include <vector>

namespace SpinLocks {

    class Spinlock
    {
    private:
        // Ensure atomic_flag is properly initialized on all supported standards.
        // Use the macro-style initializer which is compatible pre-C++20.
        std::atomic_flag m_atomic_flag = ATOMIC_FLAG_INIT;

    public:
        Spinlock() noexcept = default; // default ctor is fine now

        // Prevent accidental copying (avoid undefined behavior from copying a lock object)
        Spinlock(const Spinlock&) = delete;
        Spinlock& operator=(const Spinlock&) = delete;

        void lock() noexcept
        {
            // Busy-wait, but yield the remainder of this thread's timeslice
            // to avoid starving other threads / burning 100% CPU.
            while (m_atomic_flag.test_and_set(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
        }

        void unlock() noexcept
        {
            m_atomic_flag.clear(std::memory_order_release);
        }
    };
}

namespace TestSpinLocksCommon
{

#ifdef _DEBUG
    const std::size_t MaxIterations = 100'000;
    const std::size_t NumWorkers = 10;
#else
    const std::size_t MaxIterations = 1'000'000;
    const std::size_t NumWorkers = 10;
#endif

    using ValueType = unsigned long;

    ValueType value{};

    std::atomic<ValueType> valueAtomic;

    std::mutex mutex;

    std::latch spinlocksDone{ NumWorkers };
    std::latch atomicDone{ NumWorkers };
    std::latch mutexDone{ NumWorkers };
}

namespace TestUsingSpinLocks
{
    using namespace TestSpinLocksCommon;
    using namespace SpinLocks;

    Spinlock spinlock;

    static void spinlockTask(std::size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        // signal that this task is waiting for working
        spinlocksDone.count_down();

        for (std::size_t i{}; i != iterations; ++i)
        {
            spinlock.lock();
            value++;
            spinlock.unlock();
        }
    }
}

namespace TestUsingAtomics
{
    using namespace TestSpinLocksCommon;

    static void atomicTask(std::size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        // signal that this task is waiting for working
        atomicDone.count_down();

        for (std::size_t i{}; i != iterations; ++i)
        {
            valueAtomic++;
        }
    }
}

namespace TestUsingMutex
{
    using namespace TestSpinLocksCommon;

    static void mutexTask(std::size_t iterations)
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "Task ", tid, " started ...");

        // signal that this task is waiting for working
        mutexDone.count_down();

        for (std::size_t i{}; i != iterations; ++i)
        {
            std::lock_guard<std::mutex> guard{ mutex };
            value++;
        }
    }
}

void test_using_spinlocks() {

    using namespace SpinLocks;
    using namespace TestUsingSpinLocks;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "SpinLock test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);
    
    value = 0;
    
    for (std::size_t i{}; i != NumWorkers; ++i) {
        std::thread thread{ spinlockTask, MaxIterations };
        threads.push_back(std::move(thread));
    }

    // wait until all tasks have been initialized
    spinlocksDone.wait();

    {
        ScopedTimer watch{};

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    if (value == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

void test_using_atomics() {

    using namespace TestSpinLocksCommon;
    using namespace TestUsingAtomics;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "std::atomic<> test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);

    valueAtomic = 0;

    for (std::size_t i{}; i != NumWorkers; ++i) {
        std::thread thread{ atomicTask, MaxIterations };
        threads.push_back(std::move(thread));
    }

    // wait until all tasks have been initialized
    atomicDone.wait();

    {
        ScopedTimer watch{};

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    ValueType result = valueAtomic;

    if (result == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

void test_using_mutex() {

    using namespace TestSpinLocksCommon;
    using namespace TestUsingMutex;

    std::vector<std::thread> threads;

    Logger::log(std::cout, "std::mutex test started");
    Logger::log(std::cout, "Iterations: ", MaxIterations);

    value = 0;

    for (std::size_t i{}; i != NumWorkers; ++i) {
        std::thread thread{ mutexTask, MaxIterations };
        threads.push_back(std::move(thread));
    }

    // wait until all tasks have been initialized
    mutexDone.wait();

    {
        ScopedTimer watch{};

        for (auto& thread : threads) {
            thread.join();
        }
    }

    Logger::log(std::cout, "Threads done.");

    if (value == MaxIterations * NumWorkers)
    {
        Logger::log(std::cout, "Test passed successfully!");
    }
    else
    {
        Logger::log(std::cout, "Test FAILED!");
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
