// ===========================================================================
// Program.cpp - Wait and Notify - std::condition_variable
// ===========================================================================

extern void test_condition_variable_01();
extern void test_condition_variable_02();

extern void test_atomic_variable_wait_notify_01();
extern void test_atomic_variable_wait_notify_02();

int main()
{
    test_condition_variable_01();
    test_condition_variable_02();

    test_atomic_variable_wait_notify_01();
    test_atomic_variable_wait_notify_02();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
