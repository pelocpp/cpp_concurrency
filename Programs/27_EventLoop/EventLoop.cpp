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

void EventLoop::threadProcedure()
{
    Logger::log(std::cout, "> Event Loop");

    std::vector<Event> events;

    while (m_running)
    {
        {
            std::unique_lock<std::mutex> guard(m_mutex);

            m_condition.wait(
                guard,
                [this] () -> bool { return ! m_events.empty(); }
            );

            std::swap(events, m_events);
        }

        Logger::log(std::cout, "swapped ", events.size(), " events ...");

        for (const Event& callable : events)
        {
            Logger::log(std::cout, "! invoking next event");
            callable();
        }

        events.clear();
    }

    Logger::log(std::cout, "< Event Loop");
}

void EventLoop::enqueue(const Event& callable)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_events.emplace_back(callable);
    }

    m_condition.notify_one();
}

void EventLoop::enqueue(Event&& callable) noexcept
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_events.emplace_back(std::move(callable));
    }

    m_condition.notify_one();
}

// ===========================================================================
// End-of-File
// ===========================================================================
