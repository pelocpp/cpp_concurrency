#include "ThreadsafeQueue.h"

int main()
{
    using namespace Concurrency_ThreadsafeQueue;

    ThreadsafeQueue<int> q;

    // Push some data 
    q.push(1);
    q.push(2);
    q.push(3);

    // Pop some data 
    //std::cout << q.pop() << std::endl;
    //std::cout << q.pop() << std::endl;
    //std::cout << q.pop() << std::endl; 

    return 0;
}

// das gehört zu
// https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/


int example()
{
    Queue<int> q;

    // Start the producer thread.
    std::thread prod1(std::bind(produce, std::ref(q)));

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