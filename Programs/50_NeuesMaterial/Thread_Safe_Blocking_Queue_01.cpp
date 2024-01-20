#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <cstdlib>

#include "../Logger/Logger.h"

//https://vorbrodt.blog/2019/02/09/template-concepts-sort-of/
//
//https://vorbrodt.blog/2019/02/03/better-blocking-queue/

// https://vorbrodt.blog/2019/02/03/blocking-queue/
// Blocking queue - 1. approach


// Hmmmmmmmmmmmmmmmmmmmmmm
// Mit placement new 

// https://stackoverflow.com/questions/58900136/custom-allocator-including-placement-new-case

constexpr int QueueSize{ 5 };

constexpr int NumIterations{ 10 };

constexpr std::chrono::seconds SleepTimeConsumer{ 2 };

namespace Thread_Safe_Queue_01
{
    template<typename T>
    class BlockingQueue
    {
    private:
        size_t m_capacity;
        size_t m_size;
        size_t m_pushIndex;
        size_t m_popIndex;

        std::counting_semaphore<QueueSize> m_openSlots;
        std::counting_semaphore<QueueSize> m_fullSlots;
        std::mutex m_mutex;

        T* m_data;

    public:
        // default c'tor
        BlockingQueue(size_t capacity) : 
            m_capacity{ capacity },
            m_size{ 0 },
            m_pushIndex{ 0 },
            m_popIndex{ 0 },
            m_openSlots{ static_cast<std::ptrdiff_t>(capacity) },
            m_fullSlots{ 0 },
            m_data{ static_cast<T*>(std::malloc(sizeof(T) * m_capacity)) }
        {}

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator = (const BlockingQueue&) = delete;
        BlockingQueue& operator = (BlockingQueue&&) = delete;

        // destructor
        ~BlockingQueue()
        {
            while (m_size--)
            {
                m_data[m_popIndex].~T();
                m_popIndex = ++m_popIndex % m_capacity;
            }

            std::free(m_data);
        }

        // public interface
        void push(const T& item)
        {
            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                new (m_data + m_pushIndex) T{ item };

                m_pushIndex = ++m_pushIndex % m_capacity;
                ++m_size;
            }
            m_fullSlots.release();
        }

        void push(T&& item)
        {
            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                new (m_data + m_pushIndex) T{ std::move(item) };

                m_pushIndex = ++m_pushIndex % m_capacity;
                ++m_size;
            }
            m_fullSlots.release();
        }

        void pop(T& item)
        {
            m_fullSlots.acquire();
            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                item = m_data[m_popIndex];
                m_data[m_popIndex].~T();

                m_popIndex = ++m_popIndex % m_capacity;
                --m_size;
            }
            m_openSlots.release();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_size == 0;
        }
    };
}

void test_thread_safe_blocking_queue_01()
{
    using namespace Thread_Safe_Queue_01;

    BlockingQueue<int> queue{ QueueSize };

    std::thread producer([&] () {

        Logger::log(std::cout, "Producer");

        for (int i = 1; i <= NumIterations; ++i)
        {
            queue.push(i);
            Logger::log(std::cout, "Pushing ", i);
        }

        Logger::log(std::cout, "Producer Done.");
    });

    std::thread consumer([&] () {

        Logger::log(std::cout, "Consumer");

        for (int i = 1; i <= NumIterations; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds{ SleepTimeConsumer });
            int value;
            queue.pop(value);
            Logger::log(std::cout, "Popped  ", i);
        }

        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();
}

void test_thread_safe_blocking_queue()
{
    test_thread_safe_blocking_queue_01();
}
