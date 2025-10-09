// ===========================================================================
// ThreadsafeStack.h
// ===========================================================================

#pragma once

#include <exception>
#include <stack>
#include <mutex>
#include <optional>

namespace Concurrency_ThreadsafeStack
{
    template<typename T>
    class ThreadsafeStack
    {
    private:
        std::stack<T>       m_data;
        mutable std::mutex  m_mutex;

    public:
        // c'tors
        ThreadsafeStack() {}

        // prohibit assignment operator and move assignment
        ThreadsafeStack& operator= (const ThreadsafeStack&) = delete;
        ThreadsafeStack& operator= (ThreadsafeStack&&) noexcept = delete;

        // copy and move constructor may be useful
        ThreadsafeStack(const ThreadsafeStack& other)
        {
            std::lock_guard<std::mutex> guard{ other.m_mutex };
            m_data = other.m_data;
        }
        
        ThreadsafeStack(ThreadsafeStack&& other) noexcept
        {
            std::lock_guard<std::mutex> guard{ other.m_mutex };
            m_data = std::move(other.m_data);
        }

        // public interface
        void push(const T& value)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_data.push(value);
        }

        void push(T&& value)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_data.push(std::move(value));
        }

        template<typename... TArgs>
        void emplace(TArgs&&... args)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            m_data.emplace(std::forward<TArgs>(args) ...);
        }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
            value = std::move(m_data.top());
            m_data.pop();
        }

        bool tryPop(T& value)
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            if (m_data.empty()) {
                return false;
            }
            else {
                value = std::move(m_data.top());
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
                std::optional<T> result{ std::move(m_data.top()) };
                m_data.pop();
                return result;
            }
        }

        std::optional<T> top() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            if (m_data.empty()) {
                return std::optional<T>(std::nullopt);
            }
            else {
                std::optional<T> result(m_data.top());
                return result;
            }
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.size();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard{ m_mutex };
            return m_data.empty();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

