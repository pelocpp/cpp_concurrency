// ===========================================================================
// Program.cpp // JThread
// ===========================================================================

extern void test_concurrency_parallel_accumulate();
extern void test_concurrency_thread_pool_01();
extern void test_concurrency_thread_pool_02();
extern void test_concurrency_thread_pool_03();
extern void test_type_erasure();
extern void test_event_loop();
extern void test_thread_safe_blocking_queue();
extern void test_thread_local_storage();

extern void test_Stop_Tokens_and_Condition_Variables();
extern void test_Latches();


// void test_project_euler_39();

extern void test_parallel_for();

int main()
{
    test_Latches();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
