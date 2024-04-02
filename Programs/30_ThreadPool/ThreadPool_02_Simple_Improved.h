// ===========================================================================
// ThreadPool_02_Simple_Improved.h // Thread Pool
// ===========================================================================

#pragma once

/*
 * Aus Buch "Concurrency in Action - 2nd Edition", Kapitel 9.1
 * 
 * Größter Nachteil: Busy Polling
 *
 */

#include "../25_Threadsafe_Queue/ThreadsafeQueue.h"
using namespace Concurrency_ThreadsafeQueue;

#include "FunctionWrapper.h"
#include "JoinThreads.h"

#include <iostream>
#include <functional>
#include <thread>
#include <future>
#include <vector>
#include <deque>

namespace ThreadPool_Simple_Improved {

    class ThreadPool
    {
    private:
        std::atomic_bool                        m_done;

     //   ThreadsafeQueue<std::function<void()>>  m_workQueue;
        ThreadsafeQueue<FunctionWrapper>  m_workQueue;

        std::vector<std::thread>                m_threads;
        JoinThreads                             m_joiner;

    public:
        ThreadPool();
        ~ThreadPool();

        // TODO: Da könnte man auch eine Referenz übergeben ! Oder gleich eine Universal Referenz !!!
        //template<typename FunctionType>
        //void submit(FunctionType f)
        //{
        //    Logger::log(std::cout, "submitted function ...");
        //    m_workQueue.push(std::function<void()>(f));
        //}

        template<typename FunctionType>
        std::future<typename std::invoke_result<FunctionType>::type>
        submit(FunctionType f)
        {
            typedef typename std::invoke_result<FunctionType>::type result_type;

            std::packaged_task<result_type()> task(std::move(f));

            std::future<result_type> res(task.get_future());

            // Diese Zeile übersetzt nicht:
            m_workQueue.push(std::move(task));

            return res;
        }

    private:
        void worker_thread();
    };
}




// ===========================================================================
// End-of-File
// ===========================================================================
