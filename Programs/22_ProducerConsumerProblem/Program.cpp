// ===========================================================================
// Program.cpp // Producer Consumer Problem
// ===========================================================================

void test_thread_safe_blocking_queue_01();   // just testing single push and pop
void test_thread_safe_blocking_queue_02();   // testing limited number of push and pop operations
void test_thread_safe_blocking_queue_03();   // passing BlockingQueue to separate consumer and producer objects
void test_thread_safe_blocking_queue_04();   // testing BlockingQueue with 6 threads
void test_thread_safe_blocking_queue_05();   // testing BlockingQueue with real objects
void test_thread_safe_blocking_queue_06();   // testing BlockingQueue with 6 threads and Person objects

void test_producer_consumer_problem()
{
    test_thread_safe_blocking_queue_01();   // just testing single push and pop
    test_thread_safe_blocking_queue_02();   // testing limited number of push and pop operations
    test_thread_safe_blocking_queue_03();   // passing BlockingQueue to separate consumer and producer objects
    test_thread_safe_blocking_queue_04();   // testing BlockingQueue with 6 threads
    test_thread_safe_blocking_queue_05();   // testing BlockingQueue with real objects
    test_thread_safe_blocking_queue_06();   // testing BlockingQueue with 6 threads and Person objects
}

int main()
{
    test_producer_consumer_problem();
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
