#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>

//#include <iomanip>
//#include <deque>
//#include <future>
//#include <thread>
//#include <memory>
//#include <functional>
//#include <queue>
//#include <type_traits>
//#include <vector>
//#include <numeric>

//#include <iostream>
//#include <iomanip>
//#include <sstream>

//#include <algorithm>
//#include <functional>
//#include <vector>
//#include <iterator>
//#include <type_traits>
//

//https://vorbrodt.blog/2019/02/09/template-concepts-sort-of/
//
//https://vorbrodt.blog/2019/02/03/better-blocking-queue/

// https://vorbrodt.blog/2019/02/03/blocking-queue/
// Blocking queue - 1. approach


// Hmmmmmmmmmmmmmmmmmmmmmm
// Mit placement new 

// https://stackoverflow.com/questions/58900136/custom-allocator-including-placement-new-case

const int COUNT = 10;


namespace Thread_Safe_Queue_01
{
    template<typename T>
    class blocking_queue
    {
    private:
        unsigned int m_size;
        unsigned int m_pushIndex;
        unsigned int m_popIndex;
        unsigned int m_count;
        T* m_data;

        std::counting_semaphore<COUNT> m_openSlots;
        std::counting_semaphore<COUNT> m_fullSlots;

        std::mutex m_cs;

    public:
        blocking_queue(unsigned int size)
            : m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
            m_data((T*)operator new(size * sizeof(T))),
            m_openSlots(size), m_fullSlots(0) {}

        blocking_queue(const blocking_queue&) = delete;
        blocking_queue(blocking_queue&&) = delete;
        blocking_queue& operator = (const blocking_queue&) = delete;
        blocking_queue& operator = (blocking_queue&&) = delete;

        ~blocking_queue()
        {
            while (m_count--)
            {
                m_data[m_popIndex].~T();
                m_popIndex = ++m_popIndex % m_size;
            }
            operator delete(m_data);
        }

        void push(const T& item)
        {
            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);
                new (m_data + m_pushIndex) T(item);
                m_pushIndex = ++m_pushIndex % m_size;
                ++m_count;
            }
            m_fullSlots.release();
        }

        void pop(T& item)
        {
            m_fullSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);
                item = m_data[m_popIndex];
                m_data[m_popIndex].~T();
                m_popIndex = ++m_popIndex % m_size;
                --m_count;
            }
            m_openSlots.release();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_cs);
            return m_count == 0;
        }
    };
}

void test_thread_safe_blocking_queue_01()
{
    using namespace Thread_Safe_Queue_01;

    blocking_queue<int> q(5);
    std::mutex cout_lock;

    std::thread producer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            q.push(i);
            {
                std::scoped_lock<std::mutex> lock(cout_lock);
                std::cout << "push v = " << i << std::endl;
            }
        }
        }
    );

    std::thread consumer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            using namespace std::chrono_literals;

            std::this_thread::sleep_for(1s);
            int v;
            q.pop(v);
            {
                std::scoped_lock<std::mutex> lock(cout_lock);
                std::cout << "pop  v = " << v << std::endl;
            }
        }
        }
    );

    producer.join();
    consumer.join();
}


void test_thread_safe_blocking_queue()
{
    test_thread_safe_blocking_queue_01();
}
