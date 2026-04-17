// ===========================================================================
// ParallelFor01.h
// ===========================================================================

#pragma once

#include <algorithm>   // std::for_each
#include <concepts>    // std::integral
#include <execution>   // std::execution::par
#include <ranges>      // std::views::iota
// #include <thread>      //
#include <vector>      // std::vector
#include <numeric>     // std::iota

namespace Concurrency_ParallelFor
{
    template <typename TIndex, typename TFunc>
        requires std::integral<TIndex>
    void parallel_for_stl(TIndex first, TIndex last, TFunc func) {

        std::vector<std::size_t> indices(last - first);
        std::iota(indices.begin(), indices.end(), first);

        std::for_each(
            std::execution::par,
            indices.begin(),
            indices.end(),
            std::move(func)
        );
    }

    template <typename TIndex, typename TFunc>
        requires std::integral<TIndex>
    void parallel_for_ranges(TIndex first, TIndex last, TFunc func) {

        auto range{ std::views::iota(first, last) };

        std::for_each(
            std::execution::par,
            range.begin(),
            range.end(),
            std::move(func)
        );
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
