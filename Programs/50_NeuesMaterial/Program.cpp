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
extern void test_thread_safe_queue_pelo_version();
extern void test_thread_safe_stack_pelo_version_02();
extern void test_thread_safe_stack();

int main()
{
    test_thread_safe_stack();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
