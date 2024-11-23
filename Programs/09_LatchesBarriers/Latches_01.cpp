// ===========================================================================
// Latches_01.cpp
// ===========================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <thread>
#include <latch>
#include <array>
#include <vector>

#include "../Logger/Logger.h"

namespace Latches_01 {

    static void loop(char ch, size_t count) {

        for (int j{}; j != count; ++j)
        {
            // loop printing the char ch
            std::cout.put(ch);
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
        }
    }

    static void example_latches_01()
    {
        std::latch allDone{ 2 };

        std::cout << "Waiting until all tasks are done:" << std::endl;

        // start two threads dealing with printing chars
        std::jthread t1 {
            [&] () {
                loop('!', 30);

                // signal that this task is done
                allDone.count_down(); // atomically decrement counter of latch
            }
        };

        std::jthread t2 {
            [&] () { 
                loop('?', 30);

                // signal that this task is done
                allDone.count_down(); // atomically decrement counter of latch            
            }
        };

        // wait until all tasks have finished
        allDone.wait();

        std::cout << std::endl << "All tasks done."<< std::endl;
    }

    static void example_latches_02()
    {
        Logger::log(std::cout, "Start:");

        std::ptrdiff_t numThreads{ 10 };

        // initialize latch to start the threads
        // when all of them have been initialized
        std::latch allReady{ numThreads };

        // start numThreads threads:
        std::vector<std::jthread> threads;

        for (int i{}; i != numThreads; ++i) {

            std::jthread t{

                [i, &allReady]() {

                    Logger::log(std::cout, "JThread ", i, " started (", std::this_thread::get_id(), ")");

                    // initialize each thread (simulate to take some time):
                    std::this_thread::sleep_for(std::chrono::milliseconds{ 700 * i });

                    Logger::log(std::cout, "JThread ", i, " waiting for execution");

                    // synchronize threads so that all start together here
                    allReady.count_down();
                    allReady.wait();

                    // or
                    // allReady.arrive_and_wait();
                    
                    // perform whatever the thread does (loop printing its index)
                    char ch{ static_cast<char>('0' + i) };
                    loop(ch, 10);
                }
            };

            threads.push_back(std::move(t));
        }

        Logger::log(std::cout, "Done.");
    }
}

void test_latches_01()
{
    using namespace Latches_01;
    example_latches_01();
}

void test_latches_02()
{
    using namespace Latches_01;
    example_latches_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
