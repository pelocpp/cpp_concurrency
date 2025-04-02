// ===========================================================================
// Parallel_Count_If.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <algorithm>
#include <future>
#include <numeric>
#include <vector>
#include <chrono>

// ===========================================================================

template <typename It, typename Pred>
auto par_count_if(It first, It last, Pred pred, size_t chunk_sz) {

    auto n = static_cast<size_t>(std::distance(first, last));
    if (n <= chunk_sz)
        return std::count_if(first, last, pred);

    auto middle = std::next(first, n / 2);

    auto future = std::async(std::launch::async,
        [=, &pred] {
            return par_count_if(first, middle, pred, chunk_sz);
        }
    );

    auto num = par_count_if(middle, last, pred, chunk_sz);

    return num + future.get();
}

template <typename It, typename Pred>
auto par_count_if(It first, It last, Pred pred) {

    const auto size{ static_cast<size_t>(std::distance(first, last)) };
    const auto numCores{ std::thread::hardware_concurrency() };
    const auto chunkSize{ std::max(size / numCores * 4, size_t{ 1000 }) };

    return par_count_if(first, last, pred, chunkSize);
}

// ===========================================================================

static auto setup_test_data(size_t n) {

    std::vector<int> src(n);
    std::iota(src.begin(), src.end(), 1);

    auto isOdd = [](int value) { return (value % 2) == 1; };

    return std::pair{ std::move(src), isOdd };
}

static void test_count_if_seq(size_t size) {

    auto&& [numbers, func] = setup_test_data(size);

    ScopedTimer watch;

    auto count = std::count_if(
        numbers.begin(),
        numbers.end(),
        func
    );

    Logger::log(std::cout, "Found ", count, " numbers.");
}

static void test_count_if_par(size_t size) {

    auto&& [numbers, func] = setup_test_data(size);

    ScopedTimer watch;

    auto count = par_count_if<std::vector<int>::iterator>(
        numbers.begin(),
        numbers.end(), 
        func
    );
    Logger::log(std::cout, "Found ", count, " numbers.");
}

void test_count_if() {

    size_t const Size = 50'000'000;

    test_count_if_seq(Size);
    test_count_if_par(Size);
}

// ===========================================================================
// End-of-File
// ===========================================================================



#if BENCH

// Das sieht gut aus !!!!!!!!!!!!

#include <algorithm>
#include <future>
#include <numeric>
#include <vector>
#include <chrono>

template <typename It, typename Pred>
auto par_count_if_real(It first, It last, Pred pred, size_t chunk_sz) {

    auto n = static_cast<size_t>(std::distance(first, last));
    if (n <= chunk_sz)
        return std::count_if(first, last, pred);

    auto middle = std::next(first, n / 2);

    auto future = std::async(std::launch::async,
        [=, &pred] {
            return par_count_if_real(first, middle, pred, chunk_sz);
        }
    );

    auto num = par_count_if_real(middle, last, pred, chunk_sz);

    return num + future.get();
}

static auto setup_test_data(size_t n) {

    std::vector<int> src(n);
    std::iota(src.begin(), src.end(), 1);
    auto isOdd = [](int value) -> bool { return (value % 2) == 1; };
    return std::pair{ std::move(src), isOdd };
}

size_t const Size = 1'000'000;

void CustomArguments(benchmark::internal::Benchmark* b) {
    b->MeasureProcessCPUTime()
        ->UseRealTime()
        ->Unit(benchmark::kMillisecond);
}

static void test_count_if_par(benchmark::State& state) {

    auto&& [numbers, func] = setup_test_data(Size);

    auto isOdd = [](int value) -> bool { return (value % 2) == 1; };

    // const size_t MagicFactor = state.range(0);

    const size_t ChunkSize = state.range(0);

    for (auto _ : state) {

        //    auto count = par_count_if<std::vector<int>::iterator>(
        //      numbers.begin(),
        //      numbers.end(), 
        //      isOdd,
        //      MagicFactor
        //    );

        auto count = par_count_if_real<std::vector<int>::iterator>(
            numbers.begin(),
            numbers.end(),
            isOdd,
            ChunkSize
        );


        benchmark::DoNotOptimize(count);
    }
}

//BENCHMARK(test_count_if_par) ->Apply(CustomArguments)
// ->RangeMultiplier(2)        // chunk size goes from
//  ->Range(4, 256);     // 4 to 64

BENCHMARK(test_count_if_par)->Apply(CustomArguments)
->RangeMultiplier(2)        // chunk size goes from
->Range(10'000, 400'000);     //  10.000 to 200'000




#endif