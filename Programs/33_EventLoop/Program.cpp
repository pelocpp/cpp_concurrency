// ===========================================================================
// Program.cpp // Event Loop
// ===========================================================================

extern void test_event_loop_01();  // just starting and stopping event loop
extern void test_event_loop_02();  // demonstrating event loop with one event
extern void test_event_loop_03();  // demonstrating event loop with five events
extern void test_event_loop_04();  // demonstrating event loop with five lambdas
extern void test_event_loop_10();  // demonstrating enqueuing events with parameters
extern void test_event_loop_20();  // searching prime numbers: first enqueuing events, than starting calculations
extern void test_event_loop_21();  // searching prime numbers: enqueuing events while event loop is active

int main()
{
    test_event_loop_01();  // just starting and stopping event loop
    test_event_loop_02();  // demonstrating event loop with one event
    test_event_loop_03();  // demonstrating event loop with five events
    test_event_loop_04();  // demonstrating event loop with five lambdas

    test_event_loop_10();  // demonstrating enqueuing events with parameters

    test_event_loop_20();  // searching prime numbers: first enqueuing events, than starting calculations
    test_event_loop_21();  // searching prime numbers: enqueuing events while event loop is active

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
