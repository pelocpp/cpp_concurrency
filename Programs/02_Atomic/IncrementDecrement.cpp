// ===========================================================================
// Atomic Operations (std::atomic<> and std::mutex)
// ===========================================================================

#include <iostream>
#include <thread> 
#include <atomic>
#include <mutex>

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

namespace IncrementDecrement {

    constexpr long NumIterations{ 10000'000 };

    class IncDec {

    private:
        long m_counter{};
        std::atomic<long> m_counterAtomic;
        std::mutex m_mutex;

    public:
        // c'tors
        IncDec() : m_counter{}, m_counterAtomic{} {}

        void run() {
            Logger::log(std::cout, "Counter: ", m_counter);

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1{ &IncDec::incrementSimple, this };
            std::thread t2{ &IncDec::decrementSimple, this };

            // wait for end of both threads
            t1.join();  // blocks until t1 finishes
            t2.join();  // blocks until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counter);
        }

        void runAtomic() {
            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1{ &IncDec::incrementAtomic, this };
            std::thread t2{ &IncDec::decrementAtomic, this };

            // wait for end of both threads
            t1.join();  // blocks until t1 finishes
            t2.join();  // blocks until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counterAtomic.load());
        }

        void runMutex() {
            Logger::log(std::cout, "Counter: ", m_counter);

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1{ &IncDec::incrementMutex, this };
            std::thread t2{ &IncDec::decrementMutex, this };

            // wait for end of both threads
            t1.join();  // blocks until t1 finishes
            t2.join();  // blocks until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counter);
        }

        void runMutexRAII() {
            Logger::log(std::cout, "Counter: ", m_counter);

            // spawning two threads - calling increment() rsp. decrement()
            std::thread t1{ &IncDec::incrementMutexRAII, this };
            std::thread t2{ &IncDec::decrementMutexRAII, this };

            // wait for end of both threads
            t1.join();  // blocks until t1 finishes
            t2.join();  // blocks until t2 finishes

            Logger::log(std::cout, "Counter: ", m_counter);
        }

    private:
        // private helper methods
        void incrementSimple() {
            Logger::log(std::cout, "> incrementSimple");
            for (size_t n{}; n != NumIterations; ++n) {
                ++m_counter;
            }
            Logger::log(std::cout, "< incrementSimple");
        }

        void decrementSimple() {
            Logger::log(std::cout, "> decrementSimple");
            for (size_t n{}; n != NumIterations; ++n) {
                --m_counter;
            }
            Logger::log(std::cout, "< decrementSimple");
        }

        void incrementAtomic() {
            Logger::log(std::cout, "> incrementAtomic");
            for (size_t n{}; n != NumIterations; ++n) {
                ++m_counterAtomic;
            }
            Logger::log(std::cout, "< incrementAtomic");
        }

        void decrementAtomic() {
            Logger::log(std::cout, "> decrementAtomic");
            for (size_t n{}; n != NumIterations; ++n) {
                --m_counterAtomic;
            }
            Logger::log(std::cout, "< decrementAtomic");
        }

        void incrementMutex() {
            Logger::log(std::cout, "> incrementMutex");
            for (size_t n{}; n != NumIterations; ++n) {
                m_mutex.lock();
                ++m_counter;
                m_mutex.unlock();
            }
            Logger::log(std::cout, "< incrementMutex");
        }

        void decrementMutex() {
            Logger::log(std::cout, "> decrementMutex");
            for (size_t n{}; n != NumIterations; ++n) {
                m_mutex.lock();
                --m_counter;
                m_mutex.unlock();
            }
            Logger::log(std::cout, "< decrementMutex");
        }

        void incrementMutexRAII() {
            Logger::log(std::cout, "> incrementMutexRAII");
            for (size_t n{}; n != NumIterations; ++n) {
                std::lock_guard<std::mutex> raii{ m_mutex };
                ++m_counter;
            }
            Logger::log(std::cout, "< incrementMutexRAII");
        }

        void decrementMutexRAII() {
            Logger::log(std::cout, "> decrementMutexRAII");
            for (size_t n{}; n != NumIterations; ++n) {
                std::lock_guard<std::mutex> raii{ m_mutex };
                --m_counter;
            }
            Logger::log(std::cout, "< decrementMutexRAII");
        }
    };
}

void test_01()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");
    {
        ScopedTimer timer{};
        IncDec inc_dec;
        inc_dec.run();
    }
    Logger::log(std::cout, "Done.");
}

void test_02()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");
    {
        ScopedTimer timer{};
        IncDec inc_dec;
        inc_dec.runAtomic();
    }
    Logger::log(std::cout, "Done.");
}

void test_03()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");
    {
        ScopedTimer timer{};
        IncDec inc_dec;
        inc_dec.runMutex();
    }
    Logger::log(std::cout, "Done.");
}

void test_04()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");
    {
        ScopedTimer timer{};
        IncDec inc_dec;
        inc_dec.runMutexRAII();
    }
    Logger::log(std::cout, "Done.");
}

// ===========================================================================
// End-of-File
// ===========================================================================
