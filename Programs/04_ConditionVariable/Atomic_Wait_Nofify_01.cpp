// ===========================================================================
// Atomic_Wait_Nofify_01.cpp - Atomic Variable
// ===========================================================================

#include "../Logger/Logger.h"

#include <atomic>
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

void test_atomic_variable_wait_notify_01()
{
    Logger::log(std::cout, "Start:");

    std::thread t1{ func_01 };
    std::thread t2{ func_02 };

    t1.join();
    t2.join();

    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
