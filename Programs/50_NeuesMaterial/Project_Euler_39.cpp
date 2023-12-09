#include <iostream>

#include <string>
#include <sstream>

#include <array>
#include <sstream>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <thread>
#include <memory>
#include <cassert>
#include <format>
// #include <stack>


namespace Project_Euler_39
{
    // da ginge auch ein std::tuple ...
    class PythagoreanTriple
    {
    private:
        std::array<size_t, 3> m_numbers;

    public:
        PythagoreanTriple(size_t x, size_t y, size_t z)
        {
            m_numbers[0] = x;
            m_numbers[1] = y;
            m_numbers[2] = z;
        }

        size_t perimeter() {
            return m_numbers[0] + m_numbers[1] + m_numbers[2];
        }

        std::string toString() {
            return std::format("[{:02},{:02},{:02}]", m_numbers[0], m_numbers[1], m_numbers[2]);
            //return "";
        }
    };

    class PythagoreanTripleCalculator
    {
    private:
        size_t m_maxNumber;
        size_t m_maxPerimeter;
        size_t m_total;

        std::vector<PythagoreanTriple> m_stack;

    public:
        PythagoreanTripleCalculator() 
            : m_maxNumber{}, m_maxPerimeter{}, m_total{}
        {}

        void compute(int maximum)
        {
            for (size_t p = 3; p <= maximum; p++)
            {
                size_t found = 0;

                for (size_t a = 1; a <= p; a++)
                {
                    for (size_t b = a; b <= p; b++)
                    {
                        size_t c = p - a - b;

                        if (a * a + b * b == c * c)
                        {
                            // store this pythagorean riple
                            //PythagoreanTriple triple{ a, b, c };
                            //m_stack.push_back(triple);

                            m_stack.emplace_back(a, b, c);

                            m_total++;

                            found++;

                            if (found > m_maxNumber)
                            {

                                m_maxNumber = found;
                                m_maxPerimeter = p;
                            }
                        }
                    }
                }
            }
        }

        // getter
        size_t triplesCount() { return m_stack.size(); }

        std::string toString()
        {
            std::stringstream ss;

            ss << "Total: " << m_total << '\n';

            ss << " identically Perimeters:  " << m_maxNumber << " at number "  << m_maxPerimeter <<  '\n';

            //Console.WriteLine("Total: {0}", this.total);

            //return String.Format("# identically Perimeters: {0} at Number {1}",
            //    this.maxNumber, this.maxPerimeter);

            return ss.str();
        }

        // helper method
        void dumpStack()
        {
            if (m_stack.empty())
                return;

            size_t lastPerimeter = m_stack[0].perimeter();

            for (size_t i = 0; i < m_stack.size(); i++)
            {
                if (m_stack[i].perimeter() != lastPerimeter)
                {
                    lastPerimeter = m_stack[i].perimeter();
                    std::cout << '\n';
                }
                //Console.WriteLine("{0}: {1}", m_stack[i].Perimeter, m_stack[i]);

                std::string stripel = m_stack[i].toString();

                //std::cout << m_stack[i].perimeter() << std::string{ " - " } << stripel << std::endl;

              //   std::string s = std::format("{0}: {1}", m_stack[i].perimeter(), m_stack[i]);

                std::cout << std::format("{0}: {1}", m_stack[i].perimeter(), stripel) << std::endl;
            }
        }
    };
}

void test_project_euler_39_01()
{
    using namespace Project_Euler_39;

    PythagoreanTripleCalculator calculator;

    calculator.compute(3000);
   // calculator.dumpStack();

    //Console.WriteLine();
    std::cout << calculator.toString() << std::endl;
}

void test_project_euler_39()
{
    test_project_euler_39_01();
}
