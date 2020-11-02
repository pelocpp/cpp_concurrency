// ===========================================================================
// class Table
// ===========================================================================

#pragma once

class Table {
public:
    static const int NumPhilosophers = 5;  // number of dining philosophers

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;

    // bool m_forks[NumPhilosophers];
    std::array<bool, NumPhilosophers> m_forks;

public:
    Table();

    // pubic interface
    void DemandForks(int seat);
    void ReleaseForks(int seat);

    // test interface
    std::string forksToString() const;

    // index operator
    bool& operator[] (int index);
};

// ===========================================================================
// End-of-File
// ===========================================================================
