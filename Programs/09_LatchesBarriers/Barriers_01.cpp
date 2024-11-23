// ===========================================================================
// Barriers_01.cpp
// ===========================================================================

#include <iostream>
#include <iomanip>
#include <format>
#include <vector>
#include <thread>
#include <cmath>
#include <barrier>

namespace Concurrency_Barriers_01
{
    static void test_barriers()
    {
        std::cout << "Start:" << std::endl;

        std::vector<size_t> values{ 1, 2, 3, 4, 5, 6 };

        
        auto printValues = [&values]() noexcept {
            // noexcept needs to be used for barrier function object

            std::thread::id tid{ std::this_thread::get_id() };

            std::cout << std::setw(10) << std::setfill(' ') << tid << ' ';

            for (auto val : values) {
                std::cout << std::format("{:15d}", val);
            }
            std::cout << std::endl;
        
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        };

        // print initial values
        printValues();

        // cast needed to deal with narrowing conversion
        int count{ static_cast<int>(values.size()) };

        // initialize a barrier that prints the values
        // when all threads have done their computations.
        // Note: when zero is reached, the counter of the barrier
        // reinitializes to the initial count again.

        std::barrier allDone
        {
            count,       // initial value of the counter value
            printValues  // completion function object to be called whenever the counter is 0
        };

        // initialize a thread for each value to compute its square root in a loop
        std::vector<std::jthread> threads;

        auto calculate = [&](size_t n) {

            for (int i{}; i != 4; ++i) {

                // compute powers of 2 );
                values[n] = static_cast<size_t>(std::pow(values[n], 2));

                // synchronize with other threads to print values
                allDone.arrive_and_wait();
            }
        };

        for (size_t index{}; index != values.size(); ++index) {

            std::jthread t{ calculate, index };
            threads.push_back(std::move(t));
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "Done." << std::endl;
    }
}

void test_barriers_01()
{
    using namespace Concurrency_Barriers_01;
    test_barriers();
}

// ===========================================================================
// End-of-File
// ===========================================================================
