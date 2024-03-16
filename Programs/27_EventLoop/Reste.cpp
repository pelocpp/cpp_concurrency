// ===========================================================================
// Program.cpp // Event Loop
// ===========================================================================

#if 0

// VORSICHT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// https://floating.io/2017/07/lambda-shared_ptr-memory-leak/

template<typename TFunc, typename... TArgs>
auto enqueueSync(TFunc&& callable, TArgs&& ...args)
{
    // The first if - condition is a protection from a deadlock.
    // Sometimes you may discover a situation when some synchronous task is trying to schedule another synchronous task,
    // leading to a deadlock.

    if (std::this_thread::get_id() == m_thread.get_id())
    {
        return std::invoke(
            std::forward<TFunc>(callable),
            std::forward<TArgs>(args)...);
    }

    // using library function
    // using ReturnType = std::invoke_result<TFunc, TArgs...>::type;

    // using self written function
    using ReturnType = decltype(std::declval<TFunc>() (std::declval<TArgs>()...));

    using PackagedTaskType = std::packaged_task<ReturnType(TArgs&&...)>;

    PackagedTaskType task{ std::forward<TFunc>(callable) };

    std::future<ReturnType> future = task.get_future();

    enqueue([&]() { task(std::forward<TArgs>(args) ...); });

    //Event event{ [&]() { task(std::forward<TArgs>(args) ...); } };
    //enqueue(event);

    ReturnType result = future.get();

    return result;
}

template<typename TFunc, typename... TArgs>
auto enqueueAsync(TFunc&& callable, TArgs&& ...args)
{
    // using library function
    // using ReturnType = std::invoke_result<TFunc, TArgs...>::type;

    // using self written function
    using ReturnType = decltype(std::declval<TFunc>() (std::declval<TArgs>()...));

    using PackagedTaskType = std::packaged_task<ReturnType(TArgs&&...)>;

    PackagedTaskType task{ std::forward<TFunc>(callable) };

    std::future<ReturnType> future = task.get_future();

    enqueue([&]() { task(std::forward<TArgs>(args) ...); });

    //Event event{ [&]() { task(std::forward<TArgs>(args) ...); } };
    //enqueue(event);

  //  ReturnType result = future.get();

    return future;
}

//template<typename Func, typename... Args>
//auto enqueueAsync(Func&& callable, Args&& ...args);

// ===========================================================================


//
//    // ============================================================
//
//    struct IBankAccount
//    {
//        virtual ~IBankAccount() = default;
//        virtual void pay(unsigned amount) noexcept = 0;
//        virtual void acquire(unsigned amount) noexcept = 0;
//        virtual long long balance() const noexcept = 0;
//    };
//
//    class ThreadUnsafeAccount : public IBankAccount
//    {
//    public:
//        ThreadUnsafeAccount(long long balance) : m_balance(balance)
//        {
//        }
//        void pay(unsigned amount) noexcept override
//        {
//            m_balance -= amount;
//        }
//        void acquire(unsigned amount) noexcept override
//        {
//            m_balance += amount;
//        }
//        long long balance() const noexcept override
//        {
//            return m_balance;
//        }
//    private:
//        long long m_balance;
//    };
//
//    class ThreadSafeAccount : public IBankAccount
//    {
//    private:
//        std::shared_ptr<EventLoop> m_eventLoop;
//        std::shared_ptr<IBankAccount> m_unknownBankAccount;
//
//    public:
//        ThreadSafeAccount(
//            std::shared_ptr<EventLoop> eventLoop,
//            std::shared_ptr<IBankAccount> unknownBankAccount) :
//            m_eventLoop(std::move(eventLoop)),
//            m_unknownBankAccount(std::move(unknownBankAccount))
//        {
//        }
//
//
//        // VORSICHT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//        // https://floating.io/2017/07/lambda-shared_ptr-memory-leak/
//
//        void pay(unsigned amount) noexcept override
//        {
//            //don't use this alternative because [=] or [&] captures this,
//            //but not std::shared_ptr.
//            m_eventLoop->enqueue([=]()
//                {
//                    std::cout << "enqueue / pay: " << m_unknownBankAccount.use_count() << std::endl;
//
//                    m_unknownBankAccount->pay(amount);
//                });
//
//            //use this alternative instead
//            //m_eventLoop->enqueue(std::bind(
//            //    &IBankAccount::pay, m_unknownBankAccount, amount));
//        }
//        void acquire(unsigned amount) noexcept override
//        {
//            //don't use this alternative because [=] or [&] captures this,
//            //but not std::shared_ptr.
//            m_eventLoop->enqueue([=]()
//                {
//                    std::cout << "enqueue / acquire: " << m_unknownBankAccount.use_count() << std::endl;
//
//                    m_unknownBankAccount->acquire(amount);
//                });
//
//            //m_eventLoop->enqueue(std::bind(
//            //    &IBankAccount::acquire, m_unknownBankAccount, amount));
//        }
//
//        long long balance() const noexcept override
//        {
//            //capturing via [&] is perfectly valid here
//            return m_eventLoop->enqueueSync([&]
//                {
//                    return m_unknownBankAccount->balance();
//                });
//
//            //or you can use this variant for consistency
//            //return m_eventLoop->enqueueSync(
//            //    &IBankAccount::balance, m_unknownBankAccount);
//        }
//
//    };
//
//
//}
//

