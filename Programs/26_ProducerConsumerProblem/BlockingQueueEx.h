// ===========================================================================
// BlockingQueueEx.h
// ===========================================================================

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <array>
// #include <semaphore>
#include <mutex>

#include "../Logger/Logger.h"

//TO BE DONE: Das ist jetzt ein BlockingStack

// Im nächsten schritt abändern !!!


namespace ProducerConsumerQueue
{
    template<typename T, size_t QueueSize = 10>
    class BlockingQueue
    {
    private:

        //size_t m_pushIndex;
        //size_t m_popIndex;

        //std::counting_semaphore<QueueSize> m_openSlots;
        //std::counting_semaphore<QueueSize> m_fullSlots;
        //std::mutex mutable m_mutex;

        //T* m_data;

        // array - considered as stack
        std::array<int, QueueSize> m_data;  // shared object
        int m_index;                    // shared index
        size_t m_size;

        // take care of concurrent stack access
        std::mutex mutable m_mutex{};

        // monitor concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty{};
        std::condition_variable m_conditionIsFull{};

    public:
        // default c'tor
        BlockingQueue() :
            m_data{ 0 }, m_index{}, m_size{}
        {}

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator= (const BlockingQueue&) = delete;
        BlockingQueue& operator= (BlockingQueue&&) = delete;

        // destructor
        ~BlockingQueue()
        {
            //size_t n{ 0 };
            //while (n != m_size)
            //{
            //    m_data[m_popIndex].~T();

            //    ++m_popIndex;
            //    m_popIndex = m_popIndex % QueueSize;

            //    ++n;
            //}

            //std::free(m_data);
        }

        // public interface
        void push(const T& item)
        {
            //m_openSlots.acquire();
            //{
            //    std::lock_guard<std::mutex> guard{ m_mutex };

            //    new (m_data + m_pushIndex) T{ item };

            //    ++m_pushIndex;
            //    m_pushIndex = m_pushIndex % QueueSize;

            //    ++m_size;

            //    Logger::log(std::cout, "pushed ", item, ", Size: ", m_size);
            //}
            //m_fullSlots.release();

            // RAII
            {
                std::unique_lock raii{ m_mutex };

                // Is stack full?
                // Note: "Lost Wakeup and Spurious Wakeup"
                m_conditionIsFull.wait(
                    raii,
                    [this]() -> bool { return m_index < 9; }
                );

                // guard
                if (m_index < 9) {

                    m_index++;
                    m_data.at(m_index) = item;

                    ++m_size;

                    Logger::log(std::cout, "pushed ", item, " at index ", m_index);
                }
            }

            // wakeup any sleeping consuments
            m_conditionIsEmpty.notify_all();
        }

        void push(T&& item)
        {
            //m_openSlots.acquire();
            //{
            //    std::lock_guard<std::mutex> guard{ m_mutex };

            //    new (m_data + m_pushIndex) T{ std::move(item) };

            //    ++m_pushIndex;
            //    m_pushIndex = m_pushIndex % QueueSize;

            //    ++m_size;

            //    Logger::log(std::cout, "pushed ", item, ", Size: ", m_size);
            //}
            //m_fullSlots.release();

            {
                // RAII
                std::unique_lock raii{ m_mutex };  // Dijkstra Monitor

                // Is stack empty?
                // Note: "Lost Wakeup and Spurious Wakeup"
                m_conditionIsEmpty.wait(
                    raii,
                    [this]() -> bool { return m_index >= 0; }
                );

                // guard
                if (m_index >= 0) {

                    // int number{ m_data.at(m_index) };
                    item = m_data.at(m_index);
                    Logger::log(std::cout, "popped ", item, " at index ", m_index);

                    ++m_size;

                    m_index--;
                }
            }

            // wakeup any sleeping producers
            m_conditionIsFull.notify_all();
        }

        void pop(T& item)
        {
            //m_fullSlots.acquire();
            //{
            //    std::lock_guard<std::mutex> guard{ m_mutex };

            //    item = m_data[m_popIndex];
            //    m_data[m_popIndex].~T();

            //    ++m_popIndex;
            //    m_popIndex = m_popIndex % QueueSize;

            //    --m_size;

            //    Logger::log(std::cout, "popped ", item, ", Size: ", m_size);
            //}
            //m_openSlots.release();

            {
                // RAII
                std::unique_lock raii{ m_mutex };  // Dijkstra Monitor

                // Is stack empty?
                // Note: "Lost Wakeup and Spurious Wakeup"
                m_conditionIsEmpty.wait(
                    raii,
                    [this]() -> bool { return m_index >= 0; }
                );

                // guard
                if (m_index >= 0) {

                    item = m_data.at(m_index);
                    Logger::log(std::cout, "popped ", item, " at index ", m_index);

                    --m_size;

                    m_index--;
                }
            }

            // wakeup any sleeping producers
            m_conditionIsFull.notify_all();
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
// ===========================================================================
// End-of-File
// ===========================================================================
