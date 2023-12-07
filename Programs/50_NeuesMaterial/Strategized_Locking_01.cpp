////#include <iostream>
////#include <cassert>
////#include <chrono>
////#include <future>
////#include <mutex>
////#include <queue>
////#include <thread>
////#include <vector>
////#include <functional>
////#include <deque>
////#include <type_traits>
////#include <memory>
//
//#include <iostream>
//#include <mutex>
//#include <shared_mutex>
//
//namespace Strategized_Locking_Runtime_Version
//{
//    class ILock
//    { 
//    public:
//        // virtual destructor
//
//        virtual void lock() const = 0;
//        virtual void unlock() const = 0;
//    };
//
//    class StrategizedLocking {
//    private:
//        ILock& m_lock;                                 // (1)
//    public:
//        StrategizedLocking(ILock& l) : m_lock(l) {       // (2)
//            m_lock.lock();
//        }
//        ~StrategizedLocking() {                      // (3)
//            m_lock.unlock();
//        }
//    };
//
//    struct NullObjectMutex {
//        void lock() {}
//        void unlock() {}
//    };
//
//    class NoLock : public ILock {                    // (5)
//        void lock() const override {
//            std::cout << "NoLock::lock: " << '\n';
//            nullObjectMutex.lock();
//        }
//        void unlock() const override {
//            std::cout << "NoLock::unlock: " << '\n';
//            nullObjectMutex.unlock();
//        }
//        mutable NullObjectMutex nullObjectMutex;    // (10)
//    };
//
//    class ExclusiveLock : public ILock {             // (6)
//        void lock() const override {
//            std::cout << "    ExclusiveLock::lock: " << '\n';
//            mutex.lock();
//        }
//        void unlock() const override {
//            std::cout << "    ExclusiveLock::unlock: " << '\n';
//            mutex.unlock();
//        }
//        mutable std::mutex mutex;                   // (11)
//    };
//
//    class SharedLock : public ILock {                // (7)
//        void lock() const override {
//            std::cout << "        SharedLock::lock_shared: " << '\n';
//            sharedMutex.lock_shared();             // (8)
//        }
//        void unlock() const override {
//            std::cout << "        SharedLock::unlock_shared: " << '\n';
//            sharedMutex.unlock_shared();           // (9)
//        }
//        mutable std::shared_mutex sharedMutex;     // (12)
//    };
//
//}
//
//
//namespace Strategized_Locking_Compiletime_Version
//{
//    template <typename T>
//    concept BasicLockable = requires(T lo) {
//        lo.lock();
//        lo.unlock();
//    };
//
//    template <BasicLockable Lock>
//    class StrategizedLocking {
//        Lock& lock;
//    public:
//        StrategizedLocking(Lock& l) : lock(l) {
//            lock.lock();
//        }
//        ~StrategizedLocking() {
//            lock.unlock();
//        }
//    };
//
//
//    struct NullObjectMutex {
//        void lock() {}
//        void unlock() {}
//    };
//
//    class NoLock {
//    public:
//        void lock() const {
//            std::cout << "NoLock::lock: " << '\n';
//            nullObjectMutex.lock();
//        }
//        void unlock() const {
//            std::cout << "NoLock::unlock: " << '\n';
//            nullObjectMutex.lock();
//        }
//        mutable NullObjectMutex nullObjectMutex;
//    };
//
//    class ExclusiveLock {
//    public:
//        void lock() const {
//            std::cout << "    ExclusiveLock::lock: " << '\n';
//            mutex.lock();
//        }
//        void unlock() const {
//            std::cout << "    ExclusiveLock::unlock: " << '\n';
//            mutex.unlock();
//        }
//        mutable std::mutex mutex;
//    };
//
//    class SharedLock {
//    public:
//        void lock() const {
//            std::cout << "        SharedLock::lock_shared: " << '\n';
//            sharedMutex.lock_shared();
//        }
//        void unlock() const {
//            std::cout << "        SharedLock::unlock_shared: " << '\n';
//            sharedMutex.unlock_shared();
//        }
//        mutable std::shared_mutex sharedMutex;
//    };
//}
//
//// ======================================================================
//
//
//
//
//void test_strategized_locking_01()
//{
//    using namespace Strategized_Locking_Runtime_Version;
//
//    NoLock noLock;
//    StrategizedLocking stratLock1{ noLock };
//
//    {
//        ExclusiveLock exLock;
//        StrategizedLocking stratLock2{ exLock };
//        {
//            SharedLock sharLock;
//            StrategizedLocking startLock3{ sharLock };
//        }
//    }
//}
//
//
//void test_strategized_locking_02()
//{
//    using namespace Strategized_Locking_Compiletime_Version;
//
//    NoLock noLock;
//    StrategizedLocking<NoLock> stratLock1{ noLock };
//
//    {
//        ExclusiveLock exLock;
//        StrategizedLocking<ExclusiveLock> stratLock2{ exLock };
//        {
//            SharedLock sharLock;
//            StrategizedLocking<SharedLock> startLock3{ sharLock };
//        }
//    }
//
//}
