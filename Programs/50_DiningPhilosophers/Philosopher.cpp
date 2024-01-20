// ===========================================================================
// class Philosopher
// ===========================================================================

#include <random>
#include <array>
#include <future>
#include "../Logger/Logger.h"
#include "PhilosopherState.h"
#include "Table.h"
#include "Philosopher.h"

Philosopher::Philosopher(Table& table, int seat)
    : m_table{ table }, m_seat{ seat }, m_activities{},
      m_state{ PhilosopherState::None }, m_running{ false } {}

void Philosopher::start()
{
    m_running = true;
    m_future = std::async(
        std::launch::async,
        [this]() {
            run();
        }
    );
}

void Philosopher::stop()
{
    m_running = false;
    m_future.get();

    Logger::logAbs(std::cout, "philosopher ", m_seat, " done: ", m_activities, " activities completed.");
}

void Philosopher::thinking()
{
    m_activities++;
    Logger::log(std::cout, "thinking at seat ", m_seat);
    m_state = PhilosopherState::Thinking;
    int milliSecondsSleeping = distribution(generator);
    std::this_thread::sleep_for(std::chrono::milliseconds{ milliSecondsSleeping });
}

void Philosopher::hungry() 
{
    m_activities++;
    Logger::log(std::cout, "hungry at seat ", m_seat);
    m_state = PhilosopherState::Hungry;
    m_table.demandForks(m_seat);
}

void Philosopher::eating()
{
    m_activities++;
    Logger::log(std::cout, "eating at seat ", m_seat);
    m_state = PhilosopherState::Eating;
    int milliSecondsEating{ distribution(generator) };
    std::this_thread::sleep_for(std::chrono::milliseconds{ milliSecondsEating });
}

void Philosopher::eatingDone() 
{
    m_activities++;
    Logger::log(std::cout, "eating done at seat ", m_seat);
    m_table.releaseForks(m_seat);
}

void Philosopher::run()
{
    std::thread::id philosopherThreadId{ std::this_thread::get_id() };
    Logger::logAbs(std::cout, "philosopher enters room");

    while (m_running) {
        thinking();
        hungry();
        eating();
        eatingDone();
    }

    Logger::logAbs(std::cout, "philosopher at seat ", m_seat, " quits.");
    m_state = PhilosopherState::None;
}

// initialize static class members
std::random_device Philosopher::device;
std::mt19937 Philosopher::generator(Philosopher::device());
std::uniform_int_distribution<int> Philosopher::distribution{ 
    MinSleepingMSecs, MaxSleepingMSecs
};

// ===========================================================================
// End-of-File
// ===========================================================================
