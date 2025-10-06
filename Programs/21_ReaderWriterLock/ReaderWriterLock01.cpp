// ===========================================================================
// ReaderWriterLock01.cpp // Reader Writer Lock
// ===========================================================================

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "../Logger/ScopedTimer.h"

constexpr int RegularLocking{ 1 };
constexpr int SharedLocking{ 2 };

constexpr int LockingMode{ SharedLocking };

constexpr size_t NumIterations{ 10'000'000 };

namespace Reader_Writer
{
    class DataContainer
    {
    private:
        size_t m_data;

        mutable std::mutex m_mutex;
        mutable std::shared_mutex m_shared_mutex;

    public:
        DataContainer() : m_data{} {}

        size_t getValue() const { return m_data; }

        void write() {

            Logger::log(std::cout, "Start Writing ...");

            for (size_t i{}; i != NumIterations; ++i) {

                if constexpr (LockingMode == RegularLocking) {

                    std::lock_guard guard{ m_mutex };
                    ++m_data;
                }

                if constexpr (LockingMode == SharedLocking) {

                    std::unique_lock guard{ m_shared_mutex };
                    ++m_data;
                }
            }

            Logger::log(std::cout, "Writing Done.");
        }

        void read() {

            Logger::log(std::cout, "Start Reading ...");

            size_t copy{};

            while (copy < NumIterations) {

                if constexpr (LockingMode == RegularLocking) {

                    std::lock_guard guard{ m_mutex };
                    copy = m_data;
                }

                if constexpr (LockingMode == SharedLocking) {

                    std::shared_lock<std::shared_mutex> guard{ m_shared_mutex };
                    copy = m_data;
                }
            }

            Logger::log(std::cout, "Reading Done.");
        }
    };
}

static void test_reader_writer_lock_0xx()
{
    using namespace Reader_Writer;

    Logger::log(std::cout, "Start");

    DataContainer container;

    ScopedTimer watch{};

    std::jthread t0{ [&] { container.write(); } };
    std::jthread t1{ [&] { container.read(); } };
    std::jthread t2{ [&] { container.read(); } };
    std::jthread t3{ [&] { container.read(); } };
    std::jthread t4{ [&] { container.read(); } };

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    Logger::log(std::cout, "Value: ", container.getValue());

    Logger::log(std::cout, "Done.");
}

void test_reader_writer_lock_01()
{

    test_reader_writer_lock_0xx();
}
// ===========================================================================
// End-of-File
// ===========================================================================
