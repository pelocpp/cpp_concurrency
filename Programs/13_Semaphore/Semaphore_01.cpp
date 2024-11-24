// ===========================================================================
// Semaphore // Semaphore_01.cpp
// ===========================================================================

#include "../Logger/Logger.h"

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

        void scheduleJob()
        {
            Logger::log(std::cout, "ScheduleJob: Data preparing ...");

            std::this_thread::sleep_for(std::chrono::seconds{ 5 });

            Logger::log(std::cout, "ScheduleJob: Data prepared!");

            m_semaphore.release();
        }

        void executeJob()
        {
            Logger::log(std::cout, "ExecuteJob:  Waiting for data ...");

            m_semaphore.acquire();

            Logger::log(std::cout, "ExecuteJob:  Executing job ...");

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

            Logger::log(std::cout, "ExecuteJob:  Done.");
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
