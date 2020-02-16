// ===========================================================================
// Producer Consumer Problem
// ===========================================================================

#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>
#include <chrono>
#include <thread>

#include "../Logger/Logger.h"

std::queue<int> dataQueue;
std::mutex queueMutex;

void producerFunction() {
    // This function will keep generating data forever
    int newNumber;
    while (true) {
        // Wait from 1 to 3 seconds before generating data
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Add a number to the queue
        newNumber = rand() % 100 + 1; // Random number from 1 to 100

        std::cout << "PRODUCER - vor" << std::endl;
        std::lock_guard<std::mutex> g(queueMutex);
        std::cout << "PRODUCER - nach" << std::endl;

        dataQueue.push(newNumber);

        std::cout << "Added number to queue: " << newNumber << std::endl;
    }
}

void consumerFunction() {
    // This function will consume data forever
    while (true) {
        int numberToProcess = 0;

        // We only need to lock the mutex for the time it takes us to pop an item
        // out. Adding this scope releases the lock right after we poped the item
        {
            std::cout << "consumer - vor" << std::endl;
            std::lock_guard<std::mutex> g(queueMutex);
            std::cout << "consumer - nach" << std::endl;
            if (!dataQueue.empty()) {
                numberToProcess = dataQueue.front();
                dataQueue.pop();
            }
        }

        // Only process if there are numbers
        if (numberToProcess) {
            std::cout << "Processing number: " << numberToProcess << std::endl;
        }
    }
}

//int main() {
//    std::thread producer(producerFunction);
//    std::thread consumer1(consumerFunction);
//
//    producer.join();
//    consumer1.join();
//}

// ===========================================================================
// End-of-File
// ===========================================================================
