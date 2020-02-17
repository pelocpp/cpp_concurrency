// ===========================================================================
// Producer Consumer Problem
// ===========================================================================

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <chrono>
#include <thread>

#include "../Logger/Logger.h"

namespace ProducerConsumer {

    void test() {

        std::condition_variable cond;
        std::mutex mutex;

        std::queue<int> intQueue;
        bool stopped = false;

        std::thread producer{ [&]()
        {
            std::size_t count = 5;
            while (count--)
            {
                using namespace std::chrono_literals;
                {
                    // Always lock before changing state
                    // guarded by a mutex and condition_variable
                    std::lock_guard<std::mutex> lock{ mutex };

                    intQueue.push(count);
                    std::cout << "Producer pushed: " << count << std::endl;

                    // Tell the consumer it has an int
                    cond.notify_one();
                }

                std::cout << "Sleeping ... " << count << std::endl;
                std::this_thread::sleep_for(2s);
            }

            // All done.
            // Acquire the lock, set the stopped flag,
            // then inform the consumer.
            std::lock_guard<std::mutex> L{ mutex };

            std::cout << "Producer is done!" << std::endl;

            stopped = true;
            cond.notify_one();
        } 
        };

        std::thread consumer{ [&]()
        {
            do {
                std::unique_lock<std::mutex> lock { mutex };

                cond.wait(lock ,[&]()
                {
                    // Acquire the lock only if we've stopped
                    // or the queue isn't empty
                    bool condition = stopped || !intQueue.empty();

                    // return false if waiting should be continued ...
                    std::cout << "Waking up ... [" << condition  << "]" << std::endl;

                    return condition;
                });

                std::cout << "testing rest ...." << std::endl;

                // We own the mutex here; pop the queue
                // until it empties out.

                while (!intQueue.empty())
                {
                    const auto val = intQueue.front();
                    intQueue.pop();

                    std::cout << "Consumer popped: " << val << std::endl;
                }

                if (stopped) {
                    // producer has signaled a stop
                    std::cout << "Consumer is done!" << std::endl;
                    break;
                }

            } while (true);
        } 
        };

        consumer.join();
        producer.join();

        std::cout << "Example Completed!" << std::endl;
    }

}

//int main()
//{
//    using namespace ProducerConsumer;
//    test();
//    return 1;
//}

// ===========================================================================
// End-of-File
// ===========================================================================
