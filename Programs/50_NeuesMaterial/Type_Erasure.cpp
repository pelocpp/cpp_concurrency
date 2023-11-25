#include <iostream>
#include <cassert>
#include <chrono>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <deque>
#include <type_traits>
#include <memory>


namespace Type_Erasure
{
    class TypeErasure
    {
    };

    //void function()
    //{
    //    // using ResultType = std::invoke_result_t<std::decay_t<F>>;

    //    using UniqueFunction = std::packaged_task<void()>;

    //    int n;

    //    std::packaged_task<int()> pt1;

    //    std::future<int> future = pt1.get_future();

    //    // does not compile
    //    //auto lambda1 = [=]() mutable {
    //    //    pt1();
    //    //};

    //    auto lambda2 = [n]() mutable { n = 123; };

    //    int i = 0;
    //    int sum = 0;

    //    auto lambda3 = [i, &sum]() {
    //        sum += i;
    //        return i;
    //        };

    //    std::packaged_task<void()> task{
    //        [p = std::move(lambda3)]() mutable { p(); }
    //    };
    //}

    //// 2. Versuch
    //void function02()
    //{
    //    // Signature: int()
    //    auto l = []() -> int { return 123; };

    //    auto l2 = [value = 123]() -> int { return value; };

    //    std::function<int()> ff{ l2 };

    //    std::packaged_task<int()> pt(std::move(ff));

    //    std::packaged_task<int()> pt2(ff);

    //    std::packaged_task<int()> copy (std::move(pt2));

    //    std::future<int> future (pt.get_future());

    //    // std::queue<function_wrapper> m_queue;
    //    std::queue<std::packaged_task<int()>> m_workQueue;

    //    m_workQueue.push(std::move(pt));
    //}


    //// 3. Versuch

    //template<typename FunctionType>
    //using result_type = typename std::invoke_result<FunctionType>::type;

    //template<typename FunctionType>
    //std::future<result_type<FunctionType>>
    //submit(FunctionType f)
    //{
    //    std::packaged_task<result_type<FunctionType>()> task(std::move(f));

    //    std::future<result_type<FunctionType>> fut(task.get_future());

    //    // m_workQueue.push(std::move(task));
    //    std::queue<std::packaged_task<int()>> m_workQueue;
    //    
    //    m_workQueue.push(std::move(task));

    //    return fut;
    //}

    //void function03()
    //{
    //    // Signature: int()
    //    auto f = []() -> int { return 123; };

    //    submit(f);
    //}

        // 4. Versuch
    void function04()
    {
        // queues
        std::deque<std::function<int()>> q1;
        std::deque<std::packaged_task<int()>> q2;

        // Lamdbas // Signature: int()
        auto l1 = []() -> int { return 123; };
        auto l2 = [value = 123]() -> int { return value; };
        auto l3 = [value = 123]() -> int { 
            std::unique_ptr<int> up = std::make_unique<int>(value);
            return value;
        };

        // std::function
        std::function<int()> f1{ l1 };
        std::function<int()> f2{ l2 };
        std::function<int()> f3{ l3 };

        q1.push_back(f1);
        q1.push_back(f2);
        q1.push_back(f3);

        // std::packaged_task
        std::packaged_task<int()> pt1(f1);
        std::packaged_task<int()> pt2(f2);
        std::packaged_task<int()> pt3(f3);

        // pt1 = pt2; // std::packaged_task is movable only !!! Error
        
        // q2.push_back(pt1);             // ERROR
        q2.push_back(std::move(pt1));
        q2.push_back(std::move(pt2));

        q2.emplace_back(std::packaged_task<int()>{ []() -> int { return 123; } });  // geht

        q2.emplace_back(std::packaged_task<int()>{ [value = 123]() -> int {      // geht
            std::unique_ptr<int> up = std::make_unique<int>(value);
            return value;
            }
        });  // geht

        // std::packaged_task<int()> result = q2.back();    // ERROR
        std::packaged_task<int()> result = std::move(q2.back()); // GEHT

    }
}


namespace Type_Erasure_Fedor_Pikus
{
    // https://stackoverflow.com/questions/73676042/confused-about-type-erasure-in-hands-on-design-patterns-with-c-by-fedor-g-pik


    // https://www.youtube.com/c/EverythingCpp

    template <typename T>
    class smartptr {
    private:
        struct deleter_base {
            virtual void apply(T*) = 0;
            virtual ~deleter_base() {}
        };

        template <typename Deleter>
        struct deleter : public deleter_base {

            deleter(Deleter d) : d_(d) {}

            virtual void apply(T* p) { d_(p); }
            Deleter d_;
        };

    public:
        template <typename Deleter>
        smartptr(T* p, Deleter d)
            : p_(p),
              d_(new deleter<Deleter>(d))
        {}

        ~smartptr() {
            d_->apply(p_);
            delete d_;
        }

        T* operator->() { return p_; }
        const T* operator->() const { return p_; }

    private:
        T* p_;
        deleter_base* d_;
    };

    //template<>
    //class std::default_delete<int>
    //{
    //public:
    //    void operator()(int* ptr) const
    //    {
    //        delete ptr;
    //    }
    //};

    class MyDeleter
    {
    public:
        MyDeleter() {
            std::cout << "MyDeleter()" << std::endl;
        }

        ~MyDeleter() {
            std::cout << "~MyDeleter()" << std::endl;
        }

        MyDeleter(const MyDeleter& other) {
            std::cout << "Copy c'tor" << std::endl;
        }

        MyDeleter& operator= (const MyDeleter& other) {
            std::cout << "operator=" << std::endl;
            return *this;
        }

        void operator()(int* ptr) const
        {
            delete ptr;
        }
    };
}



// 1. Versuch
void function01_fedor()
{
    using namespace Type_Erasure_Fedor_Pikus;

    smartptr<int> sp1(new int(123), std::default_delete<int>());

    smartptr<int> sp2(new int(456), MyDeleter{});

    //const int* ip = new int(123);

    //smartptr<int> sp(ip);
}



void test_type_erasure()
{
    using namespace Type_Erasure;

    function04();
    //function01_fedor();
}