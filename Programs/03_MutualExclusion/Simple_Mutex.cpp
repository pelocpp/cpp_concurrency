// ===========================================================================
// Mutual Exclusion
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

        static std::mutex s_mutex;

        int m_id;

    public:
        Counter(int id) : m_id{ id } {}

        void operator()() const
        {
            for (size_t i{}; i != NumIterations; ++i)
            {
                {
                    // <== remove comment from next line to demonstrate scattered output
                    //std::scoped_lock<std::mutex> lock{ s_mutex };
                    //std::scoped_lock lock{ s_mutex };  // C++ 17 (automatic type deduction)
                    std::cout << "Counter " << m_id << " has value " << i << "\n";
                }

                // just to force rescheduling the execution of the threads
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    };

    std::mutex Counter::s_mutex{};
}

void test()
{
    using namespace SimpleMutexDemo;

    // run 3 threads ...
    std::thread t1{ Counter{ 1 } };
    std::thread t2{ Counter{ 2 } };
    std::thread t3{ Counter{ 3 } };

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
