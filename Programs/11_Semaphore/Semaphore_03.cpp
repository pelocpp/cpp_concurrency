// ===========================================================================
// Semaphore // Semaphore_03.cpp
// ===========================================================================

#include <iostream>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>

#include "../Logger/Logger.h"

namespace ConcurrencyCountingSemaphore {

    static void test_counting_semaphore_01()
    {
        std::mutex mutex;

        // initialize a queue with multiple sequences from ’A’ to ’Z’

        std::queue<char> values;

        for (size_t i{}; i != 1000; ++i) {
            values.push(static_cast<char>('A' + (i % ('Z' - 'A'))));
        }

        constexpr int numThreads{ 10 };

        std::counting_semaphore<numThreads> enabled{ 0 };

        // create a thread pool
        std::vector<std::jthread> pool;

        auto procedure = [&](std::stop_token token, int n) {

            while (!token.stop_requested()) {

                // request this thread to become one of the enabled threads
                enabled.acquire();

                // get next value from the queue:
                char ch{};
                {
                    std::lock_guard<std::mutex> guard{ mutex };
                    ch = values.front();
                    values.pop();
                }

                // print the value 10 times:
                for (int i{}; i != 10; ++i) {

                    std::cout.put(ch).flush();

                    auto duration = std::chrono::milliseconds{ 300 } *((n % 3) + 1);
                    std::this_thread::sleep_for(std::chrono::milliseconds{ duration });
                }

                // remove thread from the set of enabled threads
                enabled.release();
            }
        };

        // create and start all threads of the pool
        for (int i{}; i != numThreads; ++i) {

            std::jthread t{ procedure, i };
            pool.push_back(std::move(t));
        }

        // now play with the threads
        std::cout << ">  wait 3 seconds (no thread enabled)\n" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        // enable 3 concurrent threads:
        std::cout << ">  enable 3 parallel threads\n" << std::flush;
        enabled.release(3);
        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        // enable 2 more concurrent threads:
        std::cout << "\n>  enable 4 more parallel threads\n" << std::flush;
        enabled.release(4);
        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        // Normally we would run forever, but let’s end the program here:
        std::cout << "\n>  stop processing\n" << std::flush;
        for (auto& t : pool) {
            t.request_stop();
        }

        std::cout << ">  wait for end of threads\n" << std::flush;
        for (auto& t : pool) {
            t.join();
        }

        std::cout << "\n>  Done.\n" << std::flush;
    }
}

void test_counting_semaphore() {

    using namespace ConcurrencyCountingSemaphore;

    test_counting_semaphore_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
