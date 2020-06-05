// ===========================================================================
// Simple Threading Demo (4 ways to create threads)
// ===========================================================================

#include <iostream>
#include <thread> 
#include <chrono>

#include "../Logger/Logger.h"

/*
 *  4 ways to create a thread:
 *
 *  a) Thread with Function Pointer
 *  b) Thread with Function Object (Functor)
 *  c) Thread with Lambda
 *  d) Thread with Member Function
 */

namespace SimpleThreading02 {

    constexpr long NumIterations = 5;

    void function(int value, int iterations) {

        for (int i = 0; i < iterations; ++i) {
            Logger::log(std::cout, "in thread ", value);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void test_01() {
        std::thread t(function, 1, NumIterations);
        t.join();
        Logger::log(std::cout, "Done Version 1.");
    }

    class Runnable {
    private:
        int m_value;
        int m_iterations;

    public:
        Runnable (int value, int iterations) 
            : m_value(value), m_iterations(iterations) {}

        void operator () () const {
            for (int i = 0; i < m_iterations; ++i) {
                Logger::log(std::cout, "in thread ", m_value);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

    void test_02() {
        // uniform initialization syntax
        std::thread t { Runnable { 2, NumIterations } };  

        // named object
        // Runnable runnable(2, NumIterations);
        // std::thread t (runnable);
        
        // temporary object
        // std::thread t (Runnable (2, NumIterations));

        t.join();
        Logger::log(std::cout, "Done Version 2.");
    }

    void test_03() {
        int value = 3;
        int iterations = 5;
        std::thread t([value, iterations] () {
            for (int i = 0; i < iterations; ++i) {
                Logger::log(std::cout, "in thread ", value);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

        t.join();
        Logger::log(std::cout, "Done Version 3.");
    }

    class AnotherRunnable {
    private:
        int m_value;
        int m_iterations;

    public:
        AnotherRunnable(int value, int iterations)
            : m_value(value), m_iterations(iterations) {}

        void run() const {
            for (int i = 0; i < m_iterations; ++i) {
                Logger::log(std::cout, "in thread ", m_value);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

    void test_04() {
        AnotherRunnable runnable(4, NumIterations);
        std::thread t (&AnotherRunnable::run, &runnable);
        t.join();
        Logger::log(std::cout, "Done Version 4.");
    }
}

//int main()
//{
//    using namespace SimpleThreading02;
//    test_01();
//    test_02();
//    test_03();
//    test_04();
//    return 1;
//}

// ===========================================================================
// End-of-File
// ===========================================================================
