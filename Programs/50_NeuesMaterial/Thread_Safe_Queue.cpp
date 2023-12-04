#include <algorithm>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <chrono>
#include <functional>

// https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue/comment-page-1/

namespace Concurrency_Threadsafe_Queue
{
    class non_empty_queue : public std::exception {
        std::string what_;
    public:
        explicit non_empty_queue(std::string msg) { what_ = std::move(msg); }
        const char* what() const noexcept override { return what_.c_str(); }
    };

    // using namespace std::chrono_literals;


    template<typename T>
    class ThreadsafeQueue {


        std::queue<T> queue_;
        mutable std::mutex mutex_;

        // Moved out of public interface to prevent races between this
        // and pop().
        [[nodiscard]] bool empty() const {
            return queue_.empty();
        }

    public:
        ThreadsafeQueue() = default;
        ThreadsafeQueue(const ThreadsafeQueue<T>&) = delete;
        ThreadsafeQueue& operator=(const ThreadsafeQueue<T>&) = delete;

        ThreadsafeQueue(ThreadsafeQueue<T>&& other) noexcept(false) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!empty()) {
                throw non_empty_queue("Moving into a non-empty queue");
            }
            queue_ = std::move(other.queue_);
        }

        virtual ~ThreadsafeQueue() noexcept(false) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!empty()) {
                throw non_empty_queue("Destroying a non-empty queue");
            }
        }

        [[nodiscard]] size_t size() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

        std::optional<T> pop() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty()) {
                return {};
            }
            T tmp = queue_.front();
            queue_.pop();
            return tmp;
        }

        void push(const T& item) {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(item);
        }
    };


    // =======================================================

    // TEST FRAME


    void FillQueue(int from, int to, ThreadsafeQueue<int>& q) {

        using namespace std::chrono_literals;

        auto start = std::chrono::system_clock::now();
        for (int i = from; i < to; ++i) {
            q.push(i);
            std::this_thread::sleep_for(10us);
        }
        auto runtime = std::chrono::system_clock::now() - start;
        std::cout << "FillQueue thread took "
            << (std::chrono::duration_cast<std::chrono::microseconds>(runtime)).count()
            << " µsec\n";
    }

    // NOTE: `flags` is used only by ONE thread at a time; this is not
    //   where the problem is.
    std::vector<bool> flags(30, false);

    void FlushQueue(ThreadsafeQueue<int>& q, int* count) {


        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100us);

        std::optional<int> jOpt = q.pop();
        while (jOpt) {
            int j = *jOpt;
            if (flags[j]) {
                std::cout << "We've already been here: " << j << std::endl;
                return;
            }
            flags[j] = true;
            (*count)++;
            jOpt = q.pop();
            if (!jOpt) {
                std::this_thread::sleep_for(1000us);
                jOpt = q.pop();
            }
        }
    }

}

void test_thread_safe_queue()
{
    using namespace Concurrency_Threadsafe_Queue;

    ThreadsafeQueue<int> q;
    int num_elems = 0;

    std::vector<std::thread> threads;
    threads.emplace_back(FillQueue, 0, 10, std::ref(q));
    threads.emplace_back(FillQueue, 10, 15, std::ref(q));
    threads.emplace_back(FillQueue, 15, 30, std::ref(q));

    std::thread flush(FlushQueue, std::ref(q), &num_elems);

    std::cout << "Threads started, waiting for them to complete...\n";
    flush.join();
    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    std::cout << "We processed " << num_elems << " elements" << std::endl;
    std::cout << "After running the threads the Q has " << q.size() << " elements left" << std::endl;

    for (auto f : flags) {
        if (!f) {
            std::cout << "ERROR: we missed one\n";
        }
    }
}


// ===================================================================

// Versus:

// Anthony Williams
// ccia_code_samples/listings/listing_4.5.cpp


#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;

public:
    threadsafe_queue()
    {}
    threadsafe_queue(threadsafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty)
            return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
