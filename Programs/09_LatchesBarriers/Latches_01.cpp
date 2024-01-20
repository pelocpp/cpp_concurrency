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

namespace Latches_01 {

    static void loopOver(char ch) {

        for (int j{}; j != 10; ++j) {
            // loop printing the char ch
            std::cout.put(ch).flush();
            std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
        }
    }

    static void example_latches_01()
    {
        std::array<char, 5> tags = { '1', '2', '3', '4', '5' };   // tags we have to perform a task for

        // initialize latch to react when all tasks are done
        // (initialize countdown with number of tasks)
        std::latch allDone{ tags.size() };

        // start two threads dealing with every second tag
        std::jthread t1 {
            [&] () {
                for (size_t i{}; i < tags.size(); i += 2) { // even indexes

                    loopOver(tags[i]);

                    // signal that the task is done:
                    allDone.count_down(); // atomically decrement counter of latch
                }
            }
        };

        std::jthread t2 {
            [&] () {
                for (size_t i{ 1 }; i < tags.size(); i += 2) { // odd indexes

                    loopOver(tags[i]);

                    // signal that the task is done:
                    allDone.count_down(); // atomically decrement counter of latch
                }
            }
        };

        // wait until all tasks are done
        std::cout << "Waiting until all tasks are done:\n";
        allDone.wait();
        std::cout << "\nAll tasks done.";

        t1.join();  // wait until end of threads
        t2.join();
    }
}

namespace Latches_02 {

    static void example_latches_02()
    {
        std::ptrdiff_t numThreads{ 10 };

        // initialize latch to start the threads when all of them have been initialized
        // (initialize countdown with number of threads)
        std::latch allReady{ numThreads };

        // start numThreads threads:
        std::vector<std::jthread> threads;

        for (int i{}; i != numThreads; ++i) {

            std::jthread t{

                [i, &allReady]() {

                    // initialize each thread (simulate to take some time):
                    std::this_thread::sleep_for(std::chrono::milliseconds{ 300 * i });

                    std::ostringstream ss;
                    ss << "JThread " << i << " waiting for execution\n";
                    std::string s{ ss.str() };
                    std::cout.write(s.c_str(), s.size()).flush();

                    // synchronize threads so that all start together here:
                    allReady.arrive_and_wait();

                    // or
                    //allReady.count_down();
                    //allReady.wait();

                    // perform whatever the thread does
                    // (loop printing its index):
                    for (int j{}; j != 10; ++j) {
                        std::cout.put(static_cast<char>('0' + i)).flush();
                        std::this_thread::sleep_for(std::chrono::milliseconds{ 50 });
                    }
                }
            };

            threads.push_back(std::move(t));
        }
    }
}

void test_latches_01()
{
    using namespace Latches_01;
    example_latches_01();
}

void test_latches_02()
{
    using namespace Latches_02;
    example_latches_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
