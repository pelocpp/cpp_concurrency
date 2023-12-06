// Übernommen:  Siehe 20_Threadsafe_Stack

// von Anthony Williams
//
//#include <iostream>
//#include <exception>
//#include <stack>
//#include <vector>
//#include <mutex>
//#include <memory>
//#include <future>
//#include <optional>
//#include <cmath>
//#include <chrono>
//
//#include "../Logger/Logger.h"
//
//namespace Globals
//{
//    // https://www.michael-holzapfel.de/themen/primzahlen/pz-anzahl.htm
//
//    constexpr size_t NumThreads = 8;
//
//    constexpr size_t LowerLimit = 1;
//    constexpr size_t UpperLimit = 100;
//
//    //constexpr size_t LowerLimit = 1;
//    //constexpr size_t UpperLimit = 10'000'000;
//    //// Found:  664579 prime numbers
//
//    //constexpr size_t LowerLimit = 1'000'000;
//    //constexpr size_t UpperLimit = 10'000'000;
//
//    //constexpr size_t LowerLimit = 1;
//    //constexpr size_t UpperLimit = 100'000;
//    //// Found: 9.592 prime numbers
//
//    //constexpr size_t LowerLimit = 1;
//    //constexpr size_t UpperLimit = 1'000'000;
//    //// Found: 78.498 prime numbers
//
//    //constexpr size_t LowerLimit = 1;
//    //constexpr size_t UpperLimit = 10'000'000;
//    //// Found:  664.579 prime numbers
//
//    //constexpr size_t LowerLimit = 1;
//    //constexpr size_t UpperLimit = 100'000'000;
//    //// Found:  5.761.455 prime numbers
//}
//
//namespace Concurrency_ThreadsafeStack
//{
//    struct empty_stack : std::exception
//    {
//    private:
//        std::string m_what;
//
//    public:
//        explicit empty_stack() : m_what{ std::string{ "Stack is empty!" } } {}
//
//        explicit empty_stack(std::string msg) {
//            m_what = std::move(msg);
//        }
//
//        const char* what() const noexcept override { 
//            return m_what.c_str();
//        }
//    };
//
//    template<typename T>
//    class ThreadsafeStack
//    {
//    private:
//        std::stack<T>      m_data;
//        mutable std::mutex m_mutex;  // beachte mutable
//
//    public:
//        // c'tors
//        ThreadsafeStack() {}
//
//        // prohibit copy constructor, assignment operator and move assignment
//        ThreadsafeStack(const ThreadsafeStack&) = delete;
//        ThreadsafeStack& operator = (const ThreadsafeStack&) = delete;
//        ThreadsafeStack& operator = (ThreadsafeStack&&) noexcept = delete;
//
//        // move constructor may be useful
//        ThreadsafeStack(const ThreadsafeStack&& other) noexcept
//        {
//            std::lock_guard<std::mutex> lock(other.m_mutex);
//            m_data = other.m_data;
//        }
//
//        // public interface
//        void push(T new_value)
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            m_data.push(new_value);
//        }
//
//        void pop(T& value)
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            if (m_data.empty()) throw empty_stack{};
//            value = m_data.top();
//            m_data.pop();
//        }
//
//        T tryPop()
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
//            T value = m_data.top();
//            m_data.pop();
//            return value;
//        }
//
//        std::optional<T> tryPopOptional()
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            if (m_data.empty()) {
//                return std::nullopt;
//            }
//
//            std::optional<T> result{ m_data.top() };
//            m_data.pop();
//            return result;
//        }
//
//        size_t size() const
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            return m_data.size();
//        }
//
//        bool empty() const
//        {
//            std::lock_guard<std::mutex> lock{ m_mutex };
//            return m_data.empty();
//        }
//    };
//
//    template <typename T>
//    class PrimeCalculator
//    {
//    private:
//        ThreadsafeStack<T>& m_stack;
//        size_t m_begin;
//        size_t m_end;
//
//    public:
//        PrimeCalculator(ThreadsafeStack<T>& stack, size_t begin, size_t end)
//            : m_stack{ stack }, m_begin{ begin }, m_end{ end }
//        {}
//
//        void operator() ()
//        {
//            std::thread::id tid{ std::this_thread::get_id() };
//            Logger::log(std::cout, "TID:  ", tid);
//
//            for (size_t i{ m_begin }; i != m_end; ++i) {
//
//              //  Logger::log(std::cout, "         calculating ", i, " ...");
//
//                if (isPrime(i)) {
//                    m_stack.push(i);
//                }
//            }
//        }
//
//    private:
//        static bool isPrime(size_t number)
//        {
//            // the smallest prime number is 2
//            if (number <= 2) {
//                return number == 2;
//            }
//
//            // even numbers other than 2 are not prime
//            if (number % 2 == 0) {
//                return false;
//            }
//
//            // check odd divisors from 3 to the square root of the number
//            size_t end{ static_cast<size_t>(ceil(std::sqrt(number))) };
//            for (size_t i{ 3 }; i <= end; i += 2) {
//                if (number % i == 0) {
//                    return false;
//                }
//            }
//
//            // found prime number
//            return true;
//        }
//    };
//}
//
//void test_thread_safe_stack_01_from_loos()
//{
//    using namespace Concurrency_ThreadsafeStack;
//
//    ThreadsafeStack<int> stack{};
//
//    stack.push(1);
//    stack.push(2);
//    stack.push(3);
//
//    int n{};    
//    stack.pop(n);
//    std::cout << "got " << n << std::endl;
//
//    std::optional<int> m;
//    m = stack.tryPopOptional();
//    std::cout << "got " << m.value() << std::endl;
//
//    n = stack.tryPop();
//    std::cout << "got " << n << std::endl;
//
//    // n = stack.tryPop();  // crashes
//}
//
//void test_thread_safe_stack_02_from_loos()
//{
//    using namespace Concurrency_ThreadsafeStack;
//
//    const auto startTime{ std::chrono::high_resolution_clock::now() };
//
//    std::cout << "Calcalating Prime Numbers from " << Globals::LowerLimit << " up to " << Globals::UpperLimit << ':' << std::endl;
//
//    ThreadsafeStack<size_t> primes{};
//
//    PrimeCalculator<size_t> calc{ primes, Globals::LowerLimit, Globals::UpperLimit + 1 };
//
//    std::thread calculator(calc);
//
//    calculator.join();
//
//    const auto endTime{ std::chrono::high_resolution_clock::now() };
//    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();
//
//    std::cout << "Found: " << primes.size() << " prime numbers." << std::endl;
//    std::cout << "Done:  " << msecs << " msecs." << std::endl;
//}
//
//
//void test_thread_safe_stack_03_from_loos()
//{
//    using namespace Concurrency_ThreadsafeStack;
//
//    const auto startTime{ std::chrono::high_resolution_clock::now() };
//
//    std::cout << "Calcalating Prime Numbers from " << Globals::LowerLimit << " up to " << Globals::UpperLimit << ':' << std::endl;
//
//    ThreadsafeStack<size_t> primes{};
//
//    std::vector<std::thread> threads;
//    threads.reserve(Globals::NumThreads);
//
//    size_t range = (Globals::UpperLimit - Globals::LowerLimit) / Globals::NumThreads;
//    size_t start = Globals::LowerLimit;
//    size_t end = start + range;
//
//    for (size_t i{}; i != Globals::NumThreads - 1; ++i) {
//
//        PrimeCalculator<size_t> calc{ primes, start, end };
//        threads.emplace_back(calc);
//
//        start = end;
//        end = start + range;
//    }
//
//    // setup last thread
//    end = Globals::UpperLimit;
//    PrimeCalculator<size_t> calc{ primes, start, end + 1 };
//    threads.emplace_back(calc);
//
//    // wait for end of all threads
//    for (size_t i{}; i != Globals::NumThreads; ++i) {
//        threads[i].join();
//    }
//
//    const auto endTime{ std::chrono::high_resolution_clock::now() };
//    double msecs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();
//
//    std::cout << "Found: " << primes.size() << " prime numbers." << std::endl;
//    std::cout << "Done:  " << msecs << " msecs." << std::endl;
//}
//
//void test_thread_safe_stack()
//{
//    test_thread_safe_stack_03_from_loos();
//}
//
//// ===========================================================================
//// End-of-File
//// ===========================================================================
