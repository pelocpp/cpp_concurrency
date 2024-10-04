// ===========================================================================
// ConditionVariableAny.cpp // std::stop_token, std::stop_source und request_stop
// ===========================================================================

#include <iostream>
#include <queue>
#include <thread>
#include <future>
#include <stop_token>
#include <mutex>
#include <condition_variable>

#include "../Logger/Logger.h"

namespace Stop_Tokens_and_Condition_Variables
{
    static void test_01()
    {
        std::queue<std::string>     m_messages;
        std::mutex                  m_mutex;
        std::condition_variable_any m_condition_variable;

        auto task = [&](std::stop_token token) {

            while (!token.stop_requested()) {

                std::string msg{};

                Logger::log(std::cout, "Waiting  ...");

                {
                    std::unique_lock lock{ m_mutex };

                    // wait for the next message
                    bool stopRequested {
                        m_condition_variable.wait(
                            lock,
                            token,
                                [&]() {
                                bool b { !m_messages.empty()};
                                Logger::log(std::cout, "Wait: Queue is empty: ", b ? "false" : "true");
                                return b;
                            }

                        )
                    };

                    if (!stopRequested) {
                        Logger::log(std::cout, "Stop has been requested!");
                        break;
                    }

                    // retrieve the next message from the queue
                    msg = m_messages.front();
                    m_messages.pop();
                }

                // print the next message:
                Logger::log(std::cout, "Message: ", msg);
            }

            Logger::log(std::cout, "Leaving JThread");
        };

        Logger::log(std::cout, "Pushing strings into queue ...");

        // store three messages
        for (const auto& s : { "Tic" , "Tac", "Toe" }) {

            std::lock_guard guard{ m_mutex };
            m_messages.push(s);
        }

        Logger::log(std::cout, "Starting JThread");

        std::jthread t{ task };

        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        {
            // after some time, store another message
            std::lock_guard guard{ m_mutex };
            m_messages.push("Tic-Tac-Toe Done");
        }

        // notify waiting thread
        m_condition_variable.notify_one();

        // after some time, end program (requests stop, which interrupts wait())
        std::this_thread::sleep_for(std::chrono::seconds{ 5 });

        Logger::log(std::cout, "Main Thread: calling request_stop");

        t.request_stop();

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        Logger::log(std::cout, "Leaving Main");
    }
}

// =============================================================================
// =============================================================================

namespace Using_Stop_Callbacks
{
    static void task(std::stop_token token, int num)
    {
        Logger::log(std::cout, "Enter Task");

        auto id{ std::this_thread::get_id() };

        // register a stop callback
        std::stop_callback cb{
            token,
            [=] {
                auto currentId{ std::this_thread::get_id() };

                if (currentId == id) {
                    Logger::log(std::cout, "Task: Stop requested - Thread Context = Task");
                }
                else {
                    Logger::log(std::cout, "Task: Stop requested - Thread Context = Main");
                }
            }
        };

        std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        Logger::log(std::cout, "Done Task");
    }

    static void test_02()
    {
        Logger::log(std::cout, "Main");

        // create stop source and stop token
        std::stop_source source;
        std::stop_token token{ source.get_token() };

        // A) request stop before task has been created
        source.request_stop();                   // put either this line into comment ...

        std::future<void> future{
            std::async(std::launch::async, [token] { task(token, 123); })
        };

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });

        // B) request stop after task has been created
        // (runs any associated callbacks on this thread)
        // source.request_stop();                  // or put this line into comment

        Logger::log(std::cout, "Done Main");
    }
}

void test_condition_variable_any()
{
    Stop_Tokens_and_Condition_Variables::test_01();
}

void test_stop_callback()
{
    Using_Stop_Callbacks::test_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
