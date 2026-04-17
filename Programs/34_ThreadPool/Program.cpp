// ===========================================================================
// Program.cpp - Thread Pool
// ===========================================================================

extern void test_concurrency_thread_pool01();   // just launching ... and stopping the thread pool
extern void test_concurrency_thread_pool02();   // launching 5 almost empty tasks
extern void test_concurrency_thread_pool03();   // launching many tasks ... and working on the same global variable (by address)
extern void test_concurrency_thread_pool04();   // launching many tasks ... and working on an atomic variable (by address)
extern void test_concurrency_thread_pool05();   // launching many tasks ... and working on an atomic variable (by reference)
extern void test_concurrency_thread_pool10();   // computing prime numbers, using free function
extern void test_concurrency_thread_pool11();   // computing prime numbers, using lambda
extern void test_concurrency_thread_pool12();   // computing prime numbers, using lambda

int main()
{
    test_concurrency_thread_pool01();  // just launching ... and stopping the thread pool
    test_concurrency_thread_pool02();  // launching 5 almost empty tasks

    test_concurrency_thread_pool03();  // launching many tasks ... and working on the same global variable (by address)
    test_concurrency_thread_pool04();  // launching many tasks ... and working on an atomic variable (by address)
    test_concurrency_thread_pool05();  // launching many tasks ... and working on an atomic variable (by reference)

    test_concurrency_thread_pool10();   // computing prime numbers (^minimum variant)
    test_concurrency_thread_pool11();   // computing prime numbers (printing values to the console) 
    test_concurrency_thread_pool12();   // computing prime numbers (returning computed values with future objects)

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
