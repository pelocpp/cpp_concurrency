#include <iostream>
#include <sstream>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <thread>

// https://habr.com/en/articles/665730/

namespace Event_Loop
{
	class EventLoop
	{
	private:
		using Event = std::function<void()>;

	private:
		std::vector<Event> m_events;
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::thread m_thread;
		bool m_running;

	public:
		// default c'tor
		EventLoop()
			: m_running{ true } , m_thread{ &EventLoop::threadFunc, this }
		{}

		// d'tor
		~EventLoop()
		{
			enqueue([this] { m_running = false; });
			m_thread.join();
		}

		void enqueue(Event&& callable) noexcept
		{
			{
				std::lock_guard<std::mutex> guard(m_mutex);
				m_events.emplace_back(std::move(callable));
			}

			m_condition.notify_one();
		}

		template<typename Func, typename... Args>
		auto enqueueSync(Func&& callable, Args&& ...args)
		{
			if (std::this_thread::get_id() == m_thread.get_id())
			{
				return std::invoke(
					std::forward<Func>(callable),
					std::forward<Args>(args)...);
			}

			using return_type = std::invoke_result<Func, Args...>::type;

			using packaged_task_type = std::packaged_task<return_type(Args&&...)>;

			packaged_task_type task{ std::forward<Func>(callable) };

			enqueue([&] { task(std::forward<Args>(args)...); });

			return task.get_future().get();
		}

		template<typename Func, typename... Args>
		auto enqueueAsync(Func&& callable, Args&& ...args)
		{
			using return_type = std::invoke_result_t<Func, Args...>;

			using packaged_task_type = std::packaged_task<return_type()>;

			auto taskPtr = std::make_shared<packaged_task_type>(std::bind(
				std::forward<Func>(callable), std::forward<Args>(args)...));

			enqueue(std::bind(&packaged_task_type::operator(), taskPtr));

			return taskPtr->get_future();
		}

		void threadFunc()
		{
			std::vector<Event> events;

			while (m_running) 
			{
				{
					std::unique_lock<std::mutex> guard (m_mutex);

					m_condition.wait(
						guard,
						[this] () { return !m_events.empty(); }
					);
					
					std::swap(events, m_events);
				}

				for (const Event& func : events) {
					func();
				}

				events.clear();
			}
		}

		// prevent copy semantics
		EventLoop(const EventLoop&) = delete;
		EventLoop& operator= (const EventLoop&) = delete;

		// prevent move semantics
		EventLoop(EventLoop&&) noexcept = delete;
		EventLoop& operator= (EventLoop&&) noexcept = delete;

	};

	// ============================================================

	struct IBankAccount
	{
		virtual ~IBankAccount() = default;
		virtual void pay(unsigned amount) noexcept = 0;
		virtual void acquire(unsigned amount) noexcept = 0;
		virtual long long balance() const noexcept = 0;
	};

	class ThreadUnsafeAccount : public IBankAccount
	{
	public:
		ThreadUnsafeAccount(long long balance) : m_balance(balance)
		{
		}
		void pay(unsigned amount) noexcept override
		{
			m_balance -= amount;
		}
		void acquire(unsigned amount) noexcept override
		{
			m_balance += amount;
		}
		long long balance() const noexcept override
		{
			return m_balance;
		}
	private:
		long long m_balance;
	};

	class ThreadSafeAccount : public IBankAccount
	{
	private:
		std::shared_ptr<EventLoop> m_eventLoop;
		std::shared_ptr<IBankAccount> m_unknownBankAccount;

	public:
		ThreadSafeAccount(
			std::shared_ptr<EventLoop> eventLoop,
			std::shared_ptr<IBankAccount> unknownBankAccount) :
			m_eventLoop(std::move(eventLoop)),
			m_unknownBankAccount(std::move(unknownBankAccount))
		{
		}


		// VORSICHT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// https://floating.io/2017/07/lambda-shared_ptr-memory-leak/

		void pay(unsigned amount) noexcept override
		{
			//don't use this alternative because [=] or [&] captures this,
			//but not std::shared_ptr.
			m_eventLoop->enqueue([&]()
			{
				m_unknownBankAccount->pay(amount);
			});

			//use this alternative instead
			m_eventLoop->enqueue(std::bind(
				&IBankAccount::pay, m_unknownBankAccount, amount));
		}
		void acquire(unsigned amount) noexcept override
		{
			m_eventLoop->enqueue(std::bind(
				&IBankAccount::acquire, m_unknownBankAccount, amount));
		}

		long long balance() const noexcept override
		{
			//capturing via [&] is perfectly valid here
			return m_eventLoop->enqueueSync([&]
				{
					return m_unknownBankAccount->balance();
				});

			//or you can use this variant for consistency
			//return m_eventLoop->enqueueSync(
			//	&IBankAccount::balance, m_unknownBankAccount);
		}

	};


}

