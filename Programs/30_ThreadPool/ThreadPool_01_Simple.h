// ===========================================================================
// ThreadPool_01_Simple.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus dem Buch "Concurrency in Action - 2nd Edition", Kapitel 9.1
 * 
 * Größter Nachteil: Busy Polling
 *
 */

#include "../25_Threadsafe_Queue/ThreadsafeQueue.h"

#include "JoinThreads.h"

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <future>

namespace ThreadPool_Simple
{
    using namespace Concurrency_ThreadsafeQueue;

    using ThreadPoolFunction = std::function<void()>;

    class ThreadPool
    {
    private:
        std::atomic_bool                     m_done;
        ThreadsafeQueue<ThreadPoolFunction>  m_workQueue;
        std::vector<std::thread>             m_threads;
        JoinThreads                          m_joiner;

    public:
        ThreadPool();
        ~ThreadPool();

        template<typename TFunc>
        void submit(TFunc&& func)
        {
            Logger::log(std::cout, "submitted function ...");
            m_workQueue.push(std::function<void()> (func));
        }

    private:
        void worker_thread();
    };
}

// ===========================================================================
// End-of-File
// ===========================================================================
