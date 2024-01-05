#include <iostream>
#include <queue>
#include <thread>
#include <future>
#include <stop_token>
#include <mutex>
#include <condition_variable>

#include <syncstream>

namespace Concurrency_Stop_Tokens_and_Condition_Variables
{
    static void test_01()
    {
        std::queue<std::string>     m_messages;
        std::mutex                  m_mutex;
        std::condition_variable_any m_condition_variable;

        auto task = [&] (std::stop_token token) {
            
            while (! token.stop_requested()) {

                std::string msg{};

                std::cout << "Waiting  ..." << std::endl;

                {
                    std::unique_lock lock{ m_mutex };

                    // wait for the next message
                    bool stopRequested {
                        m_condition_variable.wait(
                            lock,
                            token,
                            [&] () { return ! m_messages.empty(); }
                        )
                    };

                    if (!stopRequested) {
                        std::cout << "Stop has been requested!" << std::endl;
                        return;
                    }

                    // retrieve the next message from the queue
                    msg = m_messages.front();
                    m_messages.pop();
                }

                // print the next message:
                std::cout << "Message: " << msg << std::endl;
            }
        };

        std::jthread t { task };

        std::cout << "Pushing strings into queue" << std::endl;

        // store three messages
        for (std::string s : { std::string{ "Tic" }, std::string{ "Tac" }, std::string{ "Toe" }}) {

            // std::scoped_lock lg{ m_mutex };
            std::lock_guard guard { m_mutex };
            m_messages.push(s);
        }

        // notify waiting thread
        m_condition_variable.notify_one();

        // after some time, store another message
        std::this_thread::sleep_for(std::chrono::seconds(3));

        {
            std::lock_guard guard{ m_mutex };
            m_messages.push("Done");
        }

        // notify waiting thread
        m_condition_variable.notify_one();

        // after some time, end program (requests stop, which interrupts wait())
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

// =============================================================================

namespace Concurrency_Stop_Tokens_and_Condition_Variables
{
    auto syncOut(std::ostream& strm = std::cout) {
        return std::osyncstream{ strm };
    }



    void task(std::stop_token st, int num)
    {
        auto id = std::this_thread::get_id();
        syncOut() << "call task(" << num << ")\n";

        // register a first callback:
        std::stop_callback cb1{ st, [num, id] {
        syncOut() << "- STOP1 requested in task(" << num
        << (id == std::this_thread::get_id() ? ")\n"
        : ") in main thread\n");
        } };
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        // register a second callback:
        std::stop_callback cb2{ st, [num, id] {
        syncOut() << "- STOP2 requested in task(" << num
        << (id == std::this_thread::get_id() ? ")\n"
        : ") in main thread\n");
        } };
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }



    static void test_02()
    {
        // create stop_source and stop_token:
        std::stop_source ssrc;
        std::stop_token stok{ ssrc.get_token() };
        // register callback:
        std::stop_callback cb{ stok, [] {
        syncOut() << "- STOP requested in main()\n" << std::flush;
        } };
        // in the background call task() a bunch of times:
        auto fut = std::async([stok] {
            for (int num = 1; num < 10; ++num) {
                task(stok, num);
            }
            });
        // after a while, request stop:
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ssrc.request_stop();
    }
}

void test_Stop_Tokens_and_Condition_Variables()
{
    //Concurrency_Stop_Tokens_and_Condition_Variables::test_01();
    Concurrency_Stop_Tokens_and_Condition_Variables::test_02();
}
