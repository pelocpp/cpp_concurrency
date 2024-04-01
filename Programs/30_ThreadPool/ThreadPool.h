// ===========================================================================
// ThreadPool.h // Thread Pool
// ===========================================================================

#pragma once

#include "../25_Threadsafe_Queue/ThreadsafeQueue.h"
using namespace Concurrency_ThreadsafeQueue;

#include "JoinThreads.h"

#include <iostream>
#include <functional>
#include <thread>
#include <vector>

class ThreadPool
{
private:
    std::atomic_bool                        m_done;
    ThreadsafeQueue<std::function<void()>>  m_workQueue;
    std::vector<std::thread>                m_threads;
    JoinThreads                             m_joiner;

public:
    ThreadPool();
    ~ThreadPool();

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        Logger::log(std::cout, "submitted function ...");
        m_workQueue.push(std::function<void()>(f));
    }

private:
    void worker_thread();
};

// ===========================================================================
// End-of-File
// ===========================================================================
