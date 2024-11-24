// ===========================================================================
// Program.cpp // Spin Locks
// ===========================================================================

extern void test_using_spinlocks();
extern void test_using_atomics();
extern void test_using_mutex();

int main()
{
    test_using_spinlocks();
    test_using_atomics();
    test_using_mutex();
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
