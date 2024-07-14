// ===========================================================================
// Semaphore // Semaphore_01.cpp
// ===========================================================================

#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>
#include <print>

namespace ConcurrencyBinarySemaphore {

    class Worker
    {
    private:
        std::binary_semaphore m_semaphore;

    public:
        Worker() : m_semaphore{ 0 } {}

        void scheduleJob() {

            std::println("ScheduleJob: Data preparing ...");

            std::this_thread::sleep_for(std::chrono::seconds{ 5 });

            std::println("ScheduleJob: Data prepared!");

            m_semaphore.release();
        }

        void executeJob() {

            std::println("ExecuteJob:  Waiting for data ...");

            m_semaphore.acquire();

            std::println("ExecuteJob:  Executing job ...");

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

            std::println("ExecuteJob:  Done.");
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
