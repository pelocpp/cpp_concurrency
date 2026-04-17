// ===========================================================================
// Exercise_03_ThreadPool_NumberOfThreads.cpp - 
// Calculating the number of threads in a Windows Thread Pool
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <future>  // std::future, std::async
#include <thread>  // std::thread
#include <set>     // std::set
#include <mutex>     // std::mutex


// choose 'Release' mode to demonstrate difference
#ifdef _DEBUG
static constexpr std::size_t NumThreads{ 5'000 };       // debug
#else
static constexpr std::size_t NumThreads{ 50'000 };     // release
#endif

//static std::chrono::steady_clock::time_point s_begin{};
//static bool s_loggingEnabled{ true };
static std::mutex s_mutex;
static std::set<std::thread::id> s_setOfIds;  // rename to set
static std::size_t s_count{ 0 };

namespace Thread_Pool {

    static void count_threads() {

        ScopedTimer watch{};

        std::vector<std::future<void>> futures{};

        auto threadProc = [&s_setOfIds]() {

            std::thread::id currentThreadId{ std::this_thread::get_id() };

            std::lock_guard<std::mutex> guard{ s_mutex };
            if (s_setOfIds.find(currentThreadId) == s_setOfIds.end()) {
                s_count++;
              //  s_mapIds[id] = s_nextIndex;
                s_setOfIds.insert(currentThreadId);
            }

         //   return s_mapIds[id];

        };

        for (size_t i{}; i != NumThreads; ++i) {

            std::future<void> f{ std::async(threadProc, i) };

            futures.push_back(std::move(f));
        }

        for (size_t i{}; i != NumThreads; ++i) {

            futures[i].get();
        }

        Logger::log(std::cout, "Done [", result, ']');
    }
}

// ===========================================================================

void exercise_thread_pool()
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
