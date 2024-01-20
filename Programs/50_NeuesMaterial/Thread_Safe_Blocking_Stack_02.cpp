#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>

#include "../Logger/Logger.h"

// VERSCUCH: Das Pendant von Vorbrodt auf einen Stack umarbeiten.

// Original:  Thread_Safe_Queue_02.cpp

// TO BE DONE: push und pop geben den Index zu Textzwecken zurück ...
// das sollte entfernt werden ......................

const int COUNT = 10;

namespace Thread_Safe_Queue_02
{
    template<typename T>
    class BlockingStack
    {
    private:
        size_t m_size;
        size_t m_index;
        T* m_data;

        std::counting_semaphore<COUNT> m_openSlots;
        std::counting_semaphore<COUNT> m_fullSlots;

        std::mutex m_cs;

    public:
        BlockingStack(size_t size)
            : m_size(size), m_index(0), 
            m_openSlots(size), m_fullSlots(0)
        {
            m_data = new T[size];
        }

        BlockingStack(const BlockingStack&) = delete;
        BlockingStack(BlockingStack&&) = delete;
        BlockingStack& operator = (const BlockingStack&) = delete;
        BlockingStack& operator = (BlockingStack&&) = delete;

        ~BlockingStack()
        {
            delete[] m_data;
        }

        size_t push(const T& item)
        {
            size_t index{};

            m_openSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);

                m_data[m_index] = item;
                index = m_index;
                ++m_index;
            }
            m_fullSlots.release();

            return index;
        }

        size_t pop(T& item)
        {
            size_t index{};

            m_fullSlots.acquire();
            {
                std::lock_guard<std::mutex> lock(m_cs);

                --m_index;
                item = m_data[m_index];
               // m_data[m_index].~T();
                m_data[m_index] = T{};
                index = m_index;
            }
            m_openSlots.release();

            return index;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_cs);
            return m_index == 0;
        }
    };
}

void test_thread_safe_stack_02_a()
{
    using namespace Thread_Safe_Queue_02;

    BlockingStack<int> q(5);

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

void test_thread_safe_stack_02_b()
{
    using namespace Thread_Safe_Queue_02;

    BlockingStack<std::string> q(5);

    std::thread producer([&]() {
        for (int i = 1; i <= COUNT; ++i)
        {
            std::string s{ std::string{"String: "} + std::to_string(i) };

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

void test_thread_safe_stack_02_c()
{
    using namespace Thread_Safe_Queue_02;

    constexpr std::chrono::milliseconds SleepTimeConsumer{ 500 };
    constexpr std::chrono::milliseconds SleepTimeProducer{ 150 };

    BlockingStack<int> q(10);

    std::mutex cout_lock;

    std::thread producer([&]() {

        int nextNumber{};

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeProducer }
            );

            nextNumber++;

            size_t index = q.push(nextNumber);

            Logger::log(std::cout, "pushed ", nextNumber, " at index ", index);
        }
        });

    std::thread consumer1([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeConsumer }
            );

            int number;

            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
        });

    std::thread consumer2([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeConsumer }
            );

            int number;

            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
        });

    std::thread consumer3([&]() {

        while (true) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds{ SleepTimeConsumer }
            );

            int number;

            size_t index = q.pop(number);

            Logger::log(std::cout, "popped ", number, " at index ", index);
        }
        });

    producer.join();

    consumer1.join();
    //consumer2.join();
    //consumer3.join();
}

void test_thread_safe_stack_pelo_version_02()
{
    Logger::log(std::cout, "BlockingStack: Modified PeLo Version");

    //test_thread_safe_stack_02_a();
    //test_thread_safe_stack_02_b();
    test_thread_safe_stack_02_c();
    Logger::log(std::cout, "Done.");
}
