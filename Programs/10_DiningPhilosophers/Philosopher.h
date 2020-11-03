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
    Table& m_table;
    int m_seat;
    PhilosopherState m_state;
    int m_activities;

    // random utilities
    static std::random_device device;
    static std::mt19937 generator;
    static std::uniform_int_distribution<int> distribution;

    // threading utils
    std::future<void> m_future;
    bool m_running;

public:
    // c'tor
    Philosopher(Table& table, int seat);

    // public interface
    void start();
    void stop();

private:
    // private helper methods
    void run();
    void thinking();
    void hungry();
    void eating();
    void eatingDone();
};

// ===========================================================================
// End-of-File
// ===========================================================================
