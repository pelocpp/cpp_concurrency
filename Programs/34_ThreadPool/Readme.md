# Realisierung eines Thread Pools

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::mutex`
  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::condition_variable`
  * Klasse `std::future`
  * Klasse `std::packaged_task`
  * Klasse `std::thread`
  * Klasse `std::queue`
  * Klasse `std::move_only_function`


<ins>Funktionen</ins>:

  * Funktion `std::thread::hardware_concurrency`
  * Funktion `std::bind`

---

## Allgemeines

Ein *Thread Pool* ermöglicht es, Threads wiederzuverwenden.
Auf diese Weise wird verhindert, dass zur Laufzeit neue Threads erstellt werden müssen.
Das Erstellen neuer Threads ist zeit- und ressourcenintensiv. 

Wir stellen in diesem Projekt einige Thread Pool Realisierungen vor.

---

## Eine sehr einfache Thread Pool Realisierung

> Literaturhinweis: Diese Realisierung stammt aus dem Buch &bdquo;Concurrency in Action - 2nd Edition&rdquo; von
Anthony Williams, Kapitel 9.1.

In dieser Realisierung besteht der Thread Pool aus einer festen Anzahl von Worker Threads.
Typischerweise wird diese Anzahl von der Funktion `std::thread::hardware_concurrency()` festgelegt.

Steht eine Aufgabe (*Task*) zur Ausführung an, gibt es am Thread Pool eine Methode (hier: `submit`),
die diese Funktion (*Callable*) in die Warteschlange aller noch ausstehenden Tasks am Ende hinzufügt.

Jeder Worker Thread entnimmt, wenn er nichts zu tun hat, eine Task vom Anfang dieser Warteschlange und führt die Funktion aus.
Nach Ausführung der Funktion entnimmt der Worker Thread die nächste Task aus der Warteschlange
oder er begibt sich in einen *Idle*-Zustand, wenn die Warteschlange leer ist.

In der aktuellen Realisierung haben die Tasks in der Warteschlange alle den Rückgabetyp `void`,
es gibt also keine direkte Möglichkeit, ein Ergebnis zurückzuliefern.

Auch gibt es keine Möglichkeit, auf das Ende der Ausführung einer Task zu warten.

Der größte Nachteil in dieser ersten Realisierung besteht jedoch darin,
dass die Worker Threads, die sich im *Idle*-Zustand befinden, aktiv den Zustand der Warteschlange überprüfen.
Wir haben es also mit dem so genannten &bdquo;*Busy Polling*&rdquo; zu tun.
Diesen Nachteil werden wir in einer nachfolgenden Variation beheben.

---

### Weitere Hinweise zur Realisierung

Die `worker_thread`-Funktion selbst ist recht einfach:
Sie befindet sich in einer Wiederholungsschleife und wartet, bis das `m_done`-Flag gesetzt ist,
entnimmt Tasks aus der Warteschlange und führt sie in der Zwischenzeit aus:


```cpp
01: void ThreadPool::worker_thread()
02: {
03:     while (!m_done)
04:     {
05:         std::function<void()> task{};
06: 
07:         if (m_workQueue.tryPop(task))
08:         {
09:             task();
10:         }
11:         else
12:         {
13:             std::this_thread::yield();
14:         }
15:     }
16: }
```

Die Ursache des *Busy Pollings* ist in der Methode `tryPop` verborgen:

```cpp
01: bool tryPop(T& value)
02: {
03:     std::lock_guard<std::mutex> lock{ m_mutex };
04:     if (m_data.empty()) {
05:         return false;
06:     }
07: 
08:     value = m_data.front();
09:     m_data.pop();
10:     return true;
11: }
```

Die `tryPop`-Methode verwendet keinerlei Koordinierungsmechanismen (wie z.B. ein `std::condition_variable`-Objekt),
um mit entsprechenden `wait`- und `notify_one`-Aufrufen das *Busy Polling* zu umgehen.

Wenn sich keine Tasks in der Warteschlange befinden, ruft die Funktion `std::this_thread::yield()` auf,
um zumindest eine kleine Pause einzulegen und einem anderen Thread die Möglichkeit zu geben,
etwas Arbeit in die Warteschlange zu stellen, bevor er beim nächsten Mal wieder versucht, etwas zu entnehmen.

