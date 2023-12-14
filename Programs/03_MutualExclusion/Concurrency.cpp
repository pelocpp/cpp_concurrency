// ===========================================================================
// Demonstrating concurrent access on a stack object
// ===========================================================================

#include <iostream>
#include <thread> 
#include <atomic>
#include <stack>

#include "../Logger/Logger.h"

namespace SimpleStackForTesting {

    /*
    * simple (non thread-safe) stack for testing concurrent thread access
    */

    template<typename T, int Max = 5>
    class MyStack {
    private:
        int m_index;    // stack pointer 
        T m_data[Max];  // data

    public:
        MyStack();
        virtual ~MyStack();

        // public interface
        bool push(const T&);
        T pop();
        int size() const noexcept;
        bool isEmpty() const noexcept;
        bool isFull() const noexcept;
    };

    template<typename T, int Max>
    MyStack<T, Max>::MyStack() : m_index(0) {
        std::cout << "c'tor Stack()" << std::endl;
        std::fill(m_data, m_data + Max, T{});
    }

    template<typename T, int Max>
    MyStack<T, Max>::~MyStack() {
        std::cout << "d'tor Stack()" << std::endl;
    }

    template<typename T, int Max>
    bool MyStack<T, Max>::push(const T& elem) {

        if (m_index >= Max) {
            throw std::out_of_range("stack is full");
        }

        m_data[m_index] = elem;
        ++m_index;
        return true;
    }

    template<typename T, int Max>
    T MyStack<T, Max>::pop() {

        // is stack empty
        if (m_index == 0) {
            throw std::out_of_range("stack is empty");
        }

        --m_index;
        return std::move(m_data[m_index]);
    }

    template<typename T, int Max>
    int MyStack<T, Max>::size() const noexcept {
        return Max;
    }

    template<typename T, int Max>
    bool MyStack<T, Max>::isEmpty() const noexcept {
        return m_index == 0;
    }

    template<typename T, int Max>
    bool MyStack<T, Max>::isFull() const noexcept {
        return m_index == Max;
    }
}

// ===========================================================================

namespace Concurrency {

    using namespace SimpleStackForTesting;

    constexpr long NumIterations{ 100000000 };

    class ConcurrencyTest {

    private:
        MyStack<int> m_stack;

        // propagating exception from std::thread invocation
        std::exception_ptr m_ep = nullptr;

    public:
        // c'tors
        ConcurrencyTest() = default;

    private:
        // private helper methods
        void produce() {
            Logger::log(std::cout, "> produce");
            try
            {
                for (long n = 0; n < NumIterations; ++n) {
                    if (!m_stack.isFull()) {
                        m_stack.push(n);
                    }
                }
            }
            catch (...)
            {
                Logger::log(std::cout, "> EXCEPTION (Produce)");
                m_ep = std::current_exception();
            }
            Logger::log(std::cout, "< produce");
        }

        void consume() {
            Logger::log(std::cout, "> consume");
            try
            {
                for (long n = 0; n < NumIterations; ++n) {
                    if (!m_stack.isEmpty()) {
                        m_stack.pop();
                    }
                }
            }
            catch (...)
            {
                Logger::log(std::cout, "> EXCEPTION (Consume)");
                m_ep = std::current_exception();
            }
            Logger::log(std::cout, "< consume");
        }

    public:
        void run() {
            Logger::log(std::cout, "initial stack size: ", m_stack.size());

            // spawning two threads - calling produce() rsp. consume()
            std::thread t2(&ConcurrencyTest::produce, this);
            std::thread t1(&ConcurrencyTest::consume, this);
        //  std::thread t3(&ConcurrencyTest::consume, this);

            // wait for end of both threads
            t1.join();  // pauses until t1 finishes
            t2.join();  // pauses until t2 finishes
         // t3.join();  // pauses until t2 finishes

            if (m_ep != nullptr) {
                try {
                    std::rethrow_exception(m_ep);
                }
                catch (const std::exception & ex)
                {
                    std::cerr << "Thread exited with exception: " << ex.what() << "\n";
                }
            }

            Logger::log(std::cout, "final stack size: ", m_stack.size());
        }
    };

    void test() {
        ConcurrencyTest concurrency;
        concurrency.run();
    }
}

int test_non_concurrent_stack()
{
    using namespace Concurrency;
    Logger::log(std::cout, "Begin");
    Logger::startWatch();
    test();
    Logger::stopWatchMilli(std::cout);
    Logger::log(std::cout, "Done.");
    return 1;
}

// ===========================================================================
// End-of-File
// ===========================================================================

