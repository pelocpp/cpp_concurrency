// ===========================================================================
// ParallelFor02.cpp // Parallel For
// ===========================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <thread>
#include <vector>

#include "../Logger/Logger.h"

#include "ParallelFor02.h"

namespace Concurrency_ParallelFor_Legacy
{
    constexpr bool Verbose{ false };

    void callableWrapper(Callable callable, std::size_t start, std::size_t end) {

        if (Verbose) {
            Logger::log(std::cout, "TID: ", std::this_thread::get_id());
        }

        callable(start, end);
    }

    void parallel_for(
        std::size_t from,
        std::size_t to,
        Callable callable,
        bool useThreads)
    {
        // calculate number of threads to use
        std::size_t numThreadsHint{ std::thread::hardware_concurrency() };
        std::size_t numThreads{ (numThreadsHint == 0) ? 8 : numThreadsHint };
        std::size_t numElements{ to - from + 1 };
        std::size_t batchSize{ numElements / numThreads };
        std::size_t batchRemainder{ numElements % numThreads };

        // allocate vector of uninitialized thread objects
        std::vector<std::thread> threads;
        threads.reserve(numThreads - 1);

        if (useThreads) {

            // prepare multi-threaded execution
            for (std::size_t i{}; i != numThreads - 1; ++i) {

                std::size_t start{ from + i * batchSize };
                
                std::thread t{ callableWrapper, callable, start, start + batchSize };
                threads.push_back(std::move (t));
            }
        }
        else {

            // prepare single-threaded execution (for easy debugging)
            for (std::size_t i{}; i != numThreads - 1; ++i) {

                std::size_t start{ from + i * batchSize };
                
                // callable(start, start + batchSize);
                callableWrapper(callable, start, start + batchSize);
            }
        }

        // take care of last element - calling 'callable' synchronously 
        std::size_t start{ from + (numThreads - 1) * batchSize };
        callable(start, to);

        // wait for the other thread to finish their task
        if (useThreads) {
            std::for_each(
                threads.begin(),
                threads.end(),
                std::mem_fn(&std::thread::join)
            );
        }
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