Beachten Sie, dass die Reihenfolge der Deklarationen der Instanzvariablen von Klasse `ThreadPool` wichtig ist:

```cpp
01: class ThreadPool
02: {
03: private:
04:     std::atomic_bool                        m_done;
05:     ThreadsafeQueue<std::function<void()>>  m_workQueue;
06:     std::vector<std::thread>                m_threads;
07:     JoinThreads                             m_joiner;
08: 
09: ...
```

Sowohl das `m_done`-Flag als auch das Objekt `m_workQueue` müssen vor dem Vektor der Threads `m_threads` deklariert werden,
der wiederum vor dem Objekt des Typs `JoinThreads` deklariert werden muss:

Dadurch wird sichergestellt, dass die Instanzvariablen in der richtigen Reihenfolge zerstört werden.
Das `m_joiner`-Objekt ist in seinem Destruktor dafür verantwortlich, auf das Ende aller Threads zu warten.
Erst danach kann man die beiden Objekte mit den Threads und den Tasks sicher zerstören,
wenn alle Worker Threads gestoppt worden sind.

---

### Ein Blick auf die Ausführung


Wir reihen 10 Tasks in ein `ThreadPool`-Objekt ein:

```cpp
01: auto callable = [] ()
02: {
03:     std::stringstream ss;
04:     ss << "Thread " << std::setw(4) << std::setfill('0')
05:         << std::uppercase << std::hex << std::this_thread::get_id();
06: 
07:     std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
08: };
```

Die Ausführung des Thread Pools in der Konsole sieht in etwa so aus:

Zu Beginn:

```
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[1]:    push_back of next worker_thread function ...
[3]:    > worker_thread ...
[2]:    > worker_thread ...
[3]:    std::this_thread::yield ...
[2]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[4]:    > worker_thread ...
[2]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[7]:    > worker_thread ...
[9]:    > worker_thread ...
[7]:    std::this_thread::yield ...
[6]:    > worker_thread ...
[7]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[5]:    > worker_thread ...
[1]:    push_back of next worker_thread function ...
[2]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
```

Wir erkennen jetzt schon die vielen Aufrufe von `std::this_thread::yield`.

Wenn keine neuen Aufgaben in den Pool hinzugefügt werden, sieht es so aus:

```
[4]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[4]:    std::this_thread::yield ...
```

Sind Tätigkeiten zu Ende, finden wir deren Ausgaben vor:

```
[8]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[7]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[11]:   ###  > Thread 22868
[10]:   ###  > Thread 21496
[11]:   std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[11]:   std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[10]:   std::this_thread::yield ...
[2]:    ###  > Thread 22068
[2]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[2]:    std::this_thread::yield ...
[12]:   ###  > Thread 17956
[11]:   std::this_thread::yield ...
[8]:    std::this_thread::yield ...
[11]:   std::this_thread::yield ...
```

Am Ende sieht es so aus:

```
[7]:    std::this_thread::yield ...
[6]:    std::this_thread::yield ...
[9]:    std::this_thread::yield ...
[9]:    < worker_thread ...
[16]:   std::this_thread::yield ...
[13]:   std::this_thread::yield ...
[12]:   std::this_thread::yield ...
[3]:    std::this_thread::yield ...
[12]:   < worker_thread ...
[8]:    std::this_thread::yield ...
[17]:   std::this_thread::yield ...
[4]:    std::this_thread::yield ...
[1]:    > ~JoinThreads ...
[4]:    < worker_thread ...
[6]:    < worker_thread ...
[16]:   < worker_thread ...
[13]:   < worker_thread ...
[15]:   < worker_thread ...
[5]:    < worker_thread ...
[14]:   < worker_thread ...
[2]:    < worker_thread ...
[10]:   < worker_thread ...
[3]:    < worker_thread ...
[11]:   std::this_thread::yield ...
[11]:   < worker_thread ...
[8]:    < worker_thread ...
[17]:   < worker_thread ...
[7]:    < worker_thread ...
[1]:    < ~JoinThreads ...
```

