// ===========================================================================
// JThread // JThread_01.cpp
// ===========================================================================

#include <iostream>
#include <thread>
#include <format>

namespace JoinableThread {

    void jthread_01()
    {
        std::thread t {
            []() { std::cout << "Inside std::thread" << std::endl; }
        };

        bool j{ t.joinable() };
        std::string msg{ std::format("std::thread: joinable={0}\n", j) };
        std::cout << msg;

        t.join();  // <== put into comments -- must not be missing
    }

    void jthread_02()
    {
        std::jthread t {
            []() { std::cout << "Inside std::jthread" << std::endl; }
        };

        bool j{ t.joinable() };
        std::string msg{ std::format("std::jthread: joinable={0}\n", j) };
        std::cout << msg;

        t.join();  // <== put into comments -- may be missing
    }
}

// =================================================================

void test_jthread_01()
{
    using namespace JoinableThread;

    jthread_01();
    jthread_02();
}

// ===========================================================================
// End-of-File
// ===========================================================================
