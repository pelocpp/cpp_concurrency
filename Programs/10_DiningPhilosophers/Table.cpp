// ===========================================================================
// class Table
// ===========================================================================

#include <cassert>
#include <array>
#include "../Logger/Logger.h"
#include "Table.h"

Table::Table() 
{
    for (size_t i = 0; i < NumPhilosophers; i++) {
        m_forks[i] = false;
    }
}

bool& Table::operator[] (int index)
{
    int seat = index % 5;
    return m_forks[seat];
}

void Table::DemandForks(int seat) 
{
    Logger::log(std::cout, "demand forks at seat ", std::to_string(seat));

    std::string forksDisplay;

    {
        // RAII idiom
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condition.wait(lock, [&] {
            return (*this)[seat] == false && (*this)[seat + 1] == false;
            }
        );

        // now we own the lock, take the corresponding forks 
        (*this)[seat] = true;
        (*this)[seat + 1] = true;

        Logger::log(std::cout, "got forks at seat ", std::to_string(seat));

        // create current display string of forks thread-safe
        forksDisplay = forksToString();
    }

    // output latest forks display string - not necessary within critical section
    Logger::logAbs(std::cout, forksDisplay);
}

void Table::ReleaseForks(int seat)
{
    Logger::log(std::cout, "release forks at seat ", std::to_string(seat));

    std::string forksDisplay;

    {
        // RAII idiom
        std::unique_lock<std::mutex> lock(m_mutex);

        // now we own the lock, release the corresponding forks 
        (*this)[seat] = false;
        (*this)[seat + 1] = false;

        Logger::log(std::cout, "released forks at seat ", std::to_string(seat));

        // create current display string of forks thread-safe
        forksDisplay = forksToString();

        // wakeup other waiting  philosophers (within locked region)
        m_condition.notify_all();
    }

    // output latest forks display string - not necessary within critical section
    Logger::logAbs(std::cout, forksDisplay);
}

std::string Table::forksToString() const
{
    std::string sforks;
    for (size_t i = 0; i < NumPhilosophers; i++) {
        sforks.append(m_forks[i] ? "X" : "_");
    }
    assert(sforks != "XXXXX");
    return sforks;
}

// ===========================================================================
// End-of-File
// ===========================================================================