---

## Eine zweite Thread Pool Realisierung

Diese zweite Realisierung stammt von Zen Sepiol.

Für weitere Erläuterungen sieht man sich am besten die Youtube Tutorials
[How to write Thread Pools in C++](https://www.youtube.com/watch?v=6re5U82KwbY)
und
[How C++23 made my Thread Pool twice as fast](https://www.youtube.com/watch?v=meiGRnyRBXM&t=1s) an.

Die Quellen des Thread Pools sind auch auf [Github](https://github.com/ZenSepiol/Dear-ImGui-App-Framework/blob/main/src/lib/thread_pool/thread_pool_test.cpp) hinterlegt.

Wir stellen die Realisierung komplett vor:

*Header-Datei*:

```cpp
01: using ThreadPoolFunction = std::move_only_function<void(void)>;
02: 
03: class ThreadPool
04: {
05: private:
06:     mutable std::mutex              m_mutex;
07:     std::condition_variable         m_condition;
08:     std::vector<std::thread>        m_pool;
09:     std::queue<ThreadPoolFunction>  m_queue;
10:     size_t                          m_threads_count;
11:     size_t                          m_busy_threads;
12:     bool                            m_shutdown_requested;
13: 
14: public:
15:     // c'tors/d'tor
16:     ThreadPool();
17:     ~ThreadPool();
18: 
19:     // no copying or moving
20:     ThreadPool(const ThreadPool&) = delete;
21:     ThreadPool& operator=(const ThreadPool&) = delete;
22:     ThreadPool(ThreadPool&&) = delete;
23:     ThreadPool& operator=(ThreadPool&&) = delete;
24: 
25:     // public interface
26:     void start();
27:     void stop();
28: 
29:     template <typename F, typename... Args>
30:         
31:     auto addTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
32:     {
33:         Logger::log(std::cout, "addTask ...");
34: 
35:         auto func{ std::bind(std::forward<F>(f), std::forward<Args>(args)...) };
36: 
37:         auto task{ std::packaged_task<decltype( f(args...)) (void) > { func }};
38: 
39:         auto future = task.get_future();
40: 
41:         // generalized lambda capture
42:         auto wrapper = [task = std::move(task)]() mutable { task(); };
43: 
44:         {
45:             std::lock_guard<std::mutex> guard{ m_mutex };
46:             m_queue.push(std::move(wrapper));
47:         }
48: 
49:         // wake up one waiting thread if any
50:         m_condition.notify_one();
51: 
52:         // return future from packaged_task
53:         return future;
54:     }
55: 
56:     // getter
57:     bool empty() const;
58:     size_t size() const;
59: 
60: private:
61:     void worker();
62: };
```

*Implementierungs-Datei*:

```cpp
01: ThreadPool::ThreadPool()
02:     : m_threads_count{}, m_busy_threads{ }, m_shutdown_requested {}
03: {}
04: 
05: ThreadPool::~ThreadPool()
06: {
07:     stop();
08: }
09: 
10: void ThreadPool::start()
11: {
12:     size_t size{ std::thread::hardware_concurrency() };
13: 
14:     m_pool.resize(size);
15: 
16:     for (size_t i{}; i != size; ++i)
17:     {
18:         m_pool[i] = std::thread(&ThreadPool::worker, this);
19:     }
20: 
21:     m_threads_count = size;
22:     m_busy_threads = size;
23: }
24: 
25: void ThreadPool::stop()
26: {
27:     // waits until threads finish their current task and shutdowns the pool
28: 
29:     {
30:         std::lock_guard<std::mutex> guard{ m_mutex };
31:         m_shutdown_requested = true;
32:     }
33: 
34:     m_condition.notify_all();
35: 
36:     for (size_t i{}; i != m_pool.size(); ++i)
37:     {
38:         if (m_pool[i].joinable())
39:         {
40:             m_pool[i].join();
41:         }
42:     }
43: }
44: 
45: void ThreadPool::worker()
46: {
47:     std::thread::id tid{ std::this_thread::get_id() };
48: 
49:     Logger::log(std::cout, "Started worker [", tid, "]");
50: 
51:     std::unique_lock<std::mutex> guard{ m_mutex };
52: 
53:     while (!m_shutdown_requested || (m_shutdown_requested && !m_queue.empty()))
54:     {
55:         m_busy_threads--;
56: 
57:         m_condition.wait(guard, [this] {
58:             return m_shutdown_requested || !m_queue.empty();
59:         });
60: 
61:         m_busy_threads++;
62: 
63:         if (!this->m_queue.empty())
64:         {
65:             auto func{ std::move(m_queue.front()) };
66:             m_queue.pop();
67: 
68:             guard.unlock();
69: 
70:             func();
71: 
72:             guard.lock();
73:         }
74:     }
75: 
76:     Logger::log(std::cout, "Worker Done [", tid, "]");
77: }
78: 
79: bool ThreadPool::empty() const
80: {
81:     std::lock_guard<std::mutex> guard{ m_mutex };
82:     return m_queue.empty();
83: }
84: 
85: size_t ThreadPool::size() const
86: {
87:     std::lock_guard<std::mutex> guard{ m_mutex };
88:     return m_queue.size();
89: }
```

Ein *Beispiel*:

```cpp
01: void test()
02: {
03:     Logger::log(std::cout, "Start");
04: 
05:     ScopedTimer clock{};
06: 
07:     size_t foundPrimeNumbers{};
08: 
09:     std::queue<std::future<bool>> results;
10: 
11:     ThreadPool pool{};
12: 
13:     Logger::log(std::cout, "Enqueuing tasks");
14: 
15:     Logger::enableLogging(false);
16: 
17:     for (size_t i{ Globals::Start }; i < Globals::End; i += 2) {
18: 
19:         std::future<bool> future{ pool.addTask(isPrime, i) };
20: 
21:         results.emplace(std::move(future));
22:     }
23: 
24:     Logger::enableLogging(true);
25: 
26:     Logger::log(std::cout, "Enqueuing tasks done.");
27: 
28:     pool.start();
29: 
30:     while (results.size() != 0)
31:     {
32:         auto found = results.front().get();
33:         if (found) {
34:             ++foundPrimeNumbers;
35:         }
36: 
37:         results.pop();
38:     }
39: 
40:     Logger::log(std::cout, "Found ", foundPrimeNumbers, " prime numbers between ", Globals::Start, " and ", Globals::End, '.');
41:         
42:     pool.stop();
43: 
44:     Logger::log(std::cout, "Done.");
45: }
```

*Ausgabe*:

```
[1]:    Start
[1]:    Enqueuing tasks
[1]:    Enqueuing tasks done.
[2]:    Started worker [17736]
[4]:    Started worker [17784]
[3]:    Started worker [18880]
[5]:    Started worker [15876]
[6]:    Started worker [3088]
[7]:    Started worker [16240]
[8]:    Started worker [11676]
[9]:    Started worker [16296]
[10]:   Started worker [17888]
[11]:   Started worker [18496]
[12]:   Started worker [8552]
[13]:   Started worker [17000]
[15]:   Started worker [12916]
[14]:   Started worker [9380]
[16]:   Started worker [4992]
[17]:   Started worker [16196]
[1]:    Found 4 prime numbers between 1000000000000000001 and 1000000000000000101.
[3]:    Worker Done [18880]
[2]:    Worker Done [17736]
[4]:    Worker Done [17784]
[17]:   Worker Done [16196]
[13]:   Worker Done [17000]
[15]:   Worker Done [12916]
[14]:   Worker Done [9380]
[12]:   Worker Done [8552]
[10]:   Worker Done [17888]
[11]:   Worker Done [18496]
[9]:    Worker Done [16296]
[8]:    Worker Done [11676]
[5]:    Worker Done [15876]
[16]:   Worker Done [4992]
[7]:    Worker Done [16240]
[6]:    Worker Done [3088]
[1]:    Done.
[1]:    Elapsed time: 3385 [milliseconds]
```

---

## Literaturhinweise

Das erste Beispiel ist aus dem Buch &bdquo;Concurrency in Action - 2nd Edition&rdquo; von
Anthony Williams, Kapitel 9.1, entnommen.

---

[Zurück](../../Readme.md)

---
