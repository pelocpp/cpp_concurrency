// ===========================================================================
// ThreadsafeQueue.h
// ===========================================================================

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

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
            std::lock_guard<std::mutex> guard{ other.m_mutex };
            m_data = other.m_data;
        }

        ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
        {
            std::lock_guard<std::mutex> guard{ other.m_mutex };
            m_data = std::move(other.m_data);
        }

        ThreadsafeQueue& operator= (const ThreadsafeQueue& other)
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> guard{ m_mutex, other.m_mutex };
            m_data = other.m_data;
            return *this;
        }

        ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept
        {
            if (&other == this)
                return *this;

            std::scoped_lock<std::mutex> guard{ m_mutex, other.m_mutex };
            m_data = std::move (other.m_data);
            return *this;
        }

        void push(const T& value)
        {
            std::unique_lock<std::mutex> guard{ m_mutex };
            m_data.push(value);
            guard.unlock();
            m_condition.notify_one();
        }

        void push(T&& value)
        {
            std::unique_lock<std::mutex> guard{ m_mutex };
            m_data.push(std::move(value));
            guard.unlock();
            m_condition.notify_one();
        }

        bool tryPop(T& value)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            if (m_data.empty()) {
                return false;
            }
            else {
                value = std::move(m_data.front());
                m_data.pop();
                return true;
            }
        }

        std::optional<T> tryPop()
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            if (m_data.empty()) {
                return std::optional<T>(std::nullopt);
            }
            else {
                std::optional<T> result{ std::move(m_data.front()) };
                m_data.pop();
                return result;
            }
        }

        void waitAndPop(T& value)
        {
            std::unique_lock<std::mutex> guard{ m_mutex };
            m_condition.wait(guard, [this]() {
                return !m_data.empty();
                }
            );

            value = std::move(m_data.front());
            m_data.pop();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.empty();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.size();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
