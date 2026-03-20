// ===========================================================================
// PrimeCalculator.h
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"
#include "../Globals/IsPrime.h"

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

        void operator() () const
        {
            std::thread::id tid{ std::this_thread::get_id() };
            Logger::log(std::cout, "TID: ", tid);

            for (size_t i{ m_begin }; i != m_end; ++i) {

                if (PrimeNumbers::IsPrime(i)) {
                    m_stack.push(i);
                }
            }
        }
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
