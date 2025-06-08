// ===========================================================================
// Program.cpp
// ===========================================================================

extern void test_thread_safe_queue_01();
extern void test_thread_safe_queue_02();

int main()
{
    test_thread_safe_queue_01();  // just testing pop variants
    test_thread_safe_queue_02();  // testing concurrent access to a ThreadsafeQueue object

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
