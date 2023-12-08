#include <iostream>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <array>
#include <optional>

namespace Reader_Writer
{
    struct Data {};

    template <size_t TRecentCount = 64>
    class Snapshots
    {
    private:
        // We need mutable, since we mutate the state
        // of this mutex (by grabbing a lock) in const methods.

        std::array<Data, TRecentCount> m_buffer;
        size_t m_offset = 0;
        mutable std::shared_mutex m_mutex;

    public:
        void push(const Data& data) {
            // We are about to modify the data, grab a unique_lock
            std::unique_lock lock(m_mutex);
            m_buffer[m_offset % TRecentCount] = data;
            ++m_offset;
        }

        std::optional<Data> get(size_t index) const {
            // We only read, but need to prevent 
            // concurrent writes, grab a shared_lock
            std::shared_lock lock(m_mutex);
            if (index >= m_offset)
                return std::nullopt;
            if (m_offset >= TRecentCount && m_offset - TRecentCount > index)
                return std::nullopt;
            return m_buffer[index % TRecentCount];
        }

        size_t min_offset() const {
            // We only read, but need to prevent 
            // concurrent writes, grab a shared_lock
            std::shared_lock lock(m_mutex);
            if (m_offset <= TRecentCount) return 0;
            return m_offset - TRecentCount;
        }

    };
}

void test_reader_writer_lock_01()
{
    using namespace Reader_Writer;

    using namespace std::chrono_literals;

    constexpr size_t RecentCount = 64;

    Snapshots<RecentCount> snapshots;

    // Writer that generates snapshots
    auto t = std::jthread([&snapshots](std::stop_token stop) {
        while (!stop.stop_requested()) {
            std::this_thread::sleep_for(100ms);
            snapshots.push(Data{});
        }
        });

    // Run for two seconds.
    auto deadline = std::chrono::system_clock::now() + 2s;

    while (true) {
        if (std::chrono::system_clock::now() > deadline)
            break;

        // Start two readers that will concurently read snapshots.
        int r1_cnt = 0;
        auto r1 = std::jthread([&snapshots, &r1_cnt]() {
            size_t offset = snapshots.min_offset();
            for (size_t i = offset; i < offset + RecentCount; i++)
                r1_cnt += snapshots.get(i) != std::nullopt;
            });
        int r2_cnt = 0;
        auto r2 = std::jthread([&snapshots, &r2_cnt]() {
            size_t offset = snapshots.min_offset();
            for (size_t i = offset; i < offset + RecentCount; i++)
                r2_cnt += snapshots.get(i) != std::nullopt;
            });
        r1.join();
        r2.join();
        // Note: join() introduces a synchronization point, meaning that
        // the main thread can now safely read r1_cnt and r2_cnt despite
        // these variables not being atomic.

        std::cout << "First reader read " << r1_cnt << " snapshots\n";
        std::cout << "Second reader read " << r2_cnt << " snapshots\n";
        std::this_thread::sleep_for(50ms);
    }

    // Stop the writer
    t.request_stop();

}



void test_reader_writer_lock()
{
    using namespace Reader_Writer;

    test_reader_writer_lock_01();
}