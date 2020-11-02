// ===========================================================================
// class Philosopher
// ===========================================================================

#include <random>
#include <array>
#include "../Logger/Logger.h"
#include "PhilosopherState.h"
#include "Table.h"
#include "Philosopher.h"

Philosopher::Philosopher(Table& table, int seat) : m_table(table), m_seat(seat)
{
    m_state = PhilosopherState::None;
    m_running = false;
}

void Philosopher::Start()
{
    m_running = true;
    m_thread = std::thread([this] { Run(); });
}

void Philosopher::Stop() 
{
    m_running = false;
}

void Philosopher::Detach()
{
    m_thread.detach();
}

void Philosopher::Thinking()
{
    Logger::log(std::cout, "thinking at seat ", std::to_string(m_seat));
    m_state = PhilosopherState::Thinking;
    int milliSecondsSleeping = distribution(generator);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliSecondsSleeping));
}

void Philosopher::Hungry() 
{
    Logger::log(std::cout, "hungry at seat ", std::to_string(m_seat));
    m_state = PhilosopherState::Hungry;
    m_table.DemandForks(m_seat);
}

void Philosopher::Eating()
{
    Logger::log(std::cout, "eating at seat ", std::to_string(m_seat));
    m_state = PhilosopherState::Eating;
    int milliSecondsEating = distribution(generator);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliSecondsEating));
}

void Philosopher::EatingDone() 
{
    Logger::log(std::cout, "eating done at seat ", std::to_string(m_seat));
    m_table.ReleaseForks(m_seat);
}

void Philosopher::Run()
{
    std::thread::id philosopherThreadId = std::this_thread::get_id();
    Logger::logAbs(std::cout, "philosopher enters room");

    while (m_running) {
        Thinking();
        Hungry();
        Eating();
        EatingDone();
    }

    Logger::logAbs(std::cout, "philosopher at seat ", std::to_string(m_seat), " quits.");
    m_state = PhilosopherState::None;
}

// initialize static class members
std::random_device Philosopher::device;
std::mt19937 Philosopher::generator(Philosopher::device());
std::uniform_int_distribution<int> Philosopher::distribution(MinSleepingMSecs, MaxSleepingMSecs);

// ===========================================================================
// End-of-File
// ===========================================================================
