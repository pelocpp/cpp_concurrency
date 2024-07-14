// ===========================================================================
// Program.cpp // Mutual Exclusion // Locking Mechanisms
// ===========================================================================

extern void test_simple_mutex();
extern void test_advanced_mutex();

extern void test_simple_condition_variable_01();
extern void test_simple_condition_variable_02();

extern void example_recursive_mutex();
extern void example_locking_strategies();
extern void example_scoped_lock();

int main()
{
    test_simple_mutex();
    test_advanced_mutex();

    test_simple_condition_variable_01();
    test_simple_condition_variable_02();

    example_recursive_mutex();
    example_locking_strategies();
    example_scoped_lock();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
