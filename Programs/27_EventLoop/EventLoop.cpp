// ===========================================================================
// EventLoop.cpp
// ===========================================================================

//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif
//#endif  // _DEBUG

//#include <iostream>
//#include <sstream>
//#include <condition_variable>
//#include <functional>
//#include <future>
//#include <thread>
//#include <vector>
//#include <thread>
//#include <memory>
//#include <cassert>

#include "EventLoop.h"

// default c'tor
EventLoop::EventLoop()
    : m_running{ true }, m_thread{ &EventLoop::threadFunc, this }
{}

// d'tor
EventLoop::~EventLoop()
{
    enqueue([this] { m_running = false; });
    m_thread.join();
}

void EventLoop::threadFunc()
{
    Logger::log(std::cout, "> Event Loop");

    std::vector<Event> events;

    while (m_running)
    {
        {
            std::unique_lock<std::mutex> guard(m_mutex);

            m_condition.wait(
                guard,
                [this] () -> bool { return !m_events.empty(); }
            );

            std::swap(events, m_events);
        }

        for (const Event& callable : events)
        {
            Logger::log(std::cout, "# invoking next event");
            callable();
        }

        events.clear();
    }

    Logger::log(std::cout, "< Event Loop");
}

void EventLoop::enqueue(Event&& callable) noexcept
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_events.emplace_back(std::move(callable));
    }

    m_condition.notify_one();
}

template<typename Func, typename... Args>
auto EventLoop::enqueueAsync(Func&& event, Args&& ...args)
{
    using ReturnType = std::invoke_result_t<Func, Args...>;

    using packaged_task_type = std::packaged_task<ReturnType()>;

    auto taskPtr = std::make_shared<packaged_task_type>(std::bind(
        std::forward<Func>(event), std::forward<Args>(args)...));

    enqueue(std::bind(&packaged_task_type::operator(), taskPtr));

    return taskPtr->get_future();
}

// ===========================================================================
// End-of-File
// ===========================================================================
