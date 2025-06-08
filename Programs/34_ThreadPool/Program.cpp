// ===========================================================================
// Program.cpp // Thread Pool
// ===========================================================================

extern void test_concurrency_thread_pool_01();
extern void test_concurrency_thread_pool_02();
extern void test_concurrency_thread_pool_03();
extern void test_concurrency_thread_pool_04();
extern void test_concurrency_thread_pool_05();

int main()
{
    //test_concurrency_thread_pool_01();    // using implementation from Anthony Williams
    //test_concurrency_thread_pool_02();    // using implementation from Anthony Williams (improved version)
    //test_concurrency_thread_pool_03();    // using implementation from Arthur Dwyer
    //test_concurrency_thread_pool_04();    // using implementation from Martin Vorbrodt
    test_concurrency_thread_pool_05();      // using implementation from Zen Sepiol
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
