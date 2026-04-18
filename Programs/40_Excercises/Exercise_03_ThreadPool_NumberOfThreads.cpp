// ===========================================================================
// Exercise_03_ThreadPool_NumberOfThreads.cpp - 
// Calculating the number of threads in a Windows Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"

#include <chrono>           // for std::chrono
#include <future>           // for std::future, std::async
#include <iostream>         // for Logger::log targets
#include <mutex>            // for std::mutex, std::lock_guard
#include <thread>           // for std::thread
#include <unordered_set>    // for std::unordered_set
#include <vector>           // for std::vector

// prevent thread procedure from being too fast:
// choose sleep time of 50, 100 or 150
static constexpr std::size_t NumThreads{ 3'000 };
static constexpr std::size_t MilliSeconds{ 150 };

static void countThreadsOfPool()
{
    std::unordered_set<std::thread::id> ids{};
    std::mutex                          idsMutex{};

    std::vector<std::future<void>>      futures{};
    futures.reserve(NumThreads);

    auto threadProc = [&ids, &idsMutex]() -> void {
        std::this_thread::sleep_for(std::chrono::milliseconds{ MilliSeconds });
        std::thread::id currentThreadId{ std::this_thread::get_id() };
        {
            std::lock_guard lock{ idsMutex };
            ids.insert(currentThreadId);
        }
    };

    // use explicit launch policy to force asynchronous execution and avoid deferred tasks.
    for (std::size_t i{}; i != NumThreads; ++i) {
        std::future<void> fut{ std::async(std::launch::async, threadProc) };
        futures.emplace_back(std::move(fut));
    }

    // wait for all tasks, log any exceptions
    for (auto &future : futures) {
        try {
            future.get();
        }
        catch (const std::exception& ex) {
            Logger::log(std::cerr, "Async task threw: ", ex.what());
        }
        catch (...) {
            Logger::log(std::cerr, "Async task threw unknown exception");
        }
    }

    Logger::log(std::cout, "Number of Threads in OS Thread Pool: ", ids.size());
}

static void countThreadsWithoutPool()
{
    std::unordered_set<std::thread::id> ids{};
    std::mutex                          idsMutex{};

    std::vector<std::thread>            threads{};
    threads.reserve(NumThreads);

    auto threadProc = [&ids, &idsMutex] () -> void {
        std::this_thread::sleep_for(std::chrono::milliseconds{ MilliSeconds });
        std::thread::id currentThreadId{ std::this_thread::get_id() };
        {
            std::lock_guard lock{ idsMutex };
            ids.insert(currentThreadId);
        }
    };

    // create threads
    for (std::size_t i{}; i != NumThreads; ++i) {
        std::thread t{ threadProc };
        threads.emplace_back(std::move(t));
    }

    // join all threads (no exceptions expected from join)
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    Logger::log(std::cout, "Number of Threads acquired from OS:  ", ids.size());
}

// ===========================================================================

void exercise_thread_pool()
{
    Logger::log(std::cout, "Start:");
    countThreadsOfPool();
    countThreadsWithoutPool();
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
