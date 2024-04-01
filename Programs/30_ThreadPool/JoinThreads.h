// ===========================================================================
// JoinThreads.h // Thread Pool
// ===========================================================================

#pragma once

#include "../Logger/Logger.h"

#include <thread>
#include <vector>

class JoinThreads
{
    std::vector<std::thread>& m_threads;

public:
    explicit JoinThreads(std::vector<std::thread>& threads)
        : m_threads{ threads }
    {}

    ~JoinThreads()
    {
        Logger::log (std::cout, "> ~JoinThreads ...");

        for (size_t i{}; i < m_threads.size(); ++i)
        {
            if (m_threads[i].joinable())
                m_threads[i].join();
        }

        Logger::log(std::cout, "< ~JoinThreads ...");
    }
};

// ===========================================================================
// End-of-File
// ===========================================================================
