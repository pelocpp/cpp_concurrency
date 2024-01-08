
#include <iostream>
#include <iomanip>
#include <format>
#include <vector>
#include <thread>
#include <cmath>
#include <barrier>

namespace Concurrency_Barriers_01
{
    // TODO: mit gcc checken ... noexcept

    static void test_barriers_01()
    {
        std::vector<size_t> values{ 1, 2, 3, 4, 5, 6 };

        // (noexcept needs to be used as barrier callback)
        auto printValues = [&values] () noexcept {

            std::thread::id tid{ std::this_thread::get_id() };
            std::cout << std::setw(10) << std::setfill(' ') << tid << ' ';

            for (auto val : values) {
                std::cout << std::format("{:15d}", val);
            }
            std::cout << std::endl;
        };

        // print initial values
        // printValues();

        int count{ static_cast<int>(values.size()) };  // cast needed to deal with narrowing conversion

        // initialize a barrier that prints the values
        // when all threads have done their computations
        // (Note: when zero is reached, 
        // the counter of the barrier reinitializes to the initial count again)
        std::barrier allDone
        {
            count,       // initial value of the counter
            printValues  // callback to call whenever the counter is 0
        };

        // initialize a thread for each value to compute its square root in a loop:
        std::vector<std::jthread> threads;



        //for (std::size_t index{}; index != values.size(); ++index) {
        //    
        //    threads.push_back(std::jthread { 

        //        [&, index] {

        //            for (int i{}; i != 4; ++i) {

        //                // compute square root:
        //                // values[index] = std::sqrt(values[index]);
        //                values[index] = static_cast<size_t>(std::pow(values[index], 2));

        //                // and synchronize with other threads to print values:
        //                allDone.arrive_and_wait();
        //            }
        //        }
        //    });
        //}

        auto procedure = [&] (int n) {
    
            for (int i{}; i != 4; ++i) {

                // compute power of 2
                values[n] = static_cast<size_t>(std::pow(values[n], 2));

                // and synchronize with other threads to print values:
                allDone.arrive_and_wait();
            }
        };

        for (int index{}; index != values.size(); ++index) {

            threads.push_back(std::jthread{ procedure, index });
        }

        
        for (auto& t : threads) {
            t.join();
        }

        std::cout << "Done." << std::endl;
    }
}

void test_barriers()
{
    Concurrency_Barriers_01::test_barriers_01();
}
