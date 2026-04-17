// ===========================================================================
// BlockingQueue.h
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"

#include <condition_variable>  // for std::condition_variable
#include <cstddef>             // for std::size_t
#include <mutex>               // for std::mutex
#include <utility>             // for std::move
#include <queue>               // for std::queue

namespace ProducerConsumerQueue
{
    template<typename T, std::size_t QueueSize = 10>
    class BlockingQueue
    {
    private:
        std::queue<T> m_data;    // queue container used to simulate the bounded buffer

        mutable std::mutex m_mutex;

        // Monitor Concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty;
        std::condition_variable m_conditionIsFull;

    public:
        // default c'tor
        BlockingQueue()
        {
            Logger::log(std::cout, "Using Blocking Queue with Condition Variables");
        }

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator= (const BlockingQueue&) = delete;
        BlockingQueue& operator= (BlockingQueue&&) = delete;

        // public interface
        void push(const T& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // wait until there's space (handles lost/spurious wakeups)
                m_conditionIsFull.wait(
                    guard,
                    [this]() -> bool { return m_data.size() < QueueSize; }
                );

                // push item
                m_data.push(item);

                Logger::log(std::cout, "    Size: ", m_data.size());
            }

            // wakeup any sleeping consumers
            m_conditionIsEmpty.notify_all();
        }

        void push(T&& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // wait until there's space
                m_conditionIsFull.wait(
                    guard,
                    [this]() -> bool { return m_data.size() < QueueSize; }
                );

                // push moved item
                m_data.push(std::move(item));

                Logger::log(std::cout, "    Size: ", m_data.size());
            }

            // wakeup any sleeping consumers
            m_conditionIsEmpty.notify_all();
        }

        void pop(T& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // wait until there's at least one item
                m_conditionIsEmpty.wait(
                    guard,
                    [this]() -> bool { return !m_data.empty(); }
                );

                // retrieve and pop front
                item = std::move(m_data.front());
                m_data.pop();

                Logger::log(std::cout, "    Size: ", m_data.size());
            }

            // wakeup any sleeping producers
            m_conditionIsFull.notify_all();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.empty();
        }

        std::size_t size() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.size();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

namespace ProducerConsumerQueueLegacy
{
    template<typename T, std::size_t QueueSize = 10>
    class BlockingQueue
    {
    private:
        std::array<T, QueueSize> m_data;  // array - considered as a queue

        std::size_t m_pushIndex;
        std::size_t m_popIndex;
        std::size_t m_size;

        std::mutex mutable m_mutex;

        // Monitor Concept (Dijkstra)
        std::condition_variable m_conditionIsEmpty;
        std::condition_variable m_conditionIsFull;

    public:
        // default c'tor
        BlockingQueue() :
            m_data{}, m_pushIndex{}, m_popIndex{}, m_size{}
        {
            Logger::log(std::cout, "Using Blocking Queue with Condition Variables");
        }

        // don't need other constructors or assignment operators
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue(BlockingQueue&&) = delete;

        BlockingQueue& operator= (const BlockingQueue&) = delete;
        BlockingQueue& operator= (BlockingQueue&&) = delete;

        // public interface
        void push(const T& item)
        {
            {
                std::unique_lock<std::mutex> guard{ m_mutex };

                // is stack full? (Note: lost and spurious wakeups)
                m_conditionIsFull.wait(
                    guard,
                    [this]() -> bool { return m_size < QueueSize; }
                );

                // guard
                if (m_size < QueueSize) {

                    m_data.at(m_pushIndex) = item;

                    ++m_pushIndex;
                    m_pushIndex = m_pushIndex % QueueSize;

                    ++m_size;

                    Logger::log(std::cout, "    Size: ", m_size);
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
                    [this]() -> bool { return m_size < QueueSize; }
                );

                // guard
                if (m_size < QueueSize) {

                    m_data.at(m_pushIndex) = std::move(item);

                    ++m_pushIndex;
                    m_pushIndex = m_pushIndex % QueueSize;

                    ++m_size;

                    Logger::log(std::cout, "    Size: ", m_size);
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
                    [this]() -> bool { return m_size != 0; }
                );

                // guard
                if (m_size != 0) {

                    item = m_data[m_popIndex];

                    ++m_popIndex;
                    m_popIndex = m_popIndex % QueueSize;

                    --m_size;

                    Logger::log(std::cout, "    Size: ", m_size);
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

        std::size_t size() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_size;
        }
    };
}
