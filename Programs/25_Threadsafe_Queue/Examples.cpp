// ===========================================================================
// Examples.cpp // Threadsafe Queue
// ===========================================================================

#include <iostream>
#include <sstream>

#include "../Logger/Logger.h"

#include "ThreadsafeQueue.h"

namespace Threadsafe_Queue_Examples {

    using namespace Concurrency_ThreadsafeQueue;

    static void example_01()
    {
        ThreadsafeQueue<int> queue;

        // push some data 
        queue.push(1);
        queue.push(2);
        queue.push(3);

        // pop some data 
        int value;
        queue.tryPop(value);
        std::cout << value << std::endl;
        queue.tryPop(value);
        std::cout << value << std::endl;
        queue.tryPop(value);
        std::cout << value << std::endl;
    }

    // ------------------------------------------------------------

    constexpr int NumConsumers{ 5 };
    constexpr int NumToConsume{ 4 };
    constexpr int NumToProduce{ NumToConsume * NumConsumers };

    static void produce(ThreadsafeQueue<int>& queue)
    {
        for (int i{ 1 }; i <= NumToProduce; ++i) {
            Logger::log(std::cout, "--> ", i);
            queue.push(i);
        }
    }

    static void consume(ThreadsafeQueue<int>& queue, unsigned int id)
    {
        for (int i{}; i != NumToConsume; ++i) {
            int value{};
            queue.waitAndPop(value);
            Logger::log(std::cout, "        ", value, " <== Consumer [", id, ']');
        }
    }

    static void example_02()
    {
        using namespace Concurrency_ThreadsafeQueue;

        ThreadsafeQueue<int> queue;

        std::thread producer{ produce, std::ref(queue) };

        std::vector<std::thread> consumers;

        for (int i{}; i != NumConsumers; ++i) {

            std::thread consumer{ consume, std::ref(queue), i + 1 };
            consumers.push_back(std::move(consumer));
        }

        producer.join();

        for (auto& consumer : consumers) {
            consumer.join();
        }
    }
}

void examples()
{
    using namespace Threadsafe_Queue_Examples;

    // example_01();
    example_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
