// ===========================================================================
// Program.cpp // Event Loop
// ===========================================================================

// https://habr.com/en/articles/665730/

// https://www.arnorehn.de/blog/2023/09/27/pitfalls-of-lambda-capture-initialization/

// https://stackoverflow.com/questions/18143661/what-is-the-difference-between-packaged-task-and-async

// Extrem guter Artikel zu 
// Unterschied std::packaged_task und std::async
// auch mit einem Thread Pool Beispiel

// https://medium.com/@simontoth/daily-bit-e-of-c-std-packaged-task-b2d3273d97

// VORSICHT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// https://floating.io/2017/07/lambda-shared_ptr-memory-leak/


#include <iostream>

void test_event_loop();

int main()
{
    test_event_loop();

    return 0;
}

// ===========================================================================
// End-of-File
// ===========================================================================
