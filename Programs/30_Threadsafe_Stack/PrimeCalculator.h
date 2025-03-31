// ===========================================================================
// PrimeCalculator.h
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"

#include "ThreadsafeStack.h"

namespace Concurrency_PrimeCalculator
{
    using namespace Concurrency_ThreadsafeStack;

    template <typename T>
    class PrimeCalculator
    {
    private:
        ThreadsafeStack<T>&  m_stack;
        size_t               m_begin;
        size_t               m_end;

    public:
        PrimeCalculator(ThreadsafeStack<T>& stack, size_t begin, size_t end)
            : m_stack{ stack }, m_begin{ begin }, m_end{ end }
        {
            Logger::log(std::cout, "PrimeCalculator: ", m_begin, " => ", m_end);
        }

        void operator() ()
        {
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "TID: ", tid);

            for (size_t i{ m_begin }; i != m_end; ++i) {

                if (isPrime(i)) {
                    m_stack.push(i);
                }
            }
        }

    public:
        static bool isPrime(size_t number)
        {
            if (number <= 2) {
                return number == 2;
            }

            if (number % 2 == 0) {
                return false;
            }

            // check odd divisors from 3 to the square root of the number
            size_t end{ static_cast<size_t>(std::ceil(std::sqrt(number))) };
            for (size_t i{ 3 }; i <= end; i += 2) {

                if (number % i == 0) {
                    return false;  // number not prime
                }
            }

            return true; // found prime number
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
