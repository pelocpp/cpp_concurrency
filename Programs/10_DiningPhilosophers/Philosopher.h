// ===========================================================================
// class Philosopher
// ===========================================================================

#pragma once

class Philosopher {
public:
    static const int MinSleepingMSecs = 10;       // sleeping between 1 and ...
    static const int MaxSleepingMSecs = 30;       // ... 3 seconds

private:
    // dining philosophers utilities
    Table& m_table;   // TODO: Shared Pointer oder sonst was ?!?!?
    int m_seat;
    PhilosopherState m_state;

    // random utilities
    static std::random_device device;
    static std::mt19937 generator;
    static std::uniform_int_distribution<int> distribution;

    // threading utils
    std::thread m_thread;
    bool m_running;

public:
    Philosopher(Table& table, int seat);

    void Start();
    void Detach();
    void Run();
    void Stop();

private:
    void Thinking();
    void Hungry();
    void Eating();
    void EatingDone();
};

// ===========================================================================
// End-of-File
// ===========================================================================