//




//
//static void test_event_loop_05()
//{
//    using namespace Event_LoopLegacy;
//
//    EventLoop eventLoop;
//
//    std::future<int> result = eventLoop.enqueueAsync([](int x, int y)
//        {
//            return x + y;
//        }, 1, 2);
//
//    //
//    //do some heavy work here
//    //
//
//    std::cout << result.get();
//}
//
//static void test_event_loop_10()
//{
//    using namespace Event_LoopLegacy;
//
//    int constexpr Max = 10;
//
//    auto eventLoop = std::make_shared<EventLoop>();
//    auto bankAccount = std::make_shared<ThreadUnsafeAccount>(100'000);
//
//    std::thread buy = std::thread([](std::unique_ptr<IBankAccount> account)
//        {
//            for (int i = 1; i <= Max; ++i)
//            {
//                account->pay(i);
//            }
//        }, std::make_unique<ThreadSafeAccount>(eventLoop, bankAccount));
//
//    std::thread sell = std::thread([](std::unique_ptr<IBankAccount> account)
//        {
//            for (int i = 1; i <= Max; ++i)
//            {
//                account->acquire(i);
//            }
//        }, std::make_unique<ThreadSafeAccount>(eventLoop, bankAccount));
//
//    buy.join();
//    sell.join();
//
//    std::cout << bankAccount->balance() << '\n';
//}
//
//static void test_event_loop_11()
//{
//    int constexpr Max = 100000;
//
//
//    using namespace Event_LoopLegacy;
//    ThreadSafeAccount safeAccount(
//        std::make_shared<EventLoop>(),
//        std::make_shared<ThreadUnsafeAccount>(100'000));
//
//
//    std::thread buy = std::thread([&]()
//        {
//            for (int i = 1; i <= Max; ++i)
//            {
//                safeAccount.pay(i);
//            }
//        });
//
//    std::thread sell = std::thread([&]
//        {
//            for (int i = 1; i <= Max; ++i)
//            {
//                safeAccount.acquire(i);
//            }
//        });
//
//    buy.join();
//    sell.join();
//
//    std::cout << safeAccount.balance() << '\n';
//
//}
//
//
//namespace Event_Loop_XXX_LEGACY
//{
//    class MyClass {
//    private:
//        // using namespace std::placeholders;
//
//        //just shorthand to avoid long typing
//        typedef std::function<void(float result)> TCallback;
//
//        //this function takes long time
//        void longRunningFunction(TCallback callback)
//        {
//            //do some long running task
//            //...
//            //callback to return result
//
//            float result = 123;
//
//            callback(result);
//        }
//
//        //this function gets called by longRunningFunction after its done
//        void afterCompleteCallback(float result)
//        {
//            std::cout << result;
//        }
//
//    public:
//        int longRunningFunctionAsync()
//        {
//            //create callback - this equivalent of safe function pointer
//            auto callback = std::bind(&MyClass::afterCompleteCallback,
//                this, std::placeholders::_1);
//
//            //normally you want to start below function on seprate thread, 
//            //but for illustration we will just do simple call
//            longRunningFunction(callback);
//        }
//
//        int longRunningFunctionAsyncEx()
//        {
//            //create callback - this equivalent of safe function pointer
//            auto callback = std::bind(&MyClass::afterCompleteCallback,
//                this, std::placeholders::_1);
//
//
//            TCallback cb = std::bind(&MyClass::afterCompleteCallback, this, std::placeholders::_1);
//
//            //normally you want to start below function on seprate thread, 
//            //but for illustration we will just do simple call
//            longRunningFunction(cb);
//        }
//    };
//}
//
//
//namespace PitfallsLambdaCaptureInitialization_Legacy
//{
//    struct Job
//    {
//        template<class T>
//        Job(T&& func) : m_func(std::forward<T>(func)) {}
//
//        void run() {
//            m_func();
//            m_hasRun = true;
//        }
//
//        std::function<void()> m_func;
//
//        bool m_hasRun = false;
//    };
//
//    std::vector<Job> jobs;
//
//    template<class T>
//    void enqueueJob(T&& func)
//    {
//        jobs.emplace_back([func = std::forward<T>(func)]() mutable {
//            std::cout << "Starting job..." << std::endl;
//            // Move func to ensure that it is destroyed after running
//            auto fn = std::move(func);
//            fn();
//            std::cout << "Job finished." << std::endl;
//            });
//    }
//}
//
//static void test_pitfall_01()
//{
//    using namespace PitfallsLambdaCaptureInitialization_Legacy;
//
//    struct Data {};
//
//    std::weak_ptr<Data> observer;
//
//    {
//        const std::shared_ptr<Data> context = std::make_shared<Data>();
//
//        observer = context;
//
//        enqueueJob([context] {
//            std::cout << "Running..." << std::endl;
//            });
//    }
//
//    for (auto& job : jobs) {
//        job.run();
//    }
//
//
//    std::cout << "Observer.use_count(): " << observer.use_count() << std::endl;
//
//    //assert((observer.use_count() == 0)
//    //    && "There's still shared data left!");
//
//    std::cout << "Done." << std::endl;
//}
//
//static void test_pitfall_02()
//{
//    std::vector<int> vec{ 1, 2, 3 }; // note const
//
//    auto foo = [vec]() mutable {
//
//        // can't change vec here since it is captured with cv-qualifiers
//        // vec[0] = 123;
//        };
//
//    auto bar = [vec = std::move(vec)]() mutable {
//        // can change v here since it is captured by auto deduction rules
//        // (cv-qualifiers dropped)
//        vec[0] = 123;
//        };
//
//    bar();
//
//    std::cout << "Done." << std::endl;
//}
//
//static int g = 10;//global var 'g'
//
//
//static void test_pitfall_03()
//{
//    // https://stackoverflow.com/questions/68621493/why-capture-lambda-does-not-working-in-c
//
//    // creating lambda
//    auto kitty = [=]() {return g + 1; };
//
//    auto cat = [g = g]() {return g + 1; };
//
//    g = 20;//modifying global variable 'g'
//
//    std::cout << "kitty: " << kitty() << std::endl;
//    std::cout << "cat: " << cat() << std::endl;
//
//    std::cout << "Done." << std::endl;
//}
//
//
//static void test_pitfall_04()
//{
//    typedef void (*func)();
//
//    func myfunc = []() { std::cout << "did something" << std::endl; };
//
//    myfunc();
//}
//
//
//static void test_pitfall_05()
//{
//    int mynum = 123;
//
//    typedef std::function<void()> func;
//
//    func myfunc = [mynum]() { std::cout << "the answer is " << mynum << std::endl; };
//
//    myfunc();
//}
//
//class my_class {
//    // ...
//public:
//    typedef std::function<void()> callback;
//    void on_complete(callback cb) { complete_callback = cb; }
//
//    void clean_something_up() {}
//private:
//    callback complete_callback;
//    // ...
//};
//
//static void test_pitfall_06()
//{
//    std::shared_ptr<my_class> obj = std::make_shared<my_class>();
//
//    obj->on_complete([obj]() {
//        obj->clean_something_up();
//        });
//
//
//    std::cout << "Use_count: " << obj.use_count() << std::endl;
//
//    obj->clean_something_up();
//
//    // executor->submit(obj);
//    std::cout << "Doing something else ..." << std::endl;
//
//    // HIER HABEN WIR EIN MEMORY LEAK AM ENDE !!!!!!!!!!
//}
//
//
//// ---------------------------
//
//
////class my_class_02 {
////    // ...
////public:
////    typedef std::function<void()> callback;
////    void on_complete(callback cb) { complete_callback = cb; }
////
////    void clean_something_up() {}
////private:
////    callback complete_callback;
////    // ...
////};
//
//static void test_pitfall_07()
//{
//    std::shared_ptr<my_class> obj = std::make_shared<my_class>();
//
//    std::weak_ptr<my_class> weak_obj(obj);
//
//    obj->on_complete([weak_obj]() {
//
//        auto obj = weak_obj.lock();
//        if (obj) {
//            obj->clean_something_up();
//        }
//        });
//
//
//    std::cout << "Use_count: " << obj.use_count() << std::endl;
//
//    obj->clean_something_up();
//
//    // executor->submit(obj);
//    std::cout << "Doing something else ..." << std::endl;
//
//    // KEIN MEMORY LEAK AM ENDE !!!!!!!!!!
//}
//
//static void test_event_loop()
//{
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//    using namespace Event_LoopLegacy;
//
//    test_pitfall_01();
//}
//
==============================================================================


