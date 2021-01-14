// ===========================================================================
// STL and Parallel Algorithms
// ===========================================================================

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <execution>

namespace STL_Parallel_Algorithms
{
    const size_t testSize = 1'000'000;

    const int iterationCount = 4;

    void printResults (
        std::string tag, 
        std::chrono::high_resolution_clock::time_point startTime,
        std::chrono::high_resolution_clock::time_point endTime) 
    {
        std::cout 
            << std::setw(10)
            << std::left
            << tag
            << std::fixed
            << std::setprecision(6)
            << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count()
            << " msecs."
            << std::endl;
    }

    template <typename T>
    void fillTestVector(std::vector<T>& numbers)
    {
        std::random_device device;
        for (auto& number : numbers) {
            number = static_cast<T>(device());
        }
    }

    template <typename T>
    void testSeq(const std::vector<T>& numbers)
    {
        for (int i = 0; i < iterationCount; ++i)
        {
            std::vector<T> copyToSort{ numbers };
            const auto startTime = std::chrono::high_resolution_clock::now();
            std::sort(std::begin(copyToSort), std::end(copyToSort));
            const auto endTime = std::chrono::high_resolution_clock::now();
            printResults("Serial", startTime, endTime);
        }
    }

    template <typename T>
    void testPar(const std::vector<T>& numbers)
    {
        for (int i = 0; i < iterationCount; ++i)
        {
            std::vector<T> copyToSort{ numbers };
            const auto startTime = std::chrono::high_resolution_clock::now();
            // same sort call as above, but with 'par_unseq' or 'par':
            std::sort(std::execution::par, std::begin(copyToSort), std::end(copyToSort));
            const auto endTime = std::chrono::high_resolution_clock::now();
            printResults("Parallel", startTime, endTime);
        }
    }
}

void test_STL_Parallel_Algorithms()
{
    using namespace STL_Parallel_Algorithms;

    std::cout 
        << "Testing with " << testSize << " doubles ..." 
        << std::endl << std::endl;

    std::vector<double> numbers(testSize);
    fillTestVector(numbers);

    testSeq(numbers);
    std::cout << std::endl;
    testPar(numbers);
}

// ===========================================================================
// End-of-File
// ===========================================================================
