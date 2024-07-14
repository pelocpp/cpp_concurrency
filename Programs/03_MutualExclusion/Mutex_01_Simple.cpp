// ===========================================================================
// Mutual Exclusion Demonstration // Mutex_01_Simple.cpp
// ===========================================================================

#include <iostream>
#include <thread> 
#include <mutex> 
#include <chrono>

namespace SimpleMutexDemo
{
    class Counter
    {
    private:
        const size_t NumIterations{ 100000 };

        int m_id{};

        static std::mutex s_mutex;

        // Note: remove static
        // a) Line above
        // b) put 'std::mutex Counter::s_mutex{};' into comments
        // c) add 'mutable' to s_mutex declaration

    public:
        Counter(int id) : m_id{ id } {}

        void run() const {

            for (size_t i{}; i != NumIterations; ++i)
            {
                {
                    // <== remove comment from next line to demonstrate scattered output
                    //std::scoped_lock<std::mutex> raii{ s_mutex };
                    //std::scoped_lock raii{ s_mutex };  // C++ 17 (automatic type deduction)

                    std::cout << "Counter " << m_id << ": i=" << i << "\n";
                }

                // just to force rescheduling the execution of the threads
                std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
            }
        }
    };

    std::mutex Counter::s_mutex{};
}

void test()
{
    using namespace SimpleMutexDemo;

    Counter counter1{ 1 };
    Counter counter2{ 2 };
    Counter counter3{ 3 };

    // run 3 threads ...
    std::thread t1{ &Counter::run, &counter1 };
    std::thread t2{ &Counter::run, &counter2 };
    std::thread t3{ &Counter::run, &counter3 };

    // wait for threads to finish
    t1.join();
    t2.join();
    t3.join();
}

void test_simple_mutex()
{
    using namespace SimpleMutexDemo;
    test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
