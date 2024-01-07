// ===========================================================================
// Simple Threading Demo (std::thread basics) // SimpleThreading_01.cpp
// ===========================================================================

#include <iostream>
#include <thread> 
#include <chrono>

#include "../Logger/Logger.h"

namespace SimpleThreading01 {

    /*
     *  std::thread basics
     */

    constexpr size_t NumIterations{ 5 };

    static void function(int value) {

        std::thread::id tid{ std::this_thread::get_id() };
        Logger::log(std::cout, "tid:  ", tid);

        for (size_t i{}; i != NumIterations; ++i) {
            Logger::log(std::cout, "in thread ", value);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        Logger::log(std::cout, "Done Thread.");
    }

    static void test_01() {

        Logger::log(std::cout, "Begin");

        std::thread::id mainTID{ std::this_thread::get_id() };
        Logger::log(std::cout, "main: ", mainTID);

        std::thread t1{ function, 1 };
        std::thread t2{ function, 2 };

        t1.join();
        t2.join();

        Logger::log(std::cout, "Done.");
    }

    static void test_02() {

        Logger::log(std::cout, "Begin");

        std::thread::id mainTID{ std::this_thread::get_id() };
        Logger::log(std::cout, "main: ", mainTID);

        std::thread t1{ function, 1 };
        std::thread t2{ function, 2 };

        t1.detach();
        t2.detach();

        Logger::log(std::cout, "Done.");

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(6s);

        Logger::log(std::cout, "Done Again.");
    }
}

void test_simple_threading_01()
{
    using namespace SimpleThreading01;
    test_01();
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
