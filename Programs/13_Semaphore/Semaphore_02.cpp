// ===========================================================================
// Semaphore // Semaphore_02.cpp
// ===========================================================================

#include "../Logger/Logger.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <semaphore>
#include <random>
#include <array>

namespace ConcurrencyBinarySemaphore
{
    static std::string prefix() {
        std::stringstream ss;
        ss << "Thread " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << std::this_thread::get_id() << ": ";
        return ss.str();
    }

    class Document {};

    class PrinterQueue
    {
    private:
        std::binary_semaphore                  m_semaphore;
        std::random_device                     m_device;
        std::uniform_int_distribution<size_t>  m_distribution;

    public:
        PrinterQueue() : m_semaphore{ 1 }, m_distribution{ 500, 2000 } {}

    public:
        void printJob(Document document)
        {
            m_semaphore.acquire();

            size_t duration{ m_distribution(m_device) };
            std::string msecs{ std::to_string(duration) };
            Logger::log(std::cout, "PrinterQueue: Printing a Job during ", msecs, " millseconds.");

            std::this_thread::sleep_for(std::chrono::milliseconds{ duration });
            Logger::log(std::cout, "PrinterQueue: The document has been printed");

            m_semaphore.release();
        }
    };

    class PrintingJob
    {
    private:
        PrinterQueue& m_printerQueue;

    public:
        PrintingJob(PrinterQueue& printerQueue) : m_printerQueue{ printerQueue } {}

        void operator () () const {
            Logger::log(std::cout, "PrintingJob:  Going to enqueue a document");
            m_printerQueue.printJob(Document{});
        }
    };
}

void test_binary_semaphore_02()
{
    using namespace ConcurrencyBinarySemaphore;

    constexpr size_t NumJobs{ 8 };

    PrinterQueue printerQueue{};

    std::array<std::thread, NumJobs> threads;

    for (size_t i{}; i != NumJobs; i++) {

        std::thread t{ PrintingJob{ printerQueue } };

        threads.at(i) = std::move(t);
    }

    for (size_t i{}; i != NumJobs; i++) {
        threads[i].join();
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
