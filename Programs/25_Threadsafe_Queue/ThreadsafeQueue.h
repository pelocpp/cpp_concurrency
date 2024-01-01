// ===========================================================================
// ThreadsafeQueue.h
// ===========================================================================

#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

namespace Concurrency_ThreadsafeQueue
{
    template<typename T>
    class ThreadsafeQueue
    {
    private:
        std::queue<T>            m_data;
        mutable std::mutex       m_mutex;
        std::condition_variable  m_condition;

    public:
        ThreadsafeQueue() {}

        // copy and move constructor may be useful
        ThreadsafeQueue(const ThreadsafeQueue& other)
        {
            std::lock_guard<std::mutex> lock{ other.m_mutex };
            m_data = other.m_data;
        }

        ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
        {
            std::lock_guard<std::mutex> lock{ other.m_mutex };
            m_data = std::move(other.m_data);
        }

        ThreadsafeQueue& operator= (const ThreadsafeQueue& other)
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };
            m_data = other.m_data;
            return *this;
        }

        ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };
            m_data = std::move (other.m_data);
            return *this;
        }

        void push(const T& value)
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_data.push(value);
            lock.unlock();
            m_condition.notify_one();
        }

        void push(T&& value)
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_data.push(std::move(value));
            lock.unlock();
            m_condition.notify_one();
        }

        void waitAndPop(T& value)
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_condition.wait(lock, [this] () {
                return !m_data.empty(); 
                }
            );

            value = m_data.front();
            m_data.pop();
        }

        bool tryPop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            if (m_data.empty()) {
                return false;
            }

            value = m_data.front();
            m_data.pop();
            return true;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            return m_data.empty();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            return m_data.size();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
