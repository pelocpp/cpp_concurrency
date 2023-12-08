// ===========================================================================
// Simple_Condition_Variable_01.cpp // Condition Variable
// ===========================================================================

#include <iostream>
#include <thread> 
#include <mutex> 
#include <chrono>

#include "../Logger/Logger.h"

namespace SimpleConditionVariableDemo
{
    constexpr long NumIterations{ 5 };

    std::mutex mutex;
    std::condition_variable condition;

    void function()
    {
        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "TID:  ", tid);

        std::unique_lock<std::mutex> raii{ mutex };
        condition.wait(raii);

        Logger::log(std::cout, "Done Thread ", tid);
    }

    void test() 
    {
        std::thread::id mainTID{ std::this_thread::get_id() };
        Logger::log(std::cout, "Begin Main: ", mainTID);

        std::thread t1{ function };
        std::thread t2{ function };

        std::this_thread::sleep_for(std::chrono::seconds(2));

        condition.notify_one();     // demonstrate either this line
        // condition.notify_all();  // or this line

        std::this_thread::sleep_for(std::chrono::seconds(2));
        condition.notify_one();     // and this line

        t1.join();
        t2.join();

        Logger::log(std::cout, "Done Main.");
    }
}

void test_simple_condition_variable_01()
{
    using namespace SimpleConditionVariableDemo;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
