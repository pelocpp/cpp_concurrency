// ===========================================================================
// Atomic Operations
// ===========================================================================

#include <iostream>
#include <thread> 
#include <atomic>

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

    private:
        long m_counter = 0;
        std::atomic<long> m_counterAtomic;

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
    };

    void test_regular() {
        IncDec inc_dec;
        inc_dec.run();
    }

    void test_atomic() {
        IncDec inc_dec;
        inc_dec.runAtomic();
    }
}

int main_inc_dec()
{
    using namespace IncrementDecrement;
    Logger::log(std::cout, "Begin");
    Logger::startWatch();
    test_regular();
    Logger::stopWatchMilli();
    test_atomic();
    Logger::stopWatchMilli();
    Logger::log(std::cout, "Done.");
    return 1;
}

// ===========================================================================
// End-of-File
// ===========================================================================
