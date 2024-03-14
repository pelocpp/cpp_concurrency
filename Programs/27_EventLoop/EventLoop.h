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
    std::vector<Event> m_events;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_thread;
    bool m_running;

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

    void start();

    template<typename Func, typename... TArgs>
    auto enqueueSync(Func&& callable, TArgs&& ...args)
        -> std::invoke_result<Func, TArgs...>::type      // ??????
    {
        // The first if - condition is a protection from a deadlock.
        // Sometimes you may discover a situation when some synchronous task is trying to schedule another synchronous task,
        // leading to a deadlock.

        if (std::this_thread::get_id() == m_thread.get_id())
        {
            return std::invoke(
                std::forward<Func>(callable),
                std::forward<TArgs>(args)...);
        }



        //// added in Offenbach
        //XXXX
        //using type = decltype(detail::INVOKE(std::declval<F>(), std::declval<Args>()...));



        using ReturnType = std::invoke_result<Func, TArgs...>::type;

        using PackagedTaskType = std::packaged_task<ReturnType(TArgs&&...)>;

        PackagedTaskType task{ std::forward<Func>(callable) };

        enqueue([&] () { task(std::forward<TArgs>(args) ...); });

        std::future<ReturnType> future = task.get_future();

        ReturnType result = future.get();

        return result;
    }

    template<typename Func, typename... Args>
    auto enqueueAsync(Func&& callable, Args&& ...args);

private:
    void threadProcedure();
};

// ===========================================================================
// End-of-File
// ===========================================================================
