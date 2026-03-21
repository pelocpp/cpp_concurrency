// ===========================================================================
// ParallelFor02.h
// ===========================================================================

#pragma once

#include <functional>

namespace Concurrency_ParallelFor_Legacy
{
    using Callable = std::function<void(std::size_t start, std::size_t end)>;

    extern void callableWrapper(Callable callable, std::size_t start, std::size_t end);
    extern void parallel_for(std::size_t from, std::size_t to, Callable callable, bool useThreads);
}

// ===========================================================================
// End-of-File
// ===========================================================================
