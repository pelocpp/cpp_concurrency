#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <optional>
#include <exception>

namespace Reader_Writer
{
    struct Data
    {
        int m_data;
    };

    template <size_t TRecentCount = 64>
    class Snapshots
    {
    private:
        std::array<Data, TRecentCount>  m_buffer;
        size_t                          m_offset;
        mutable std::shared_mutex       m_mutex;

    public:
        Snapshots() : m_buffer{}, m_offset{} {}

        void push(const Data& data) {

            // we are about to modify the data, therefore we need a unique_lock
            std::unique_lock<std::shared_mutex> lock{ m_mutex };
            m_buffer[m_offset % TRecentCount] = data;
            ++m_offset;
        }

        std::optional<Data> get(size_t index) const {

            // we only read, but need to prevent concurrent writes,
            // therefore we are using a shared_lock
            std::shared_lock<std::shared_mutex> lock{ m_mutex };

            if (index >= TRecentCount) {
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

void test_reader_writer_lock_01()
{
    using namespace Reader_Writer;
    using namespace std::chrono_literals;

    constexpr size_t RecentCount{ 64 };

    Snapshots<RecentCount> snapshots{};

    std::jthread thread { 
        [&] (std::stop_token stop) {
            int count{};
            while (!stop.stop_requested()) {
                std::this_thread::sleep_for(100ms);
                ++count;
                snapshots.push(Data{ count });
            }
        }
    };

    // run next while loop for two seconds
    std::chrono::system_clock::time_point deadline{
        std::chrono::system_clock::now() + 2s 
    };

    std::chrono::system_clock::duration pause{ 20ms };

    while (true) {

        if (std::chrono::system_clock::now() > deadline)
            break;

        // start two readers reading concurrently snapshots
        int counter1{};
        int counter2{};

        std::jthread reader1 { 
            [&]() {
                for (size_t i{}; i < RecentCount; i++) {
                    if (snapshots.get(i) != std::nullopt) {
                        ++ counter1;
                    }
                }
            } 
        };

        std::jthread reader2{
            [&]() {
                for (size_t i{}; i < RecentCount; i++) {
                    if (snapshots.get(i) != std::nullopt) {
                        ++counter2;
                    }
                }
            }
        };

        reader1.join();
        reader2.join();

        // Note:
        // join() introduces a synchronization point, meaning that
        // the main thread can now safely read counter1 and counter2
        // despite these variables not being atomic.

        std::cout << "1. reader: " << counter1 << " snapshots" << std::endl;
        std::cout << "2. reader: " << counter2 << " snapshots" << std::endl;

        std::this_thread::sleep_for(pause);
    }

    // stop the writer thread
    thread.request_stop();
}


void test_reader_writer_lock()
{
    using namespace Reader_Writer;

    test_reader_writer_lock_01();
}