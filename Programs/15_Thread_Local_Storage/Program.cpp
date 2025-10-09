// ===========================================================================
// Program.cpp //  Thread Local Storage
// ===========================================================================

#include <print>
#include <thread>

namespace ThreadLocalStorage {
    thread_local int x{};
}

extern void test_thread_local_storage();

int main()
{
    using namespace ThreadLocalStorage;

    std::println("Main: {} ", std::this_thread::get_id());
    std::println("  &x: {:#010x} => {}", reinterpret_cast<intptr_t>(&x), x);

    test_thread_local_storage();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
