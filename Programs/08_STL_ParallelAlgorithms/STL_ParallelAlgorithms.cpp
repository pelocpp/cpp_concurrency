// ===========================================================================
// STL and Parallel Algorithms
// ===========================================================================

#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <vector>
#include <execution>

namespace STL_Parallel_Algorithms
{
    const size_t testSize = 1'000'000;
    const int iterationCount = 5;

    void print_results_02 (
        std::string tag, 
        const std::vector<double>& sorted,
        std::chrono::high_resolution_clock::time_point startTime,
        std::chrono::high_resolution_clock::time_point endTime) 
    {
        std::cout 
            << tag 
            << 
            ": Lowest: "
            << sorted.front() 
            << ", Highest: "
            << sorted.back() 
            << ", Time: "
            << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count()
            << std::endl;
    }

    void test_01() 
    {
        std::random_device rd;

        // generate some random doubles:
        std::cout << "Testing with " << testSize << " doubles..." << std::endl;
        std::vector<double> doubles(testSize);
        for (auto& d : doubles) {
            d = static_cast<double>(rd());
        }

        // time how long it takes to sort them:
        for (int i = 0; i < iterationCount; ++i)
        {
            std::vector<double> sorted(doubles);
            const auto startTime = std::chrono::high_resolution_clock::now();
            std::sort(sorted.begin(), sorted.end());
            const auto endTime = std::chrono::high_resolution_clock::now();
            print_results_02("Serial", sorted, startTime, endTime);
        }
    }

    void test_02()
    {
        std::random_device rd;

        // generate some random doubles:
        std::cout << "Testing with " << testSize << " doubles..." << std::endl;
        std::vector<double> doubles(testSize);
        for (auto& d : doubles) {
            d = static_cast<double>(rd());
        }

        // time how long it takes to sort them:
        for (int i = 0; i < iterationCount; ++i)
        {
            std::vector<double> sorted(doubles);
            const auto startTime = std::chrono::high_resolution_clock::now();
            // same sort call as above, but with 'par_unseq':
            std::sort(std::execution::par_unseq, sorted.begin(), sorted.end());
            const auto endTime = std::chrono::high_resolution_clock::now();
            print_results_02("Parallel", sorted, startTime, endTime);
        }
    }
}

void test_STL_Parallel_Algorithms()
{
    using namespace STL_Parallel_Algorithms;
    test_01();
    std::cout << std::endl;
    test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
