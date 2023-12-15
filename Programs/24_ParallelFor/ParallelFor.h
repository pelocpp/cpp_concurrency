// ===========================================================================
// ParallelFor.h
// ===========================================================================

#pragma once

#include <functional>

namespace Concurrency_Parallel_For
{
    using Callable = std::function<void(size_t start, size_t end)>;

    extern void callableWrapper(Callable callable, size_t start, size_t end);
    extern void parallel_for(size_t from, size_t to, Callable callable, bool useThreads);
}

// ===========================================================================
// End-of-File
// ===========================================================================
