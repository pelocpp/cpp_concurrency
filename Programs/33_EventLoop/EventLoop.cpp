// ===========================================================================
// EventLoop.cpp
// ===========================================================================

#include "EventLoop.h"

// default c'tor
EventLoop::EventLoop() : m_running{ false } {}

// d'tor
EventLoop::~EventLoop()
{
    stop();
}

void EventLoop::start()
{
    m_running = true;

    m_thread = std::jthread { &EventLoop::threadProcedure, this };
}

void EventLoop::stop()
{
    if (m_thread.joinable()) {

        enqueue([this] { m_running = false; });

        m_thread.join();
    }
}

void EventLoop::enqueue(Event callable)
{
    {
        std::lock_guard<std::mutex> guard{ m_mutex };

        m_events.push_back(std::move(callable));
    }

    m_condition.notify_one();
}

void EventLoop::threadProcedure()
{
    Logger::log(std::cout, "> Event Loop");

    std::vector<Event> events;

    while (true)
    {
        {
            std::unique_lock<std::mutex> guard{ m_mutex };

            m_condition.wait(
                guard,
                [this] () -> bool { return ! m_events.empty() || !m_running; }
            );

            if (!m_running && m_events.empty()) {
                Logger::log(std::cout, "< Event Loop");
                return;
            }

            std::swap(events, m_events);
        }

        Logger::log(std::cout, "swapped ", events.size(), " event(s) ...");

        for (auto& callable : events)
        {
            Logger::log(std::cout, "! invoking next event");
            callable();
        }

        events.clear();  // empty container for next loop
    }
}

// ===========================================================================
// End-of-File
// ===========================================================================
