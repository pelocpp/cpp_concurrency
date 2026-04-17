// ===========================================================================
// Atomic_Wait_Nofify.cpp - Atomic Variable
// ===========================================================================

#include "../Logger/Logger.h"

#include <atomic>
#include <future>
#include <iostream> 
#include <mutex> 
#include <print>
#include <thread> 

std::atomic<bool> g_ready{ false };

static void func_01()
{
    Logger::log(std::cout, "Before Wait");

    g_ready.wait(false); // blocks, as long  the value is 'false'

    Logger::log(std::cout, "After Wait");
}

static void func_02()
{
    Logger::log(std::cout, "Another Thread");

    std::this_thread::sleep_for(std::chrono::seconds{ 5 });

    Logger::log(std::cout, "Storing value 'false'");

    g_ready = false;
    g_ready.notify_one();

    std::this_thread::sleep_for(std::chrono::seconds{ 5 });

    Logger::log(std::cout, "Storing value 'true'");

    g_ready = true;
    g_ready.notify_one();

    Logger::log(std::cout, "Done Thread.");
}

void test_atomic_variable_01()
{
    Logger::log(std::cout, "Start:");

    std::thread t1{ func_01 };
    std::thread t2{ func_02 };

    t1.join();
    t2.join();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================

static void test_atomic_variable()
{
    using namespace std::literals;

    constexpr std::size_t MaxTasks = 10;

    std::atomic<bool> allTasksCompleted{ false };
    std::atomic<std::size_t> completion_count{};
    std::future<void> taskFutures[MaxTasks]{};
    std::atomic<std::size_t> outstandingTaskCount{ MaxTasks };

    // spawn several tasks which take different amounts of time,
    // decrement the outstanding task count, when thread finishes
    for (std::size_t seconds{}; std::future<void>& task_future : taskFutures)
    {
        task_future = std::async(

            [&](std::size_t seconds) {

                Logger::log(std::cout, "Running ...");

                std::this_thread::sleep_for(std::chrono::seconds{ seconds });

                ++completion_count;
                --outstandingTaskCount;

                // when the task count falls to zero, notify the waiter (main thread).
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

void test_atomic_variable_02()
{
    Logger::log(std::cout, "Start:");
    test_atomic_variable();
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
