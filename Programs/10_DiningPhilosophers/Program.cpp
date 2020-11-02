// ===========================================================================
// Dining Philosophers Program
// ===========================================================================

#include <random>
#include <array>
#include "../Logger/Logger.h"
#include "PhilosopherState.h"
#include "Table.h"
#include "Philosopher.h"

const int SimulationDuration = 3000;   // total time of simulation (milliseonds)

void testDiningPhilosophers()
{
    Logger::enableLogging(true);

    std::thread::id mainThreadId = std::this_thread::get_id();
    size_t mainTid = Logger::readableTID(mainThreadId);
    Logger::log(std::cout, 
        "Dining Philosophers Simulation [TID=",
        std::to_string(Logger::readableTID(mainThreadId)),
        "]"
    );

    Logger::enableLogging(false);

    Table table;

    Logger::logAbs(std::cout, table.forksToString());

    Philosopher p1(table, 0);
    Philosopher p2(table, 1);
    Philosopher p3(table, 2);
    Philosopher p4(table, 3);
    Philosopher p5(table, 4);

    p1.Start();
    p2.Start();
    p3.Start();
    p4.Start();
    p5.Start();

    p1.Detach();
    p2.Detach();
    p3.Detach();
    p4.Detach();
    p5.Detach();

    // set duration time of this simulation
    std::this_thread::sleep_for(std::chrono::milliseconds(SimulationDuration));

    // terminate simulation
    p1.Stop();
    p2.Stop();
    p3.Stop();
    p4.Stop();
    p5.Stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(Table::NumPhilosophers * Philosopher::MaxSleepingMSecs));
    Logger::logAbs(std::cout, "Done.");
}

int main()
{
    testDiningPhilosophers();
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
