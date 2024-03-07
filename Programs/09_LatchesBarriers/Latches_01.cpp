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

    static void loopOverChar(char ch) {

        for (int j{}; j != 30; ++j) 
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
                loopOverChar('!');

                // signal that the task is done:
                allDone.count_down(); // atomically decrement counter of latch
            }
        };

        std::jthread t2 {
            [&] () { 
                loopOverChar('?');

                // signal that the task is done:
                allDone.count_down(); // atomically decrement counter of latch            
            }
        };

        // wait until all tasks are done
        allDone.wait();
        std::cout << std::endl << "All tasks done."<< std::endl;
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
                        std::cout.put(static_cast<char>('0' + i));
                        std::cout.flush();
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
