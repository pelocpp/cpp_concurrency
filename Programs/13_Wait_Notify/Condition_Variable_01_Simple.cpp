// ===========================================================================
// Condition_Variable_01_Simple.cpp // Condition Variable
// ===========================================================================

#include <iostream>
#include <thread> 
#include <mutex> 
#include <chrono>

#include "../Logger/Logger.h"

namespace SimpleConditionVariableDemo01
{
    std::mutex mutex{};

    std::condition_variable condition{};

    bool data{ false };

    static void consume()
    {
        Logger::log(std::cout, "Function Consume:");

        {
            std::unique_lock<std::mutex> guard{ mutex };

            condition.wait(
                guard,
                [] () {
                    Logger::log(std::cout, "  ... check for data being present ...");
                    return data == true;
                }
            );
        }

        Logger::log(std::cout, "Data has been consumed ...");

        Logger::log(std::cout, "Done Thread ");
    }

    static void produce()
    {
        Logger::log(std::cout, "Function Produce:");

        // put into comment ... or not
        std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });

        {
            std::lock_guard<std::mutex> guard{ mutex };

            data = true;
        }

        Logger::log(std::cout, "Data has been produced ...");

        condition.notify_one();

        Logger::log(std::cout, "Done Thread ");
    }

    static void test()
    {
        Logger::log(std::cout, "Function Main:");

        std::thread t1{ produce };
        std::thread t2{ consume };

        t1.join();
        t2.join();

        Logger::log(std::cout, "Done Main.");
    }
}

void test_simple_condition_variable_01()
{
    using namespace SimpleConditionVariableDemo01;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
