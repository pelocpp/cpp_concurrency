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

// constexpr int QueueSize{ 5 };

constexpr int NumIterations{ 10 };

constexpr std::chrono::milliseconds SleepTimeConsumer{ 20 };
constexpr std::chrono::milliseconds SleepTimeProducer{ 10 };

namespace Thread_Safe_Queue_01
{
    template<typename T, size_t QueueSize = 10>
    class BlockingQueue
    {
    private:
        size_t m_size;
        size_t m_pushIndex;
        size_t m_popIndex;

        std::counting_semaphore<QueueSize> m_openSlots;
        std::counting_semaphore<QueueSize> m_fullSlots;
        std::mutex mutable m_mutex;

        T* m_data;

    public:
        // default c'tor
        BlockingQueue() : 
        // BlockingQueue(size_t capacity) : 
           // m_capacity{ capacity },
            m_size{ 0 },
            m_pushIndex{ 0 },
            m_popIndex{ 0 },
            m_openSlots{ static_cast<std::ptrdiff_t>(QueueSize) },
            m_fullSlots{ 0 },
            m_data{ static_cast<T*>(std::malloc(sizeof(T) * QueueSize)) }
        {}

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator= (const BlockingQueue&) = delete;
        BlockingQueue& operator= (BlockingQueue&&) = delete;

        // destructor
        ~BlockingQueue()
        {
            size_t n{ 0 };
            while (n != m_size)
            {
                m_data[m_popIndex].~T();

                ++m_popIndex;
                m_popIndex = m_popIndex % QueueSize;

                ++n;
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

                ++m_pushIndex;
                m_pushIndex = m_pushIndex % QueueSize;

                ++m_size;

                Logger::log(std::cout, "pushed ", item, ", Size: ", m_size);
            }
            m_fullSlots.release();
        }

        void push(T&& item)
        {
            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> guard{ m_mutex };

                new (m_data + m_pushIndex) T{ std::move(item) };

                ++m_pushIndex;
                m_pushIndex = m_pushIndex % QueueSize;

                ++m_size;

                Logger::log(std::cout, "pushed ", item, ", Size: ", m_size);
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

                ++m_popIndex;
                m_popIndex = m_popIndex % QueueSize;

                --m_size;

                Logger::log(std::cout, "popped ", item, ", Size: ", m_size);
            }
            m_openSlots.release();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_size == 0;
        }


        size_t size() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_size;
        }
    };
}

static void test_thread_safe_blocking_queue_01()
{
    using namespace Thread_Safe_Queue_01;

    constexpr int QueueSize{ 5 };

    BlockingQueue<int, QueueSize> queue{};

    queue.push(1);
    queue.push(2);
    queue.push(3);

    std::cout << "Size: " << queue.size() << std::endl;
}


static void test_thread_safe_blocking_queue_02()
{
    using namespace Thread_Safe_Queue_01;

    constexpr int QueueSize{ 5 };

    BlockingQueue<int, QueueSize> queue{};

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
            std::this_thread::sleep_for(std::chrono::milliseconds{ SleepTimeConsumer });
            int value;
            queue.pop(value);
            Logger::log(std::cout, "Popped  ", i);
        }

        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();
}

class Consumer
{
private:
    Thread_Safe_Queue_01::BlockingQueue<int>& m_queue;

public:
    // c'tor
    Consumer(Thread_Safe_Queue_01::BlockingQueue<int>& queue) 
        : m_queue{ queue }
    {}

    void consume() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeConsumer)
            );

            int value;
            m_queue.pop(value);
        }
    }
};

class Producer
{
private:
    Thread_Safe_Queue_01::BlockingQueue<int>& m_queue;

public:
    // c'tor
    Producer(Thread_Safe_Queue_01::BlockingQueue<int>& queue)
        : m_queue{ queue }
    {}

    void produce() 
    {
        int nextNumber{};

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(SleepTimeProducer)
            );

            nextNumber++;

            m_queue.push(nextNumber);
        }
    }
};

static void test_thread_safe_blocking_queue_03()
{

    constexpr int QueueSize{ 10 };

    Thread_Safe_Queue_01::BlockingQueue<int, QueueSize> queue{ };

    Consumer c{ queue };
    Producer p{ queue };

    std::thread producer([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread consumer([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    producer.join();
    consumer.join();

    Logger::log(std::cout, "Done.");
}

static void test_thread_safe_blocking_queue_04()
{

    constexpr int QueueSize{ 10 };

    Thread_Safe_Queue_01::BlockingQueue<int, QueueSize> queue{ };

    Consumer c{ queue };
    Producer p{ queue };

    std::thread producer1([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
    });

    std::thread producer2([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
        });

    std::thread producer3([&]() {

        Logger::log(std::cout, "Producer");
        p.produce();
        Logger::log(std::cout, "Producer Done.");
        });


    std::thread consumer1([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
    });

    std::thread consumer2([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
        });

    std::thread consumer3([&]() {

        Logger::log(std::cout, "Consumer");
        c.consume();
        Logger::log(std::cout, "Consumer Done.");
        });

    producer1.join();
    producer2.join();
    producer3.join();

    consumer1.join();
    consumer2.join();
    consumer3.join();

    Logger::log(std::cout, "Done.");
}


void test_thread_safe_blocking_queue()
{
    //test_thread_safe_blocking_queue_01();
    //test_thread_safe_blocking_queue_02();
   // test_thread_safe_blocking_queue_03();
    test_thread_safe_blocking_queue_04();
}