// ===========================================================================
// dealing with result of an event

static void test_event_loop_05()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    eventLoop.start();

    auto func = [](int x, int y, int z) -> int {
        Logger::log(std::cout, "func: adding ", x, ", ", y, " and ", z);
        return x + y + z;
        };

    auto result = eventLoop.enqueueSync(func, 1, 2, 3);
    Logger::log(std::cout, "Result: ", result);

    result = eventLoop.enqueueSync(func, 4, 5, 6);
    Logger::log(std::cout, "Result: ", result);

    Logger::log(std::cout, "Done.");
}

// testing non-trivial return type parameter

class X
{
public:
    std::string print() { return "I'm a X ;)"; };
};

static void test_event_loop_05_a()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    eventLoop.start();

    auto func = [](int x, int y, int z) {
        Logger::log(std::cout, "func: returning X{} ");
        return X{};
        };

    auto result = eventLoop.enqueueSync(func, 1, 2, 3);
    Logger::log(std::cout, "Result: ", result.print());

    result = eventLoop.enqueueSync(func, 4, 5, 6);
    Logger::log(std::cout, "Result: ", result.print());

    Logger::log(std::cout, "Done.");
}



// ===========================================================================
// first attempt to create an asyn version

// crashes ???????????????????????????????????????????????????????

static void test_event_loop_06()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    eventLoop.start();

    auto func = [](int x, int y, int z) -> int {
        Logger::log(std::cout, "func: adding ", x, ", ", y, " and ", z);
        return x + y + z;
        };

    auto result = eventLoop.enqueueAsync(func, 1, 2, 3);
    //    Logger::log(std::cout, "Result: ", result);

    auto result2 = eventLoop.enqueueAsync(func, 4, 5, 6);
    //    Logger::log(std::cout, "Result: ", result);


    result.get();
    result2.get();

    Logger::log(std::cout, "Done.");
}


// ========================================================


static void test_event_loop_07()
{
    Logger::log(std::cout, "Main");

    EventLoop eventLoop;

    auto func = [](int x, int y, int z) {
        Logger::log(std::cout, "func: adding", x, ", ", y, " and ", z);
        return x + y + z;
        };

    eventLoop.enqueue(function);

    auto result = eventLoop.enqueueSync(func, 1, 2, 3);

    eventLoop.enqueue(function);

    Logger::log(std::cout, "Result: ", result);
    Logger::log(std::cout, "Done.");
}

#endif

// ===========================================================================
// End-of-File
// ===========================================================================
