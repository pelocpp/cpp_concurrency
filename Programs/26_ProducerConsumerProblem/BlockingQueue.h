// ===========================================================================
// BlockingQueueEx.h
// ===========================================================================

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <array>
#include <mutex>

#include "../Logger/Logger.h"

// TO BE DONE: Das ist jetzt ein BlockingStack
// Im nächsten schritt abändern !!!

namespace ProducerConsumerQueue
{
    template<typename T, size_t QueueSize = 10>
    class BlockingQueue
    {
    private:
        // array - considered as stack
        std::array<int, QueueSize> m_data;  // shared object
        int m_index;                        // shared index
        size_t m_size;                      // shared size

        // take care of concurrent stack access
        std::mutex mutable m_mutex{};

        // Monitor Concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty{};
        std::condition_variable m_conditionIsFull{};

    public:
        // default c'tor
        BlockingQueue() :
            m_data{ 0 }, m_index{ -1 }, m_size{}
        {
            Logger::log(std::cout, "Using BlockingQueue with Condition Variables");
        }

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator= (const BlockingQueue&) = delete;
        BlockingQueue& operator= (BlockingQueue&&) = delete;

        // destructor - bleibt das so: Löschen
        ~BlockingQueue()
        {}

        // public interface
        void push(const T& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // is stack full? (Note: lost and spurious wakeups)
                m_conditionIsFull.wait(
                    guard,
                    [this] () -> bool { return m_index < static_cast<int>(QueueSize - 1); }
                );

                // guard
                if (m_index < static_cast<int>(QueueSize - 1)) {

                    m_index++;
                    m_data.at(m_index) = item;

                    ++m_size;

                   // Logger::log(std::cout, "pushed ", item, " at index ", m_index);
                }
            }

            // wakeup any sleeping consuments
            m_conditionIsEmpty.notify_all();
        }

        void push(T&& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // is stack full? (Note: lost and spurious wakeups)
                m_conditionIsFull.wait(
                    guard,
                    [this]() -> bool { return m_index < static_cast<int>(QueueSize - 1); }    // das mit  der 9 ist FALSCH !!!!!!!!!!
                );

                // guard
                if (m_index < static_cast<int>(QueueSize - 1)) {

                    m_index++;
                    m_data.at(m_index) = std::move(item);

                    ++m_size;

                 //   Logger::log(std::cout, "pushed ", item, " at index ", m_index);
                }
            }

            // wakeup any sleeping consuments
            m_conditionIsEmpty.notify_all();
        }

        void pop(T& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // is stack empty? (Note: lost and spurious wakeups)
                m_conditionIsEmpty.wait(
                    guard,
                    [this]() -> bool { return m_index >= 0; }
                );

                // guard
                if (m_index >= 0) {

                    item = m_data.at(m_index);
                    --m_index;

                    --m_size;

                //    Logger::log(std::cout, "popped ", item, " at index ", (m_index + 1));
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
