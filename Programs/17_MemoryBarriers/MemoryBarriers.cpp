// ===========================================================================
// MemoryBarriers.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <print>
#include <string>
#include <thread>
#include <vector>

namespace MemoryBarriersIntroduction {

    static std::size_t       g_data{};
    static std::atomic<bool> g_ready{ false };

    static void producerRelaxed()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        std::println("[{}] Writing ...", tid);

        g_data = 123;                                                   // (1) write data
        g_ready.store(true, std::memory_order_relaxed);                 // (2) publish flag

        std::println("[{}] Writing done.", tid);
    }

    static void consumerRelaxed()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        std::println("[{}] Reading ...", tid);

        if (g_ready.load(std::memory_order_relaxed)) {                  // (3) consume flag
            std::println("[{}] Data: {}", tid, g_data);                 // (4) read data
        }

        std::println("[{}] Reading done.", tid);
    }

    static void test_memory_order_relaxed()
    {
        std::thread t1{ producerRelaxed };
        std::thread t2{ consumerRelaxed };

        t1.join();
        t2.join();
    }

    // -----------------------------------------------------------------------------------

    static void producerAcquireRelease()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        std::println("[{}] Writing ...", tid);

        g_data = 123;                                                   // (1) write data
        g_ready.store(true, std::memory_order_release);                 // (2) 'release'

        std::println("[{}] Writing done.", tid);
    }

    static void consumerAcquireRelease()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        std::println("[{}] Reading ...", tid);

        if (g_ready.load(std::memory_order_acquire)) {                  // (3) 'acquire'
            std::println("[{}] Data: {}", tid, g_data);                 // (4) read data
        }

        std::println("[{}] Reading done.", tid);
    }

    static void test_memory_order_acquire_release()
    {
        std::thread t1{ producerAcquireRelease };
        std::thread t2{ consumerAcquireRelease };

        t1.join();
        t2.join();
    }
}

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------

namespace MemoryBarriers_RealWorldExample_Relaxed {

    static std::atomic<int> requestCount{};
    static constexpr std::size_t NumRequests{ 10'000 };

    static void handle_request()
    {
        // get the job done...
        // std::memory_order_relaxed: We just want the number to be incremented.
        // We don't care in what order other threads see it
        // or whether other variables are "synchronized".
        requestCount.fetch_add(1, std::memory_order_relaxed);
    }

    static void test_memory_order_relaxed()
    {
        Logger::log(std::cout, "Begin");

        ScopedTimer guard{};

        std::vector<std::thread> threads;
        threads.reserve(NumRequests);

        for (std::size_t i{}; i != NumRequests; ++i) {
            threads.emplace_back(handle_request);
        }
        
        for (auto& t : threads) { 
            t.join(); 
        }

        Logger::log(std::cout, "Total requests: ", requestCount.load(std::memory_order_relaxed));
        Logger::log(std::cout, "Done.");
    }
}

// -----------------------------------------------------------------------------------

namespace MemoryBarriers_RealWorldExample_ReleaseAcquire
{
    static std::atomic<bool> g_ready{ false };
    static std::string       g_data{ "<empty>" }; // normal, non-atomare variable

    static void producer()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        Logger::log(std::cout, "Producer: data = [", g_data, "]");

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        Logger::log(std::cout, "Producer: writing data now ...");

        // (1) Write at first data, non synchronized
        g_data = "<secret password>";

        // (2) Everything I did before this point,
        // must be visible to anyone reading 'g_ready' with ACQUIRE
        g_ready.store(true, std::memory_order_release);

        Logger::log(std::cout, "Producer: Done.");
    }

    static void consumer()
    {
        std::thread::id tid{ std::this_thread::get_id() };

        Logger::log(std::cout, "Consumer: data = [", g_data, "]");

        // (3) ACQUIRE: I wait until 'ready' is true. Once that happens,
        // I guarantee that I will also see all previous write accesses (such as 'data').
        while (!g_ready.load(std::memory_order_acquire))
            ;

        // (4) Secure access: data is guaranteed "Secret password"
        Logger::log(std::cout, "Consumer: received data [", g_data, "]");
    }

    static void test_memory_order_acquire_release()
    {
        std::thread t1{ producer };
        std::thread t2{ consumer };
        t1.join();
        t2.join();
    }
}

// -----------------------------------------------------------------------------------

namespace MemoryBarriers_AnthonyWilliams
{
    auto x{ std::atomic<bool>{} };
    auto y{ std::atomic<bool>{} };

    auto z{ std::atomic<std::size_t>{} };

    auto order{ std::memory_order_seq_cst };

    static void write_x()
    {
        x.store(true, order);
    }

    static void write_y()
    {
        y.store(true, order);
    }

    static void read_x_then_y()
    {
        while (!x.load(order));
        if (y.load(order))
            ++z;
    }

    static void read_y_then_x()
    {
        while (!y.load(order));
        if (x.load(order))
            ++z;
    }

    static void test_memory_order_anthony_williams_listing_5_4()
    {
        Logger::log(std::cout, "Start");

        std::thread a{ write_x };
        std::thread b{ write_y };
        std::thread c{ read_x_then_y };
        std::thread d{ read_y_then_x };

        a.join();
        b.join();
        c.join();
        d.join();
        
        assert(z.load() != 0);
        Logger::log(std::cout, "z: ", z.load());
        Logger::log(std::cout, "Done.");
    }
}

void test_memory_barriers()
{
    MemoryBarriersIntroduction::test_memory_order_relaxed();
    MemoryBarriersIntroduction::test_memory_order_acquire_release();
    MemoryBarriers_RealWorldExample_Relaxed::test_memory_order_relaxed();
    MemoryBarriers_RealWorldExample_ReleaseAcquire::test_memory_order_acquire_release();
    MemoryBarriers_AnthonyWilliams::test_memory_order_anthony_williams_listing_5_4();
}

// ===========================================================================
// End-of-File
// ===========================================================================
