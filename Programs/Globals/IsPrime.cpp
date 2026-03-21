// ===========================================================================
// IsPrime.cpp
// ===========================================================================

#include <cmath>
#include <cstddef>

#include "IsPrime.h"

bool PrimeNumbers::IsPrime(std::size_t number)
{
    if (number == 0 || number == 1) {
        return false;
    }

    if (number % 2 == 0) {
        return false;
    }

    // check odd divisors from 3 to the square root of the number
    std::size_t end{ static_cast<std::size_t>(std::ceil(std::sqrt(number))) };
    for (std::size_t i{ 3 }; i <= end; i += 2) {

        if (number % i == 0) {
            return false; // number not prime
        }
    }

    return true; // found prime number
}

// ===========================================================================
// End-of-File
// ===========================================================================
