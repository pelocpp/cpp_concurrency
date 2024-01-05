#include <iostream>
#include <queue>
#include <thread>
#include <stop_token>
#include <mutex>
#include <condition_variable>

using namespace std::literals; //

namespace Concurrency_Stop_Tokens_and_Condition_Variables
{
    static void test()
    {
        std::queue<std::string>     m_messages;
        std::mutex                  m_mutex;
        std::condition_variable_any m_condition_variable;

        auto task = [&] (std::stop_token st) {
            
            while (!st.stop_requested()) {

                std::string msg{};

                {
                    std::unique_lock lock{ m_mutex };

                    // wait for the next message
                    bool stopRequested {
                        m_condition_variable.wait(
                            lock,
                            st,
                            [&] () { return ! m_messages.empty(); }
                        )
                    };

                    if (!stopRequested) {
                        return;
                    }

                    // retrieve the next message from the queue
                    msg = m_messages.front();
                    m_messages.pop();
                }

                // print the next message:
                std::cout << "msg: " << msg << std::endl;
            }
        };

        std::jthread t1{ task };

        // store 3 messages and notify one waiting thread each time:
        for (std::string s : {"Tic", "Tac", "Toe"}) {
            std::scoped_lock lg{ m_mutex };
            m_messages.push(s);
            m_condition_variable.notify_one();
        }

        // after some time
        // - store 1 message and notify all waiting threads:
        std::this_thread::sleep_for(1s);
        {
            std::scoped_lock lg{ m_mutex };
            m_messages.push("done");
            m_condition_variable.notify_all();
        }
        // after some time
        // - end program (requests stop, which interrupts wait())
        std::this_thread::sleep_for(1s);
    }
}

void test_Stop_Tokens_and_Condition_Variables()
{
    Concurrency_Stop_Tokens_and_Condition_Variables::test();
}
