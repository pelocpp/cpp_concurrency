// ===========================================================================
// ThreadsafeStack.h
// ===========================================================================

// Adopted from Project "20_Threadsafe_Stack"

#pragma once

#include "StrategizedLock.h"

#include <exception>
#include <stack>
#include <mutex>
#include <optional>

using namespace Concurrency_StrategizedLock;

namespace Concurrency_ThreadsafeStack
{
    template<typename T>
    class ThreadsafeStack
    {
    private:
        std::stack<T> m_data;
        const ILock& m_lock;

    public:
        // c'tors
        ThreadsafeStack(const ILock& lock) : m_lock{lock} {}

        // prohibit copy constructor, assignment operator and move assignment
        ThreadsafeStack(const ThreadsafeStack&) = delete;
        ThreadsafeStack& operator = (const ThreadsafeStack&) = delete;
        ThreadsafeStack& operator = (ThreadsafeStack&&) noexcept = delete;

        // move constructor may be useful
        ThreadsafeStack(const ThreadsafeStack&& other) noexcept
        {
            StrategizedLocking guard{ m_lock };
            m_data = other.m_data;
        }

        // public interface
        void push(const T& value)
        {
            StrategizedLocking guard{ m_lock };
            m_data.push(value);
        }

        void pop(T& value)
        {
            StrategizedLocking guard{ m_lock };

            // in case of testing recursive lock 
            //if (empty()) {
            //    std::cout << "Emtpy Stack !";
            //}

            if (m_data.empty()) {
                throw std::out_of_range{ "Stack is empty!" };
            }
            value = m_data.top();
            m_data.pop();
        }

        T tryPop()
        {
            StrategizedLocking guard{ m_lock };
            if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
            T value = m_data.top();
            m_data.pop();
            return value;
        }

        size_t size() const
        {
            StrategizedLocking guard{ m_lock };
            return m_data.size();
        }

        bool empty() const
        {
            StrategizedLocking guard{ m_lock };
            return m_data.empty();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

