// ===========================================================================
// Semaphore // Semaphore_02.cpp
// ===========================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <semaphore>
#include <random>

// https://howtodoinjava.com/java/multi-threading/binary-semaphore-tutorial-and-example/

namespace ConcurrencyBinarySemaphore
{
    std::string prefix() {
        std::stringstream ss;
        ss << "Thread " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << std::this_thread::get_id() << ": ";
        return ss.str();
    }

    std::string toString(std::string s) {
        std::stringstream ss;
        ss << prefix() << s << '\n';
        return ss.str();
    }

    std::string toString(std::string s1, std::string s2) {
        std::stringstream ss;
        ss << prefix() << s1 << s2 << '\n';
        return ss.str();
    }

    std::string toString(std::string s1, std::string s2, std::string s3) {
        std::stringstream ss;
        ss << prefix() << s1 << s2 << s3 << '\n';
        return ss.str();
    }

    class Document {};

    class PrinterQueue
    {
    private:
        std::binary_semaphore m_semaphore;
        std::random_device m_device;
        std::uniform_int_distribution<size_t> m_distribution;

    public:
        PrinterQueue() : m_semaphore{ 1 }, m_distribution{ 500, 2000 } {}

    public:
        void printJob(Document document)
        {
            //     using namespace std::chrono_literals;

            m_semaphore.acquire();

            size_t duration{ m_distribution(m_device) };
            std::string msecs{ std::to_string(duration) };
            std::cout << toString("PrinterQueue: Printing a Job during ", msecs, " millseconds.");

            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            std::cout << toString("PrinterQueue: The document has been printed");

            m_semaphore.release();
        }
    };

    class PrintingJob
    {
    private:
        PrinterQueue& m_printerQueue;

    public:
        PrintingJob(PrinterQueue& printerQueue) : m_printerQueue{ printerQueue } {}

        void run()
        {
            std::cout << toString("PrintingJob:  Going to enqueue a document");

            m_printerQueue.printJob(Document{});
        }
    };
}

void test_binary_semaphore_02()
{
    using namespace ConcurrencyBinarySemaphore;

    constexpr size_t NumJobs{ 10 };

    PrinterQueue printerQueue{};

    std::thread threads[NumJobs];

    for (size_t i{}; i != NumJobs; i++) {

        PrintingJob job{ printerQueue };

        std::thread t{ &PrintingJob::run, &job };

        threads[i] = std::move(t);
    }

    for (size_t i{}; i != NumJobs; i++) {
        threads[i].join();
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================

