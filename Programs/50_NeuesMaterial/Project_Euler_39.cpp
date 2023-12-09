#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <sstream>
#include <algorithm>
#include <functional>
#include <thread>
#include <vector>
#include <thread>
#include <format>

// https://stackoverflow.com/questions/36246300/parallel-loops-in-c

namespace Concurrency_Parallel_For_Ex
{
    using Callable = std::function<void(size_t start, size_t end)>;

    static void parallel_for(
        size_t numElements,
        Callable callable,
        bool useThreads)
    {
        // calculate number of threads to use
        size_t numThreadsHint{ std::thread::hardware_concurrency() };
        size_t numThreads{ (numThreadsHint == 0) ? 8 : numThreadsHint };
        size_t batchSize{ numElements / numThreads };
        size_t batchRemainder{ numElements % numThreads };

        // allocate vector of uninitialized thread objects
        std::vector<std::thread> threads;

        if (useThreads) {

            // prepare multi-threaded execution
            for (size_t i{}; i != numThreads; ++i) {

                size_t start{ i * batchSize };
                threads.push_back(std::move(std::thread{ callable, start, start + batchSize }));
            }
        }
        else {

            // prepare single-threaded execution (for easy debugging)
            for (size_t i{}; i < numThreads; ++i) {

                size_t start{ i * batchSize };
                callable(start, start + batchSize);
            }
        }

        // take care of last element - calling 'callable' synchronously 
        size_t start{ numThreads * batchSize };
        callable(start, start + batchRemainder);

        // wait for the other thread to finish their task
        if (useThreads) {
           std::for_each(
               threads.begin(),
               threads.end(),
               std::mem_fn(&std::thread::join)
           );
        }

        std::cout << "Done." << std::endl;
    }
}

namespace Project_Euler_39
{
    class PythagoreanTriple
    {
    private:
        std::array<size_t, 3> m_numbers;

    public:
        PythagoreanTriple(size_t x, size_t y, size_t z) 
            : m_numbers { x, y, z}
        {}

        size_t circumference () {
            return m_numbers[0] + m_numbers[1] + m_numbers[2];
        }

        std::string toString() {
            return std::format("[{:02},{:02},{:02}]",
                m_numbers[0], m_numbers[1], m_numbers[2]);
        }
    };

    class PythagoreanTripleCalculator
    {
    private:
        size_t m_maxNumber;
        size_t m_maxCircumference;
        size_t m_total;

        std::vector<PythagoreanTriple> m_triples;

    public:
        PythagoreanTripleCalculator() 
            : m_maxNumber{}, m_maxCircumference{}, m_total{}
        {}

        // sequential interface
        void computeAll(size_t maxCircumference)
        {
            for (size_t circ{ 3 }; circ <= maxCircumference; ++circ) {
            
                for (size_t found{}, a{ 1 }; a <= circ; ++a) {
                
                    for (size_t b{ a }; b <= circ; ++b) {
                    
                        size_t c{ circ - a - b };

                        if (a * a + b * b == c * c) {
                        
                            // store this pythagorean triple
                            m_triples.emplace_back(a, b, c);
                            m_total++;

                            found++;
                            if (found > m_maxNumber)
                            {

                                m_maxNumber = found;
                                m_maxCircumference = circ;
                            }
                        }
                    }
                }
            }
        }

        void computeAllEx(size_t maxCircumference)
        {
            for (size_t c{ 3 }; c <= maxCircumference; ++c) {

                computeAllRectangles(c);
            }
        }

        void computeAllRectangles(size_t circ)
        {
            for (size_t found{}, a{ 1 }; a <= circ; ++a) {
            
                for (size_t b{ a }; b <= circ; ++b) {
                
                    size_t c{ circ - a - b };

                    if (a * a + b * b == c * c) {
                    
                        // store this pythagorean triple
                        m_triples.emplace_back(a, b, c);  //NEED THREAD SAFE QUEUE !!!
                        m_total++;

                        found++;
                        if (found > m_maxNumber)
                        {

                            m_maxNumber = found;
                            m_maxCircumference = circ;
                        }
                    }
                }
            }
        }

        // concurrent interface
        void processRange(size_t start, size_t end)
        {
            for (size_t i{ start }; i != end; ++i) {
                computeAllRectangles(i);
            }
        }

        void process(size_t maximum, bool useThreads)
        {
            Concurrency_Parallel_For_Ex::parallel_for(
                maximum, 
                [this] (size_t start, size_t end) { processRange(start, end); },
                useThreads
            );
        }

        // getter
        size_t triplesCount() { return m_triples.size(); }

        std::string toString()
        {
            return std::format("Total: {}\n# identically circumferences: {} at {}",
                m_total, m_maxNumber, m_maxCircumference);
        }

        // helper method
        void dumpStack()
        {
            if (m_triples.empty())
                return;

            size_t lastCircumference{ m_triples[0].circumference() };

            for (size_t i{}; i != m_triples.size(); i++) {
            
                if (m_triples[i].circumference() != lastCircumference) {
                
                    lastCircumference = m_triples[i].circumference();
                    std::cout << '\n';
                }

                std::string s{ m_triples[i].toString() };
                std::cout << std::format("{0}: {1}", m_triples[i].circumference(), s) << std::endl;
            }
        }
    };
}

void test_project_euler_39_01()
{
    using namespace Project_Euler_39;

    PythagoreanTripleCalculator calculator;

    //calculator.computeAll(20);
    //calculator.computeAllEx(20);
    calculator.process(1000, false);
   
    calculator.dumpStack();
    std::cout << calculator.toString() << std::endl;
}

void test_project_euler_39()
{
    test_project_euler_39_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