void worker()
{
	std::thread::id tid{ std::this_thread::get_id() };

	std::ostringstream ss{};

	ss << tid;

	std::cout << "Current Thread: " << ss.str() << std::endl;
}

void one_second_worker ()
{
	std::cout << "> Starting: " << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::thread::id tid{ std::this_thread::get_id() };
	std::ostringstream ss{};
	ss << tid;
	std::cout << "< Current Thread: " << ss.str() << std::endl;
}

void test_event_loop_01()
{
    using namespace Event_Loop;

	EventLoop eventLoop;

	eventLoop.enqueue([]
		{
			std::cout << "message from a different thread\n";
		});


    std::cout << "Done." << std::endl;
}

void test_event_loop_02()
{
	using namespace Event_Loop;

	EventLoop eventLoop;

	eventLoop.enqueue([]
		{
			std::cout << "message from a different thread\n";
		});


	std::cout << "Done." << std::endl;
}

void test_event_loop_03()
{
	using namespace Event_Loop;

	worker();

	EventLoop eventLoop;

	for (int i{}; i < 5; ++i) {

		std::function<void()> w{ one_second_worker };
		eventLoop.enqueue(std::move(w));

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Done." << std::endl;
}

void test_event_loop_04()
{
	using namespace Event_Loop;

	EventLoop eventLoop;

	std::cout << eventLoop.enqueueSync([](const int& x, int&& y, int z)
		{
			return x + y + z;
		}, 1, 2, 3);
}

void test_event_loop_05()
{
	using namespace Event_Loop;

	EventLoop eventLoop;

	std::future<int> result = eventLoop.enqueueAsync([](int x, int y)
		{
			return x + y;
		}, 1, 2);

	//
	//do some heavy work here
	//

	std::cout << result.get();
}

void test_event_loop_10()
{
	using namespace Event_Loop;

	auto eventLoop = std::make_shared<EventLoop>();
	auto bankAccount = std::make_shared<ThreadUnsafeAccount>(100'000);

	std::thread buy = std::thread([](std::unique_ptr<IBankAccount> account)
		{
			for (int i = 1; i <= 10; ++i)
			{
				account->pay(i);
			}
		}, std::make_unique<ThreadSafeAccount>(eventLoop, bankAccount));

	std::thread sell = std::thread([](std::unique_ptr<IBankAccount> account)
		{
			for (int i = 1; i <= 10; ++i)
			{
				account->acquire(i);
			}
		}, std::make_unique<ThreadSafeAccount>(eventLoop, bankAccount));

	buy.join();
	sell.join();

	std::cout << bankAccount->balance() << '\n';
}

void test_event_loop_11()
{
	using namespace Event_Loop;
	ThreadSafeAccount safeAccount(
		std::make_shared<EventLoop>(),
		std::make_shared<ThreadUnsafeAccount>(100'000));


	std::thread buy = std::thread([&]()
		{
			for (int i = 1; i <= 10; ++i)
			{
				safeAccount.pay(i);
			}
		});

	std::thread sell = std::thread([&]
		{
			for (int i = 1; i <= 10; ++i)
			{
				safeAccount.acquire(i);
			}
		});

	buy.join();
	sell.join();

	std::cout << safeAccount.balance() << '\n';

}

void test_event_loop()
{
    using namespace Event_Loop;

    test_event_loop_03();
}

namespace Event_Loop_XXX
{
	class MyClass {
	private:
		// using namespace std::placeholders;

		//just shorthand to avoid long typing
		typedef std::function<void(float result)> TCallback;

		//this function takes long time
		void longRunningFunction(TCallback callback)
		{
			//do some long running task
			//...
			//callback to return result

			float result = 123;

			callback(result);
		}

		//this function gets called by longRunningFunction after its done
		void afterCompleteCallback(float result)
		{
			std::cout << result;
		}

	public:
		int longRunningFunctionAsync()
		{
			//create callback - this equivalent of safe function pointer
			auto callback = std::bind(&MyClass::afterCompleteCallback,
				this, std::placeholders::_1);

			//normally you want to start below function on seprate thread, 
			//but for illustration we will just do simple call
			longRunningFunction(callback);
		}

		int longRunningFunctionAsyncEx()
		{
			//create callback - this equivalent of safe function pointer
			auto callback = std::bind(&MyClass::afterCompleteCallback,
				this, std::placeholders::_1);


			TCallback cb = std::bind(&MyClass::afterCompleteCallback, this, std::placeholders::_1);

			//normally you want to start below function on seprate thread, 
			//but for illustration we will just do simple call
			longRunningFunction(cb);
		}
	};
}