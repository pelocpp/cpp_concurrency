// ===========================================================================
// Program.cpp // Mutual Exclusion // Locking Mechanisms
// ===========================================================================

extern void examples();
extern void examples_recursive_mutex();
extern void examples_locking_strategies();
extern void examples_scoped_lock();

int main()
{
    examples();
    examples_recursive_mutex();
    examples_locking_strategies();
    examples_scoped_lock();

    return 1;
}

// ===========================================================================
// End-of-File
// ===========================================================================
