// ===========================================================================
// Semaphore // Semaphore_01.cpp
// ===========================================================================

#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>

namespace ConcurrencyBinarySemaphore {

    class Worker
    {
    private:
        std::binary_semaphore   m_semaphore;

    public:
        Worker() : m_semaphore{ 0 } {}

        void scheduleJob() {

            std::cout << "scheduleJob: Data preparing ...\n";

            std::this_thread::sleep_for(std::chrono::seconds(5));

            std::cout << "scheduleJob: Data prepared.\n";

            m_semaphore.release();
        }

        void executeJob() {

            std::cout << "executeJob:  Waiting for data ...\n";

            m_semaphore.acquire();

            std::cout << "executeJob:  Executing job ...\n";

            std::this_thread::sleep_for(std::chrono::seconds(3));

            std::cout << "executeJob:  Done.\n";
        }
    };
}

void test_binary_semaphore_01() {

    using namespace ConcurrencyBinarySemaphore;

    Worker worker{};

    std::jthread t1{ &Worker::scheduleJob, &worker };
    std::jthread t2{ &Worker::executeJob, &worker };
}

// ===========================================================================
// End-of-File
// ===========================================================================
