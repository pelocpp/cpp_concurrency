// ===========================================================================
// ReaderWriterLock02.cpp // Reader Writer Lock
// ===========================================================================

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <optional>
#include <exception>

#include "../Logger/Logger.h"

namespace Reader_Writer
{
    struct Data
    {
        int m_data;
    };

    template <size_t TSize = 64>
    class Snapshots
    {
    private:
        std::array<Data, TSize>   m_buffer;
        size_t                    m_offset;
        mutable std::shared_mutex m_mutex;

    public:
        Snapshots() : m_buffer{}, m_offset{} {}

        void push(const Data& data) {

            // we are about to modify the data, therefore we need a unique_lock
            std::unique_lock<std::shared_mutex> lock{ m_mutex };
            m_buffer[m_offset % TSize] = data;
            ++m_offset;
        }

        std::optional<Data> get(size_t index) const {

            // we only read, but need to prevent concurrent writes,
            // therefore we are using a shared_lock
            std::shared_lock<std::shared_mutex> lock{ m_mutex };

            if (index >= TSize) {
                std::string msg{ 
                    std::string{ "Wrong Index:" } +
                    std::to_string(index) +
                    std::string{ " !" }
                };
                
                throw std::out_of_range{ msg };
            }

            if (m_buffer[index].m_data != 0) {
                return m_buffer[index];
            }
            else {
                return std::nullopt;
            }
        }
    };
}

    static void test_reader_writer_lock_01()
    {
        using namespace Reader_Writer;
        using namespace std::chrono_literals;

        constexpr size_t Size{ 64 };

        Snapshots<Size> snapshots{};

        std::jthread thread { 
            [&] (std::stop_token stop) {
                int count{};
                while (!stop.stop_requested()) {
                    std::this_thread::sleep_for(100ms);
                    ++count;
                    Logger::log(std::cout, "Writer:    pushing ", count);
                    snapshots.push(Data{ count });
                }
            }
        };

        // run next while loop for two seconds
        std::chrono::system_clock::time_point deadline{
            std::chrono::system_clock::now() + 4s 
        };

        std::chrono::system_clock::duration pause{ 15ms };

        while (true) {

            if (std::chrono::system_clock::now() > deadline)
                break;

            // start two readers reading concurrently snapshots
            int counter1{};
            int counter2{};

            std::jthread reader1 { 
                [&] () {
                    for (size_t i{}; i != Size; i++) {
                        if (snapshots.get(i).has_value()) {
                            ++ counter1;
                        }
                    }

                  // Logger::log(std::cout, "1. reader: counted ", counter1, " snapshots");
                } 
            };

            std::jthread reader2{
                [&] () {
                    for (size_t i{}; i != Size; i++) {
                        if (snapshots.get(i).has_value()) {
                            ++counter2;
                        }
                    }

                  // Logger::log(std::cout, "2. reader: counted ", counter2, " snapshots");
                }
            };

            reader1.join();
            reader2.join();

            // Note:
            // join() introduces a synchronization point, meaning that
            // the main thread can now safely read counter1 and counter2
            // despite these variables not being atomic.

            Logger::log(std::cout, "1. Reader: ", counter1, " snapshots");
            Logger::log(std::cout, "2. Reader: ", counter2, " snapshots");

            std::this_thread::sleep_for(pause);
        }

        // stop the writer thread
        thread.request_stop();
    }

void test_reader_writer_lock_02()
{
    using namespace Reader_Writer;

    test_reader_writer_lock_01();
}

// ===========================================================================
// End-of-File
// ===========================================================================
