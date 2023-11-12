// ===========================================================================
// Semaphore // Semaphore_01.cpp
// ===========================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <semaphore>

namespace ConcurrencyBinarySemaphore {

    class Worker
    {
    private:
        std::binary_semaphore m_semaphore;

    public:
        Worker() : m_semaphore{ 0 } {}

        void scheduleJob() {

            using namespace std::chrono_literals;

            std::cout << "scheduleJob: Data preparing ...\n";

            std::this_thread::sleep_for(5000ms);

            std::cout << "scheduleJob: Data prepared.\n";

            m_semaphore.release();
        }

        void executeJob() {

            using namespace std::chrono_literals;

            std::cout << "executeJob:  Waiting for data ...\n";

            m_semaphore.acquire();

            std::cout << "executeJob:  Executing job ...\n";

            std::this_thread::sleep_for(3000ms);

            std::cout << "executeJob:  Done.\n";
        }
    };
}

void test_binary_semaphore_01() {

    using namespace ConcurrencyBinarySemaphore;

    Worker worker{};

    std::thread t1(&Worker::scheduleJob, &worker);
    std::thread t2(&Worker::executeJob, &worker);

    t1.join();
    t2.join();
}

// ===========================================================================
// End-of-File
// ===========================================================================

