// ===========================================================================
// Parallel_Transform.cpp
// ===========================================================================

#include "../Logger/Logger.h"
#include "../Logger/ScopedTimer.h"

#include <algorithm>
#include <future>
#include <numeric>
#include <print>
#include <vector>

// ===========================================================================

extern const size_t Start;
extern const size_t End;

extern bool isPrime(size_t number);

const size_t ChunkSize = 500;

// ===========================================================================

template <typename SrcIt, typename DstIt, typename Func>
static auto parallel_transform(SrcIt first, SrcIt last, DstIt dst, Func func) {

    const auto size{ static_cast<size_t>(std::distance(first, last)) };
    const auto numCores{ std::thread::hardware_concurrency() };
    const auto numTasks{ std::max(static_cast<size_t>(numCores), size_t{ 1 }) };
    const auto chunkSize{ (size + numTasks - 1) / numTasks };   // chunkSize of batchSize // evtl. rename

    auto futures{ std::vector<std::future<void>>{} };

    // process each chunk on a separate task
    for (size_t i{}; i != numTasks; ++i) {

        auto start{ chunkSize * i };

        if (start < size) {

            auto stop{ std::min(chunkSize * (i + 1), size) };

            auto fut{
                std::async(
                    std::launch::async,
                    [=]() {
                        // std::println("Chunk: {} to {}", start, stop);
                     //   Logger::log(std::cout, "Chunk: ", start, " to ", stop);
                        std::transform(first + start, first + stop, dst + start, func);
                    }
                )
            };

            futures.emplace_back(std::move(fut));
        }
    }

    // wait for each task to finish
    for (auto& fut : futures) {
        fut.wait();
    }
}

template <typename SrcIt, typename DstIt, typename Func>
static auto parallel_transform (SrcIt first, SrcIt last, DstIt dst, Func func, size_t chunkSize) {

 //   Logger::log(std::cout, "parallel_transform (divide-conquer)");

    const auto n = static_cast<size_t>(std::distance(first, last));
    if (n <= chunkSize) {
        std::transform(first, last, dst, func);
        return;
    }

    const auto srcMiddle{ std::next(first, n / 2) };

    // push branch of first part to another task
    auto future{ std::async(
        std::launch::async,
        [=, &func] {
            parallel_transform(first, srcMiddle, dst, func, chunkSize);
        })
    };

    // recursively handle the second part
    const auto dstMiddle{ std::next(dst, n / 2) };
    parallel_transform(srcMiddle, last, dstMiddle, func, chunkSize);

    future.wait();
}

// ===========================================================================

static auto setup_simple_test(int n) {
    auto src = std::vector<size_t>(n);
    std::iota(src.begin(), src.end(), 1);       // Values from 1.0 to n
    auto dst = std::vector<size_t>(src.size());
    auto transform_function = [](size_t v) {
        auto sum = v;
        sum = 2 * v + 1;
        return sum;
        };

    return std::tuple{ src, dst, transform_function };
}

static void test_transform_sequential() {

    auto [src, dst, func] = setup_simple_test(100);

    std::transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func
    );
}

static void test_transform_parallel() {

    auto [src, dst, func] = setup_simple_test(100);

    parallel_transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func
    );
}

static void test_transform_parallel_div_con() {

    auto [src, dst, func] = setup_simple_test(100);

    parallel_transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func,
        5
    );
}

// ===========================================================================

static auto setup_primes_calculation(size_t from, size_t to) {

    size_t size{ to - from };

    auto src{ std::vector<size_t>(size) };

    std::iota(src.begin(), src.end(), from);  // values starting with value 'from '

    auto dst{ std::vector<bool>(src.size()) };

    auto transformFunction{
        [](size_t number) -> bool { return isPrime(number); }
    };

    return std::tuple{ src, dst, transformFunction };
}

static void test_transform_primes_sequential(size_t from, size_t to) {

    auto [src, dst, func] = setup_primes_calculation(from, to);

    std::transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func
    );

    auto count = std::count_if(
        dst.begin(),
        dst.end(),
        [](int elem) { return elem; }
    );

    std::println("Found {} primes sequential", count);
}

static void test_transform_primes_parallel(size_t from, size_t to) {

    auto [src, dst, func] = setup_primes_calculation(from, to);

    ScopedTimer timer{};

    parallel_transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func
    );

    auto count = std::count_if(
        dst.begin(),
        dst.end(),
        [](int elem) { return elem; }
    );

    // std::println("Found {} primes parallel", count);
    Logger::log(std::cout, "Found ", count, " primes parallel");
}

static void test_transform_primes_parallel_div_con(size_t from, size_t to, size_t chunkSize) {

    auto [src, dst, func] = setup_primes_calculation(from, to);

    ScopedTimer timer{};

    parallel_transform(
        src.begin(),
        src.end(),
        dst.begin(),
        func,
        chunkSize
    );

    auto count = std::count_if(
        dst.begin(),
        dst.end(),
        [](int elem) { return elem; }
    );

  //  std::println("Found {} primes parallel (divide-conquer)", count);
    Logger::log(std::cout, "Found ", count, " primes parallel (divide-conquer)");
}

// ===========================================================================
// Snippets for Benchmark.com

#ifdef BENCHMARK

#include <thread>
#include <future>
#include <numeric>
#include <cmath>

const size_t From = 1000000000001;
const size_t To = From + 100;

bool isPrime(size_t number)
{
   ...
}

template <typename SrcIt, typename DstIt, typename Func>
static auto parallel_transform(SrcIt first, SrcIt last, DstIt dst, Func func) {
...

template <typename SrcIt, typename DstIt, typename Func>
static auto parallel_transform(SrcIt first, SrcIt last, DstIt dst, Func func, size_t chunkSize) {
...

static void bm_parallel(benchmark::State& state) {

    auto [src, dst, func] = setup_primes_calculation(From, To);

    for (auto _ : state) {
        parallel_transform(
            src.begin(),
            src.end(),
            dst.begin(),
            func
        );
    }
}

// Naive version
static void bm_parallel_div_com(benchmark::State& state) {

    auto [src, dst, func] = setup_primes_calculation(From, To);

    auto n = state.range(0);

    for (auto _ : state) {
        parallel_transform(
            src.begin(),
            src.end(),
            dst.begin(),
            func,
            n
        );
    }
}

static void CustomArguments(benchmark::internal::Benchmark* b) {
    b->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
}

BENCHMARK(bm_parallel)->Apply(CustomArguments);

BENCHMARK(bm_parallel_div_com)->Apply(CustomArguments)
  ->RangeMultiplier(10)        // Chunk size goes from
  ->Range(10, 10'000'000);     // 1k to 10M

#endif

// ===========================================================================

static void test_transform_simple() {

    test_transform_sequential();
    test_transform_parallel();
    test_transform_parallel_div_con();
}

static void test_transform_primes() {

    //test_transform_primes_sequential(Start, End);
    test_transform_primes_parallel(Start, End);
    test_transform_primes_parallel_div_con(Start, End, ChunkSize);
}

void test_transform() {

//    test_transform_simple();
    test_transform_primes();
}

// ===========================================================================
// End-of-File
// ===========================================================================
