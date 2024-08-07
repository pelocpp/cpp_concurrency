// ===========================================================================
// Program.cpp
// ===========================================================================

extern void test_thread_safe_stack_01();
extern void test_thread_safe_stack_02();
extern void test_thread_safe_stack_03();
extern void test_thread_safe_stack_04();

int main()
{
    test_thread_safe_stack_01();  // just testing pop variants
    test_thread_safe_stack_02();  // testing primes calculator with one thread
    test_thread_safe_stack_03();  // testing primes calculator with several threads
    test_thread_safe_stack_04();  // testing primes calculator with simple 'Intercepting Filter Pattern'
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
