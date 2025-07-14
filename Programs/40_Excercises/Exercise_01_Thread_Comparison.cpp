// ===========================================================================
// Exercise_01_Thread_Comparison.cpp // std::thread vs. std::async
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <future>
#include <thread>
#include <vector>

// choose 'Release' mode to demonstrate difference
constexpr size_t NumThreads = 100'000;

namespace Thread_Comparison {

    static void compare_std_thread() {

        Logger::log(std::cout, "Start [std::thread]");

        ScopedTimer watch{};

        std::atomic<size_t> result;

        std::vector<std::thread> threads{};

        auto threadProc = [&](size_t value) {
            result += value;
        };

        for (size_t i{}; i != NumThreads; ++i) {

            std::thread t{ threadProc, i };

            threads.push_back(std::move(t));
        }

        for (size_t i{}; i != NumThreads; ++i) {

            threads[i].join();
        }

        Logger::log(std::cout, "Done [", result, ']');
    }

    static void compare_std_async() {

        Logger::log(std::cout, "Start [std::async]");

        ScopedTimer watch{};

        std::atomic<size_t> result;

        std::vector<std::future<void>> futures{};

        auto threadProc = [&](size_t value) {
            result += value;
            };

        for (size_t i{}; i != NumThreads; ++i) {

            std::future<void> f{ std::async (threadProc, i) };

            futures.push_back(std::move(f));
        }

        for (size_t i{}; i != NumThreads; ++i) {

            futures[i].get();
        }

        Logger::log(std::cout, "Done [", result, ']');
    }
}

// ===========================================================================

void exercise_thread_comparison()
{
    using namespace Thread_Comparison;

    Logger::log(std::cout, "Start:");
    compare_std_thread();
    compare_std_async();
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
