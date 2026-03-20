// ===========================================================================
// Moving a Thread object // SimpleThreading_03.cpp
// ===========================================================================

#include "../Logger/Logger.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread> 

namespace SimpleThreading03 {

    constexpr std::size_t NumIterations{ 5 };

    static void function(int value) {

        for (std::size_t i{}; i != NumIterations; ++i) {

            Logger::log(std::cout, "in thread ", value);
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        }

        Logger::log(std::cout, "Done Thread.");
    }

    static void test() {

        Logger::log(std::cout, "Begin");

        std::thread t1{ function, 1 };

        std::this_thread::sleep_for(std::chrono::seconds{ 1 });

        std::thread t2{ std::move(t1) };

        t2.join();

        Logger::log(std::cout, "Done.");
    }
}

void test_simple_threading_03()
{
    using namespace SimpleThreading03;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
