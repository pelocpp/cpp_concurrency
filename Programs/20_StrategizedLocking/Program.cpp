// ===========================================================================
// Program.cpp // Strategized Locking
// ===========================================================================

extern void test_strategized_locking_01();
extern void test_strategized_locking_02();
extern void test_strategized_locking_03();
extern void test_strategized_locking_04();

int main()
{
    test_strategized_locking_01();  // testing NoLock vs ExclusiveLock
                                    //
    test_strategized_locking_02();  // modify pop method of class ThreadsafeStack before testing
                                    //
    test_strategized_locking_03();  // searching prime numbers in a single thread // single-threading context // 
                                    // using NoLock should be preferred
    test_strategized_locking_04();  // searching prime numbers with serveral threads // multi-threading context // 
                                    // using ExclusiveLock mandatory
                                    // sporadically crashes can be observer when using NoLock
    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
