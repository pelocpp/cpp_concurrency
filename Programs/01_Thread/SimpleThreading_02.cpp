// ===========================================================================
// Simple Threading Demo (4 ways to create threads) // SimpleThreading_02.cpp
// ===========================================================================

#include <iostream>
#include <thread> 
#include <chrono>

#include "../Logger/Logger.h"

/*
 *  4 ways to create a thread:
 *
 *  a) Thread with Function Pointer
 *  b) Thread with Callable Object
 *  c) Thread with Lambda
 *  d) Thread with Member Function
 */

namespace SimpleThreading02 {

    constexpr size_t NumIterations{ 5 };

    static void function(int value, size_t iterations) {

        for (size_t i{}; i != iterations; ++i) {

            Logger::log(std::cout, "in thread ", value);
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        }
        Logger::log(std::cout, "Done.");
    }

    static void test_01() {

        std::thread t{ function, 1, NumIterations };
        t.join();
        Logger::log(std::cout, "Done Version 1.");
    }

    // --------------------------------------------

    class Runnable {
    private:
        int m_value;
        size_t m_iterations;

    public:
        Runnable (int value, size_t iterations)
            : m_value{ value }, m_iterations{iterations } {}

        void operator () () const {

            for (size_t i{}; i != m_iterations; ++i) {
                Logger::log(std::cout, "in thread ", m_value);
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            }
            Logger::log(std::cout, "Done.");
        }
    };

    static void test_02() {

        std::thread t { Runnable { 2, NumIterations } };
        t.join();
        Logger::log(std::cout, "Done Version 2.");
    }

    // --------------------------------------------

    static void test_03() {

        int value{ 3 };
        size_t iterations{ NumIterations };

        std::thread t {
            [=] () {
                for (size_t i{}; i != iterations; ++i) {
                    Logger::log(std::cout, "in thread ", value);
                    std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                }
                Logger::log(std::cout, "Done.");
            }
        };

        t.join();
        Logger::log(std::cout, "Done Version 3.");
    }

    // --------------------------------------------

    class AnotherRunnable {
    private:
        int m_value;
        size_t m_iterations;

    public:
        AnotherRunnable(int value, size_t iterations)
            : m_value{ value }, m_iterations{ iterations } {}

        void run() const {

            for (size_t i{}; i != m_iterations; ++i) {

                Logger::log(std::cout, "in thread ", m_value);
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            }
            Logger::log(std::cout, "Done.");
        }
    };

    static void test_04() {

        AnotherRunnable runnable{ 4, NumIterations };
        std::thread t { &AnotherRunnable::run, &runnable };
        t.join();
        Logger::log(std::cout, "Done Version 4.");
    }

    // --------------------------------------------

    // Hints for Error Handling:
    static void test_05() {

        AnotherRunnable runnable{ 5, NumIterations };

        std::thread t1{ &AnotherRunnable::run, runnable }; // works
        
        std::thread t2{ &AnotherRunnable::run, std::move(runnable) }; // works too

        AnotherRunnable* pRunnable = new AnotherRunnable{ 5, NumIterations };  // delete is missing
        std::thread t3{ &AnotherRunnable::run, pRunnable }; // works again too

        t1.detach();
        t2.detach();
        t3.detach();

        Logger::log(std::cout, "Done Version 5.");
    }
}

void test_simple_threading_02()
{
    using namespace SimpleThreading02;
    test_01();
    test_02();
    test_03();
    test_04();
    test_05();
}

// ===========================================================================
// End-of-File
// ===========================================================================
