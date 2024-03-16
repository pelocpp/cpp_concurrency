// ===========================================================================
// EventLoop.h
// ===========================================================================

#pragma once

#include <iostream>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <mutex>

#include "../Logger/Logger.h"

class EventLoop
{
private:
    using Event = std::function<void()>;

private:
    std::vector<Event>      m_events;
    std::mutex              m_mutex;
    std::condition_variable m_condition;
    std::thread             m_thread;
    bool                    m_running;

public:
    // c'tor(s) / d'tor
    EventLoop();
    ~EventLoop();

    // prevent copy semantics
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator= (const EventLoop&) = delete;

    // prevent move semantics
    EventLoop(EventLoop&&) noexcept = delete;
    EventLoop& operator= (EventLoop&&) noexcept = delete;

    // public interface
    void enqueue(const Event& callable);
    void enqueue(Event&& callable) noexcept;

    template<typename TFunc, typename ... TArgs>
    void enqueueTask(TFunc&& callable, TArgs&& ...args)
    {
        Logger::log(std::cout, "enqueueTask ...");

        {
            std::lock_guard<std::mutex> guard(m_mutex);

            m_events.emplace_back([=]() mutable { 
                std::forward<TFunc>(callable) (std::forward<TArgs>(args)...);
                }
            );
        }

        m_condition.notify_one();
    }

    void start();
    void stop();

private:
    void threadProcedure();
};

// ===========================================================================
// End-of-File
// ===========================================================================
