// ===========================================================================
// class Table
// ===========================================================================

#pragma once

class Table {
public:
    static const int NumPhilosophers = 5;  // number of dining philosophers

private:
    std::array<bool, NumPhilosophers> m_forks;

    std::mutex m_mutex;
    std::condition_variable m_condition;

public:
    // c'tor
    Table();

    // public interface
    void demandForks(int seat);
    void releaseForks(int seat);

private:
    // private helper methods / operators
    std::string forksToString() const;
    bool& operator[] (int index);
};

// ===========================================================================
// End-of-File
// ===========================================================================
