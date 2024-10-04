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
    static void test()
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

void test_condition_variable_any()
{
    Stop_Tokens_and_Condition_Variables::test();
}

// ===========================================================================
// End-of-File
// ===========================================================================
