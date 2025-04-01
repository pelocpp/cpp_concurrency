// ===========================================================================
// Prime.cpp
// ===========================================================================

#include <cmath>

//extern const size_t Start = 1'000'000'000'000'000'001;
//extern const size_t End = Start + 100;

// 24 prime numbers
//extern const size_t Start = 1;
//extern const size_t End = Start + 100;

//// 4 prime numbers
//extern const size_t Start = 1000000000001;
//extern const size_t End = Start + 100;

// 4 prime numbers
extern const size_t Start = 1'000'000'000'000'000'001;
extern const size_t End = Start + 100;

// 3614 prime numbers
//extern const size_t Start = 1000000000001;
//extern const size_t End = Start + 100000;



namespace Globals
{
    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm

    // constexpr size_t NumThreads = 8;

    // constexpr size_t UpperLimit { 100 };             // Found:  25 prime numbers
    // constexpr size_t UpperLimit { 1000 };            // Found:  168 prime numbers
    // constexpr size_t UpperLimit { 100'000 };         // Found:  9.592 prime numbers
    // constexpr size_t UpperLimit { 1'000'000 };       // Found:  78.498 prime numbers
    // constexpr size_t UpperLimit { 10'000'000 };      // Found:  664.579 prime numbers
   //  constexpr size_t UpperLimit{ 100'000'000 };        // Found:  5.761.455 prime numbers
}





bool isPrime(size_t number)
{
    if (number == 0 || number == 1) {
        return false;
    }

    if (number % 2 == 0) {
        return false;
    }

    // check odd divisors from 3 to the square root of the number
    size_t end{ static_cast<size_t>(std::ceil(std::sqrt(number))) };
    for (size_t i{ 3 }; i <= end; i += 2) {

        if (number % i == 0) {
            return false; // number not prime
        }
    }

    return true; // found prime number
}

// ===========================================================================
// End-of-File
// ===========================================================================
