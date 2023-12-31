// ===========================================================================
// Examples_RecursiveMutex.cpp // std::recursive_mutex
// ===========================================================================

#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>

// https://medium.com/@simontoth/daily-bit-e-of-c-std-recursive-mutex-dd9b84f38f8d

namespace Recursive_Mutex_Example
{
    constexpr size_t BucketSize = 4;

    class NonRecursive
    {
    private:
        std::mutex m_mutex;
        std::unique_ptr<int[]> m_data;
        size_t m_size;
        size_t m_capacity;

    public:
        NonRecursive() : m_size{}, m_capacity{} {}

        void push_back(int value) {

            std::unique_lock lock{ m_mutex };

            // We already hold m_mutex, so we couldn't call reserve()

            if (m_size == m_capacity) {
                allocate(m_capacity == 0 ? BucketSize : m_capacity * 2);
            }

            m_data[m_size++] = value;
        }

        void reserve(size_t capacity) {

            std::unique_lock lock{ m_mutex };
            allocate(capacity);
        }

        void print() {

            std::unique_lock lock{ m_mutex };

            for (size_t i{}; i != m_size; ++i) {
                std::cout << m_data[i] << ' ';
            }
            std::cout << std::endl;
        }

    private:
        // allocate expects m_mutex to be held by the caller

        void allocate(size_t capacity) {

            std::cout << "allocating " << capacity << std::endl;

            std::unique_ptr<int[]> data{ std::make_unique<int[]>(capacity) };

            size_t newSize{ std::min(m_size, capacity) };

            std::copy(
                m_data.get(),
                m_data.get() + newSize,
                data.get()
            );

            m_data = std::move(data);
            m_capacity = capacity;
            m_size = newSize;
        }
    };

    class Recursive
    {
    private:
        std::recursive_mutex m_mutex;
        std::unique_ptr<int[]> m_data;
        size_t m_size;
        size_t m_capacity;

    public:
        Recursive() : m_size{}, m_capacity{} {}

        void push_back(int value) {

            std::unique_lock lock{ m_mutex };

            // holding a recursive mutex multiple times is fine

            if (m_size == m_capacity) {
                reserve(m_capacity == 0 ? BucketSize : m_capacity * 2);
            }

            m_data[m_size++] = value;
        }

        void reserve(size_t capacity) {

            std::unique_lock lock{ m_mutex };

            std::cout << "allocating " << capacity << std::endl;

            std::unique_ptr<int[]> data{ std::make_unique<int[]>(capacity) };

            size_t newSize{ std::min(m_size, capacity) };

            std::copy(
                m_data.get(),
                m_data.get() + newSize,
                data.get()
            );

            m_data = std::move(data);
            m_capacity = capacity;
            m_size = newSize;
        }

        void print() {

            std::unique_lock lock{ m_mutex };

            for (size_t i{}; i != m_size; ++i) {
                std::cout << m_data[i] << ' ';
            }
            std::cout << std::endl;
        }
    };
}



void examples_recursive_mutex()
{
    using namespace Recursive_Mutex_Example;

    NonRecursive non;
    for (int i = 0; i < 18; i++) {
        non.push_back(i+1);
    }
    non.print();
    non.reserve(2);
    non.print();
 
    Recursive rec;
    for (int i = 0; i < 18; i++) {
        rec.push_back(100 + i+1);
    }
    rec.print();
    rec.reserve(2);
    rec.print();
}


// ===========================================================================
// End-of-File
// ===========================================================================
