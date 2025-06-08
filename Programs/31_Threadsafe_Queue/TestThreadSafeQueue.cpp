// ===========================================================================
// TestThreadSafeQueue.cpp // Threadsafe Queue
// ===========================================================================

#include "../Logger/Logger.h"
#include "ThreadsafeQueue.h"

#include <iostream>
#include <sstream>

void test_thread_safe_queue_01()
{
    using namespace Concurrency_ThreadsafeQueue;

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

static constexpr size_t NumConsumers{ 4 };
static constexpr size_t NumToConsume{ 3 };
static constexpr size_t NumToProduce{ NumToConsume * NumConsumers };

void test_thread_safe_queue_02()
{
    using namespace Concurrency_ThreadsafeQueue;

    ThreadsafeQueue<size_t> queue;

    auto produce = [&] () {
        for (int i{ 1 }; i <= NumToProduce; ++i) {
            Logger::log(std::cout, "--> ", i);
            queue.push(i);
        }
    };

    auto consume = [&](size_t id) {
        for (size_t i{}; i != NumToConsume; ++i) {
            size_t value{};
            queue.waitAndPop(value);
            Logger::log(std::cout, "        ", value, " <== Consumer [", id, ']');
        }
    };

    std::thread producer{ produce };

    std::vector<std::thread> consumers;

    for (size_t i{}; i != NumConsumers; ++i) {

        std::thread consumer{ consume, i + 1 };
        consumers.push_back(std::move(consumer));
    }
    
    producer.join();

    for (auto& consumer : consumers) {
        consumer.join();
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
