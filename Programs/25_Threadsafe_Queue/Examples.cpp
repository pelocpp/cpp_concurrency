// ===========================================================================
// Examples.cpp // Threadsafe Queue
// ===========================================================================

#include <iostream>
#include <sstream>

#include <functional>


#include "ThreadsafeQueue.h"


namespace Threadsafe_Queue_Examples {

    using namespace Concurrency_ThreadsafeQueue;

    void example_01()
    {
        ThreadsafeQueue<int> q;

        // push some data 
        q.push(1);
        q.push(2);
        q.push(3);

        // pop some data 
        int value;
        q.try_pop(value);
        std::cout << value << std::endl;
        q.try_pop(value);
        std::cout << value << std::endl;
        q.try_pop(value);
        std::cout << value << std::endl;
    }

    // das gehört zu
    // https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/


    const int nbConsumers = 4;
    const int nbToConsume = 3;
    const int nbToProduce = nbToConsume * nbConsumers;

    void print(std::string x) {
        static std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        std::cout << x << "\n";
    }


    void produce(ThreadsafeQueue<int>& q) {
        for (int i = 1; i <= nbToProduce; ++i) {
            std::ostringstream tmp;
            tmp << "--> " << i;
            print(tmp.str());
            q.push(i);
        }
    }

    void consume(ThreadsafeQueue<int>& q, unsigned int id) {
        for (int i = 0; i < nbToConsume; ++i) {
            int value{};
            q.wait_and_pop(value);
            std::ostringstream tmp;
            tmp << "        " << value << " --> C" << id;
            print(tmp.str());
        }
    }


    void example_02()
    {
        using namespace Concurrency_ThreadsafeQueue;

        ThreadsafeQueue<int> q;

        // start the producer thread.
        std::thread prod1(std::bind(produce, std::ref(q)));    // #include <functional>

        // Start the consumer threads.
        std::vector<std::thread> consumers;
        for (int i = 0; i < nbConsumers; ++i) {
            std::thread consumer(std::bind(&consume, std::ref(q), i + 1));
            consumers.push_back(std::move(consumer));
        }

        prod1.join();

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
