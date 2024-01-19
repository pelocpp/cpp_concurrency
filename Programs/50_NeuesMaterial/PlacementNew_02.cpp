// placement new

#include <new>
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <memory>

#include <iostream>
#include <mutex>
#include <thread>

#include <semaphore>


// std::chrono::seconds{ 1 }    Neue Init Syntax !!!!!!!!!

namespace PlacementNew02 {

    template<typename T>
    class blocking_queue
    {
    public:
        blocking_queue(unsigned int size)
            : m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
            m_data((T*)operator new(size * sizeof(T))),
            m_openSlots{ size }, m_fullSlots{ 0 } {}

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

    private:
        unsigned int m_size;
        unsigned int m_pushIndex;
        unsigned int m_popIndex;
        unsigned int m_count;
        T* m_data;

        std::counting_semaphore<5> m_openSlots;
        std::counting_semaphore<5> m_fullSlots;
        
        std::mutex m_cs;
    };


    // ===========================================================

    const int COUNT = 10;

    void test_placement_new_queue_01()
    {
        using namespace PlacementNew02;

        blocking_queue<int> q(5);   // der 5-er muss anders gemacht werden 
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
            });

        std::thread consumer([&]() {
            for (int i = 1; i <= COUNT; ++i)
            {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                int v;
                q.pop(v);
                {
                    std::scoped_lock<std::mutex> lock(cout_lock);
                    std::cout << "pop  v = " << v << std::endl;
                }
            }
            });

        producer.join();
        consumer.join();
    }
}


void test_placement_new_queue ()
{
    using namespace PlacementNew02;

    test_placement_new_queue_01();


}