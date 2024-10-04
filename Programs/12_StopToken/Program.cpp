// ===========================================================================
// Program.cpp // std::condition_variable_any // request_stop
// ===========================================================================

extern void test_joinable_thread_cooperative_interruptibility();
extern void test_condition_variable_any();
extern void test_stop_callback();

int main()
{
    test_joinable_thread_cooperative_interruptibility();
    test_condition_variable_any();
    test_stop_callback();
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
