// von Anthony Williams

#include <iostream>
#include <exception>
#include <stack>
#include <mutex>
#include <memory>
#include <future>

namespace Concurrency_Threadsafe_Stack
{
    struct empty_stack : std::exception
    {
        const char* what() const throw()
        {
            return "empty stack";
        }
    };

    template<typename T>
    class threadsafe_stack
    {
    private:
        std::stack<T> data;
        mutable std::mutex m;  // beachte mutable
    public:
        threadsafe_stack() {}

        // von Grimm

        threadsafe_stack(const threadsafe_stack&) = delete;
        threadsafe_stack& operator = (const threadsafe_stack&) = delete;

        //threadsafe_stack(const threadsafe_stack& other)
        //{
        //    std::lock_guard<std::mutex> lock(other.m);
        //    data = other.data;
        //}

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock(m);
            data.push(new_value);
        }

        std::shared_ptr<T> pop()
        {
            std::lock_guard<std::mutex> lock(m);
            if (data.empty()) throw empty_stack();
            std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
            data.pop();
            return res;
        }

        // von Grimm
        // Note: Returning a special non-value or returning a std::optional is also a valid option.

        T topAndPop() {
            std::lock_guard<std::mutex> lockStack(m);
            if (data.empty()) throw std::out_of_range("The stack is empty!");
            auto val = data.top();
            data.pop();
            return val;
        }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> lock(m);
            if (data.empty()) throw empty_stack();
            value = data.top();
            data.pop();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
    };
}

void test_thread_safe_stack_01_from_grimm()
{
    using namespace Concurrency_Threadsafe_Stack;

    std::cout << "Threadsafe Stack: " << std::this_thread::get_id() << std::endl;

    threadsafe_stack<int> conStack;

     auto fut = std::async(std::launch::async, [&conStack] { conStack.push(1); });
     auto fut1 = std::async(std::launch::async, [&conStack] { conStack.push(2); });
     auto fut2 = std::async(std::launch::async, [&conStack] { conStack.push(3); });

     auto fut3 = std::async(std::launch::async, [&conStack] { return conStack.topAndPop(); });
     auto fut4 = std::async(std::launch::async, [&conStack] { return conStack.topAndPop(); });
     auto fut5 = std::async(std::launch::async, [&conStack] { return conStack.topAndPop(); });

     fut.get(), fut1.get(), fut2.get();

     std::cout << fut3.get() << '\n';
     std::cout << fut4.get() << '\n';
     std::cout << fut5.get() << '\n';
}

void test_thread_safe_stack_01()
{
    test_thread_safe_stack_01_from_grimm();
}