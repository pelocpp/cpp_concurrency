// ===========================================================================
// Atomic Operations (std::atomic<> and std::mutex)
// ===========================================================================

#include <iostream>
#include <thread> 
#include <atomic>
#include <mutex>

#include "../Logger/Logger.h"

namespace IncrementDecrement {

    constexpr long NumIterations = 100000000;

    class IncDec {
    public:
        // c'tors
        IncDec() : m_counter(0), m_counterAtomic(0) {}

        void run() {
            Logger::log(std::cout, "Counter: ", m_counter);

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1(&IncDec::increment, this);
            std::thread t2(&IncDec::decrement, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counter);
        }

        void runAtomic() {
            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1(&IncDec::incrementAtomic, this);
            std::thread t2(&IncDec::decrementAtomic, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());
        }

        void runMutex() {
            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1(&IncDec::incrementMutex, this);
            std::thread t2(&IncDec::decrementMutex, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());
        }

        void runMutexRAII() {
            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1(&IncDec::incrementMutexRAII, this);
            std::thread t2(&IncDec::decrementMutexRAII, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());
        }

    private:
        long m_counter = 0;
        std::atomic<long> m_counterAtomic;
        std::mutex m_mutex;

    private:
        // private helper methods
        void increment() {
            Logger::log(std::cout, "> increment");
            for (long n = 0; n < NumIterations; ++n) {
                ++m_counter;
            }
            Logger::log(std::cout, "< increment");
        }

        void decrement() {
            Logger::log(std::cout, "> decrement");
            for (long n = 0; n < NumIterations; ++n) {
                --m_counter;
            }
            Logger::log(std::cout, "< decrement");
        }

        void incrementAtomic() {
            Logger::log(std::cout, "> incrementAtomic");
            for (long n = 0; n < NumIterations; ++n) {
                ++m_counterAtomic;
            }
            Logger::log(std::cout, "< incrementAtomic");
        }

        void decrementAtomic() {
            Logger::log(std::cout, "> decrementAtomic");
            for (long n = 0; n < NumIterations; ++n) {
                --m_counterAtomic;
            }
            Logger::log(std::cout, "< decrementAtomic");
        }

        void incrementMutex() {
            Logger::log(std::cout, "> incrementMutex");
            for (long n = 0; n < NumIterations; ++n) {
                m_mutex.lock();
                ++m_counter;
                m_mutex.unlock();
            }
            Logger::log(std::cout, "< incrementMutex");
        }

        void decrementMutex() {
            Logger::log(std::cout, "> decrementMutex");
            for (long n = 0; n < NumIterations; ++n) {
                m_mutex.lock();
                --m_counter;
                m_mutex.unlock();
            }
            Logger::log(std::cout, "< decrementMutex");
        }

        void incrementMutexRAII() {
            Logger::log(std::cout, "> incrementMutexRAII");
            for (long n = 0; n < NumIterations; ++n) {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_counter;
            }
            Logger::log(std::cout, "< incrementMutexRAII");
        }

        void decrementMutexRAII() {
            Logger::log(std::cout, "> decrementMutexRAII");
            for (long n = 0; n < NumIterations; ++n) {
                std::lock_guard<std::mutex> lock(m_mutex);
                --m_counter;
            }
            Logger::log(std::cout, "< decrementMutexRAII");
        }
    };

    void test_regular() {
        IncDec inc_dec;
        inc_dec.run();
    }

    void test_atomic() {
        IncDec inc_dec;
        inc_dec.runAtomic();
    }

    void test_mutex() {
        IncDec inc_dec;
        inc_dec.runMutex();
    }

    void test_mutexRAII() {
        IncDec inc_dec;
        inc_dec.runMutexRAII();
    }
}

int main()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");

    Logger::startWatch();
    test_regular();
    Logger::stopWatchMilli();

    Logger::startWatch();
    test_atomic();
    Logger::stopWatchMilli();

    Logger::startWatch();
    test_mutex();
    Logger::stopWatchMilli();

    Logger::startWatch();
    test_mutexRAII();
    Logger::stopWatchMilli();

    Logger::log(std::cout, "Done.");
    return 1;
}

// ===========================================================================
// End-of-File
// ===========================================================================
