// ===========================================================================
// Atomic_Wait_Nofify_02.cpp - Atomic Variable
// ===========================================================================

#include "../Logger/Logger.h"

#include <atomic>
#include <future>
#include <thread> 

static void test_atomic_variable()
{
    using namespace std::literals;

    constexpr std::size_t MaxTasks{ 10 };

    std::atomic<bool>         allTasksCompleted{ false };
    std::atomic<std::size_t>  completion_count{};
    std::future<void>         taskFutures[MaxTasks]{};
    std::atomic<std::size_t>  outstandingTaskCount{ MaxTasks };

    // spawn several tasks which take different amounts of time,
    // decrement the outstanding task count, when thread finishes
    for (std::size_t seconds{ 5 }; auto& taskFuture : taskFutures)
    {
        taskFuture = std::async(

            [&](std::size_t seconds) {

                std::thread::id tid{ std::this_thread::get_id() };
                Logger::log(std::cout, "Running ... [", tid, "]");

                std::this_thread::sleep_for(std::chrono::seconds{ seconds });

                ++completion_count;

                --outstandingTaskCount;

                // when the task count falls to zero, notify the waiter (main thread)
                if (outstandingTaskCount.load() == 0)
                {
                    allTasksCompleted = true;
                    allTasksCompleted.notify_one();
                }

                Logger::log(std::cout, "Done.");

            },
            seconds
        );

        ++seconds;
    }

    allTasksCompleted.wait(false);

    Logger::log(std::cout, "Tasks completed: ", completion_count);
}

void test_atomic_variable_wait_notify_02()
{
    Logger::log(std::cout, "Start:");
    test_atomic_variable();
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
