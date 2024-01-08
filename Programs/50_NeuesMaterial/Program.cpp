// ===========================================================================
// Program.cpp // Semaphore
// ===========================================================================

extern void test_concurrency_parallel_accumulate();
extern void test_concurrency_thread_pool_01();
extern void test_concurrency_thread_pool_02();
extern void test_concurrency_thread_pool_03();
extern void test_type_erasure();
extern void test_event_loop();
extern void test_thread_safe_blocking_queue();
extern void test_thread_local_storage();
extern void test_barriers();

extern void test_semaphore_01();

int main()
{
    test_barriers();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
