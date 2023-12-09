// ===========================================================================
// Simple_Condition_Variable_01.cpp // Condition Variable
// ===========================================================================

#include <iostream>
#include <thread> 
#include <mutex> 
#include <chrono>

#include "../Logger/Logger.h"

namespace SimpleConditionVariableDemo01
{
    std::mutex g_mutex;
    std::condition_variable g_condition;

    bool g_data{ false };

    void consume()
    {
        Logger::log(std::cout, "Function Consume:");

        {
            std::unique_lock<std::mutex> raii{ g_mutex };

            g_condition.wait(raii, []() {
                Logger::log(std::cout, "  ... check for data being present ...");
                return g_data == true;
                }
            );
        }

        Logger::log(std::cout, "Data has been consumed ...");

        Logger::log(std::cout, "Done Thread ");
    }

    void produce()
    {
        Logger::log(std::cout, "Function Produce:");

        {
            std::lock_guard<std::mutex> guard (g_mutex);

            g_data = true;
        }

        Logger::log(std::cout, "Data has been produced ...");

        g_condition.notify_one();

        Logger::log(std::cout, "Done Thread ");
    }

    void test() 
    {
        Logger::log(std::cout, "Begin Main:");

        std::thread t1{ produce };  // or consume

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
        std::thread t2{ consume };  // or produce

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
