#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>

#include "../Logger/Logger.h"

// https://vorbrodt.blog/2019/02/03/blocking-queue/
// Blocking queue - 1. approach

// Folgende Änderungen wurden von mir eingebracht:

// a) Kein placement new
// 

// TO BE DONE: Die GRöße des Puffers könnte ein Template Parameter sein ..........

const int COUNT = 10;


namespace Thread_Safe_Queue_02
{
    template<typename T>
    class BlockingQueue
    {
    private:
        size_t m_size;
        size_t m_pushIndex;
        size_t m_popIndex;
        size_t m_count;
        T* m_data;

        std::counting_semaphore<COUNT> m_openSlots;
        std::counting_semaphore<COUNT> m_fullSlots;

        std::mutex m_cs;

    public:
        BlockingQueue(size_t size)
            : m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
            m_openSlots(size), m_fullSlots(0)
        {
            m_data = new T[size];
        }

        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;
        BlockingQueue& operator = (const BlockingQueue&) = delete;
        BlockingQueue& operator = (BlockingQueue&&) = delete;

        ~BlockingQueue()
        {
            //while (m_count--)
            //{
            //    m_data[m_popIndex].~T();
            //    m_popIndex = ++m_popIndex % m_size;
            //}
            //operator delete(m_data);

            delete[] m_data;
        }

        //void push(const T& item)
        //{
        //    m_openSlots.acquire();
        //    {
        //        std::lock_guard<std::mutex> lock(m_cs);
        //        // new (m_data + m_pushIndex) T(item);
        //        m_data[m_pushIndex] = item;
        //        m_pushIndex = ++m_pushIndex % m_size;
        //        ++m_count;
        //    }
        //    m_fullSlots.release();
        //}

        size_t push(const T& item)
        {
            size_t index{};

            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);
                // new (m_data + m_pushIndex) T(item);
                m_data[m_pushIndex] = item;
                index = m_pushIndex;
                m_pushIndex = ++m_pushIndex % m_size;
                ++m_count;
            }
            m_fullSlots.release();

            return index;
        }

        //void pop(T& item)
        //{
        //    m_fullSlots.acquire();
        //    {
        //        std::lock_guard<std::mutex> lock(m_cs);
        //        item = m_data[m_popIndex];
        //        // m_data[m_popIndex].~T();
        //        m_data[m_popIndex] = T{};
        //        m_popIndex = ++m_popIndex % m_size;
        //        --m_count;
        //    }
        //    m_openSlots.release();
        //}

        size_t pop(T& item)
        {
            size_t index{};

            m_fullSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);
                item = m_data[m_popIndex];
                // m_data[m_popIndex].~T();
                m_data[m_popIndex] = T{};
                index = m_popIndex;
                m_popIndex = ++m_popIndex % m_size;
                --m_count;
            }
            m_openSlots.release();

            return index;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_cs);
            return m_count == 0;
        }
    };
}

void test_thread_safe_queue_02_a()
{
    using namespace Thread_Safe_Queue_02;

    BlockingQueue<int> q(5);
    
    // std::mutex cout_lock;

    std::thread producer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            q.push(i);

            Logger::log(std::cout, "Push: ", i);
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

            Logger::log(std::cout, "Pop:  ", i);
        }
        }
    );

    producer.join();
    consumer.join();
}

void test_thread_safe_queue_02_b()
{
    using namespace Thread_Safe_Queue_02;

    BlockingQueue<std::string> q(5);
    std::mutex cout_lock;

    std::thread producer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            std::string s{ std::string{"String: "} + std::to_string(i)};

            q.push(s);

            Logger::log(std::cout, "Push: ", s);
        }
        }
    );

    std::thread consumer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            using namespace std::chrono_literals;

            std::this_thread::sleep_for(1s);

            std::string s;

            q.pop(s);

            Logger::log(std::cout, "Pop:  ", s);
        }
        }
    );

    producer.join();
    consumer.join();
}

void test_thread_safe_queue_02_c()
{
    using namespace Thread_Safe_Queue_02;

    constexpr std::chrono::milliseconds SleepTimeConsumer{ 30 };
    constexpr std::chrono::milliseconds SleepTimeProducer{ 3000 };

    BlockingQueue<int> q(10);

    std::mutex cout_lock;

    std::thread producer([&]() {

        int nextNumber{};

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeProducer)
            );

            nextNumber++;

            size_t index = q.push(nextNumber);

            Logger::log(std::cout, "pushed ", nextNumber, " at index ", index);
        }
    });

    std::thread consumer1([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeConsumer)
            );

            int number;
            
            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
    });

    std::thread consumer2([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeConsumer)
            );

            int number;

            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
        });

    std::thread consumer3([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeConsumer)
            );

            int number;

            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
        });

    producer.join();
    
    consumer1.join();
    consumer2.join();
    consumer3.join();
}




void test_thread_safe_queue_pelo_version()
{
    Logger::log(std::cout, "BlockingQueue: Modified PeLo Version");

    // test_thread_safe_queue_02_a();
    // test_thread_safe_queue_02_b();
    test_thread_safe_queue_02_c();
    Logger::log(std::cout, "Done.");
}
