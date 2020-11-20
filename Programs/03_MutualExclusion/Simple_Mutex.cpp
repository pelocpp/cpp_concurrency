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
        static const long s_NumIterations = 100000;
        static std::mutex s_Mutex;

        int m_id;

    public:
        Counter(int id, int numIterations) : m_id{ id } {}

        void operator()() const
        {
            for (int i = 0; i < s_NumIterations; ++i) 
            {
                {
                    // <== remove comment from next line to demonstrate scattered output
                    // std::scoped_lock<std::mutex> lock(s_Mutex);
                    // std::scoped_lock lock(s_Mutex);  // C++ 17 (automatic type deduction)
                    std::cout << "Counter " << m_id << " has value " << i << "\n";
                }

                // just to force rescheduling the execution of the threads
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    };

    std::mutex Counter::s_Mutex;
}

void test()
{
    using namespace SimpleMutexDemo;

    // using uniform initialization syntax
    std::thread t1{ Counter{ 1, 20 } };

    // using named variable
    Counter counter(2, 20);
    std::thread t2(counter);

    // using temporary
    std::thread t3(Counter(3, 20));

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
