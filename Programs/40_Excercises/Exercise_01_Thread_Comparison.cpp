// ===========================================================================
// Exercise_01_Thread_Comparison.cpp - std::thread vs. std::async
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <future>
#include <thread>
#include <vector>

// choose 'Release' mode to demonstrate difference
#ifdef _DEBUG
static constexpr std::size_t NumThreads{ 5'000 };       // debug
#else
static constexpr std::size_t NumThreads{ 50'000 };     // release
#endif

namespace Thread_Comparison {

    static void compareStdThread() {

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

    static void compareStdAsync() {

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
    compareStdThread();
    compareStdAsync();
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
