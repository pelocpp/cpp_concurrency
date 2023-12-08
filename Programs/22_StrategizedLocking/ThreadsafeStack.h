// ===========================================================================
// ThreadsafeStack.h
// ===========================================================================

// Adapted from Project "20_Threadsafe_Stack"

#pragma once

#include <exception>
#include <stack>
#include <mutex>
#include <optional>

#include "StrategizedLock.h"

using namespace Concurrency_StrategizedLock;

namespace Concurrency_ThreadsafeStack
{
    struct empty_stack : std::exception
    {
    private:
        std::string m_what;

    public:
        explicit empty_stack() : m_what{ std::string{ "Stack is empty!" } } {}

        explicit empty_stack(std::string msg) {
            m_what = std::move(msg);
        }

        const char* what() const noexcept override {
            return m_what.c_str();
        }
    };

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
            StrategizedLocking m_guard{ m_lock };
            m_data = other.m_data;
        }

        // public interface
        void push(T new_value)
        {
            StrategizedLocking m_guard{ m_lock };
            m_data.push(new_value);
        }

        void pop(T& value)
        {
            StrategizedLocking m_guard{ m_lock };

            //// in case of testing recursive lock 
            // if (empty()) {
            //     std::cout << "Emtpy Stack !";
            // }

            if (m_data.empty()) throw empty_stack{};
            value = m_data.top();
            m_data.pop();
        }

        T tryPop()
        {
            StrategizedLocking m_guard{ m_lock };
            if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
            T value = m_data.top();
            m_data.pop();
            return value;
        }

        size_t size() const
        {
            StrategizedLocking m_guard{ m_lock };
            return m_data.size();
        }

        bool empty() const
        {
            StrategizedLocking m_guard{ m_lock };
            return m_data.empty();
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================

