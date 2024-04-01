// ===========================================================================
// Program.cpp // Thread Pool
// ===========================================================================

// Nachteil:  
// Die Threads warten mit yield auf einen nächsten Job
// ==> das ist eine Busy-Loop

void test_concurrency_thread_pool_01();

int main()
{
    test_concurrency_thread_pool_01();
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
