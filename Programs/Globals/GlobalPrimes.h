// ===========================================================================
// GlobalPrimes.h
// ===========================================================================

#pragma once

// https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

#include <cstddef>

namespace PrimeNumberLimits
{
    static constexpr std::size_t LowerLimit{ 1 };

    //static constexpr std::size_t UpperLimit{ 100 };             // Found:  25 prime numbers
    //static constexpr std::size_t UpperLimit{ 1000 };            // Found:  168 prime numbers
    //static constexpr std::size_t UpperLimit{ 100'000 };         // Found:  9.592 prime numbers
    //static constexpr std::size_t UpperLimit{ 1'000'000 };       // Found:  78.498 prime numbers
    static constexpr std::size_t UpperLimit{ 10'000'000 };        // Found:  664.579 prime numbers
    //static constexpr std::size_t UpperLimit{ 100'000'000 };     // Found:  5.761.455 prime numbers

    // 24 prime numbers
    //constexpr std::size_t Start{ 1 };
    //constexpr std::size_t End{ Start + 100 };

    // 4 prime numbers
    //constexpr std::size_t Start { 1000000000001 };
    //constexpr std::size_t End { Start + 100 };

    // 37 prime numbers
    //constexpr std::size_t Start { 1000000000001 };
    //constexpr std::size_t End { Start + 1000 };

    // 3614 prime numbers
    //constexpr std::size_t Start { 1000000000001 };
    //constexpr std::size_t End { Start + 100000 };

    // 23 prime numbers
    //constexpr std::size_t Start{ 1'000'000'000'000'000'001 };
    //constexpr std::size_t End{ Start + 1'000 };

    // 4 prime numbers
    constexpr std::size_t Start { 1'000'000'000'000'000'001 };
    constexpr std::size_t End { Start + 100 };

    // 241 prime numbers
    //static constexpr std::size_t Start { 1'000'000'000'000'000'001 };
    //static constexpr std::size_t End { Start + 10'000 };

    // 114 prime numbers
    //static constexpr std::size_t Start{ 1'000'000'000'000'000'001 };
    //static constexpr std::size_t End{ Start + 5'000 };
}

// ===========================================================================
// End-of-File
// ===========================================================================
