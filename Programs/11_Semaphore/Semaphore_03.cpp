// ===========================================================================
// Semaphore // Semaphore_03.cpp
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>
#include <print>

namespace ConcurrencyCountingSemaphore {

    static void test_counting_semaphore_01()
    {
        std::mutex mutex{};

        // initialize a queue with multiple sequences from ’A’ to ’Z’

        std::queue<char> values{};

        for (size_t i{}; i != 1000; ++i) {
            char ch{ static_cast < char>('A' + (i % ('Z' - 'A'))) };
            values.push(ch);
        }

        constexpr int numThreads{ 8 };

        std::counting_semaphore<numThreads> enabled{ 0 };

        // create a thread pool
        std::vector<std::jthread> pool{};

        auto procedure = [&](std::stop_token token, int n) {

            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "> tid:  ", tid);

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

                // print the value 10 times
                for (int i{}; i != 10; ++i) {

                    if (token.stop_requested()) {
                        break;
                    }

                    std::print("{}", ch);

                    auto duration{ std::chrono::milliseconds{ 300 } *((n % 3) + 1) };
                    std::this_thread::sleep_for(std::chrono::milliseconds{ duration });
                }

                // remove thread from the set of enabled threads
                enabled.release();
            }

            Logger::log(std::cout, "< tid:  ", tid);
        };

        // create and start all threads of the pool
        for (int i{}; i != numThreads; ++i) {

            std::jthread t{ procedure, i };
            pool.push_back(std::move(t));
        }

        // now play with the threads

        Logger::log(std::cout, "> wait 2 seconds (no thread enabled)");

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // enable 4 concurrent threads:
        Logger::log(std::cout, "\n> enable 4 parallel threads");

        enabled.release(4);

        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        // enable 4 more concurrent threads
        Logger::log(std::cout, "\n> enable 4 more parallel threads");

        enabled.release(4);
        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        // we could run forever, but let’s end the program here:
        Logger::log(std::cout, "\n> stop processing");

        for (auto& t : pool) {
            t.request_stop();
        }

        Logger::log(std::cout, "\n> wait for end of threads");

        for (auto& t : pool) {
            t.join();
        }

        Logger::log(std::cout, "\n>  Done.");
    }
}

void test_counting_semaphore() {

    using namespace ConcurrencyCountingSemaphore;

    test_counting_semaphore_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
