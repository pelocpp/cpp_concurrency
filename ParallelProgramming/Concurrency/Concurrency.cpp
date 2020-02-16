// ===========================================================================
// Demonstrating concurrent access on a stack object
// ===========================================================================

#include <iostream>
#include <thread> 
#include <atomic>
#include <stack>

#include "../Logger/Logger.h"

namespace Concurrency {

    constexpr long NumIterations = 10000000;

    class Concurrency {
    public:
        // c'tors
        Concurrency()  {}

        void run() {
            Logger::log(std::cout, "initial stack size: ", m_stack.size());

            // spawning two threads - calling produce() rsp. consume()
            std::thread t2(&Concurrency::produce, this);
            std::thread t1(&Concurrency::consume, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes

            Logger::log(std::cout, "final stack size: ", m_stack.size());
        }

    private:
        std::stack<int> m_stack;

    private:
        // private helper methods
        void produce() {
            Logger::log(std::cout, "> produce");
            for (long n = 0; n < NumIterations; ++n) {
                m_stack.push(n);
            }
            Logger::log(std::cout, "< produce");
        }

        void consume() {
            Logger::log(std::cout, "> consume");
            for (long n = 0; n < NumIterations; ++n) {

                if (!m_stack.empty()) {
                    m_stack.pop();
                }
            }
            Logger::log(std::cout, "< consume");
        }
    };

    void test() {
        Concurrency concurrency;
        concurrency.run();
    }
}

int main_concurrency()
{
    using namespace Concurrency;
    Logger::log(std::cout, "Begin");
    Logger::startWatch();
    test();
    Logger::stopWatchMilli();
    Logger::log(std::cout, "Done.");
    return 1;
}

// ===========================================================================
// End-of-File
// ===========================================================================
