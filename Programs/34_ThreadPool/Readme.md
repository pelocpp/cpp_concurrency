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

---

## Allgemeines

Ein *Thread Pool* ermöglicht es, Threads wiederzuverwenden.
Auf diese Weise wird verhindert, dass zur Laufzeit neue Threads erstellt werden müssen.
Das Erstellen neuer Threads ist zeit- und ressourcenintensiv. 

In der einschlägigen Literatur oder im Netz findet man Realisierungen für Thread Pools vor:

  * Buch von Anthony Williams: &bdquo;Concurrency in Action &ndash; 2nd Edition&rdquo;,<br />Kapitel 9: &bdquo;Thread Pools&rdquo;.

  * Buch von  Arthur O'Dwyer: &bdquo;Mastering the C++17 STL&rdquo;,<br />Kapitel 7: &bdquo;Building your own thread pool&rdquo;.
  
  * Zwei Artikel von Martin Vorbrodt: &bdquo;Vorbrodt's C++ Blog&rdquo; &ndash;<br />&bdquo;[Simple thread pool](https://vorbrodt.blog/2019/02/27/advanced-thread-pool/)&rdquo; und &bdquo;[Advanced thread pool](https://vorbrodt.blog/2019/02/12/simple-thread-pool/)&rdquo;.

Wir stellen in diesem Projekt eine Überarbeitung einer Thread Pool Realisierung von Zen Sepiol vor,
die in Youtube verfügbar ist:<br />
[How to write Thread Pools in C++](https://www.youtube.com/watch?v=6re5U82KwbY)
und
[How C++23 made my Thread Pool twice as fast](https://www.youtube.com/watch?v=meiGRnyRBXM&t=1s).

---

## Einige Details in der Thread Pool Realisierung

In der vorliegenden Realisierung besteht der Thread Pool aus zwei Warteschlangen:

  * Warteschlangen mit Worker Threads
  * Warteschlangen mit *Tasks* bzw. *Callables* (auszuführenden Funktionen)

Für die Warteschlangen der Worker Threads greifen wir auf den STL-Container zurück:

```cpp
std::vector<std::thread> m_pool;
```

Typischerweise wird die Größe dieses Containers, also die Anzahl der zur Verfügung stehenden Worker-Threads,
von der Funktion `std::thread::hardware_concurrency()` beeinflusst.


Nun kommen wir auf die zweite Warteschlangen mit den *Callables* (auszuführenden Funktionen) zu sprechen.
Steht eine Aufgabe (*Task*) zur Ausführung an, gibt es am Thread Pool eine Methode (hier: `addTask`),
die die dazugehörige Funktion (*Callable*) in die Warteschlange aller noch ausstehenden Tasks am Ende hinzufügt.

Wie legen wir den Datentyp für eine solche *Task* fest?
Ein sehr einfacher Ansatz würde hierzu *Callables* mit einer festen Signatur festlegen,
zum Beispiel Funktionen ohne Parameter und mit Rückgabetyp `void`. Derartige Funktionen könnte man dann
mit der *Universal Function* Wrapperklasse `std::function` als Variablen in einem Programm hantieren:

```cpp
std::function<void()> func;
```

Unser Anspruch besteht darin, beliebige Funktionen mit unterschiedlichen Signaturen als Threadprozeduren verwalten zu können.
Dazu müssen wir zunächst einmal eine &bdquo;flexible&rdquo; `addTask`-Methode definieren.
Die Flexibilität gewinnen wir mit variadischen Parametern:

```cpp
template <typename TFunc, typename... TArgs>
auto addTask(TFunc&& func, TArgs&&... args)
    -> std::future<typename std::invoke_result<TFunc, TArgs...>::type>
{
    ...
}
```

Der Parameter `func` nimmt ein *Callable* entgegen, die Parameter zum Aufruf dieses  *Callables* wiederum
folgen in einer variablen Anzahl von Parametern, die als *Parameter Pack* `args` beschrieben werden. 

Wie lassen sich der Werte dieser Parameter in einem Hüllenobjekt zwischenspeichern?
Dazu bietet sich ein Lambda-Objekt an, das die Parameter über den *Closure* in das Lambda-Objekt kopiert.

Jetzt haben wir aber nicht eine feste Anzahl von Parametern, sondern variabel viele.
An dieser Stelle kommt eine &bdquo;*variadische Capture Clause*&rdquo; ins Spiel,
also syntaktisch gesehen ein Ausdruck der Gestalt

```cpp
[...args = std::forward<Args>(args)]
```

Damit werden das *Callable* und die Parameter durch einen Aufruf von `addTask`
wie folgt in einem Hüllenobjekt abgelegt:

```cpp
template <typename TFunc, typename... TArgs>
auto addTask(TFunc&& func, TArgs&&... args)
    -> std::future<typename std::invoke_result<TFunc, TArgs...>::type>
{
    using ReturnType = std::invoke_result<TFunc, TArgs...>::type;

    auto task = std::packaged_task<ReturnType()>{
        [func = std::forward<TFunc>(func),
        ... args = std::forward<TArgs>(args)]() mutable -> ReturnType
        {
            return std::invoke(std::move(func), std::move(args) ...);
        }
    };

    ...
}
```

Mit dem so genannten &bdquo;*Generalized Lambda Capture*&rdquo; kann die Move-Semantik
beim Transport der Daten in das Lambda-Objekt Anwendung finden, zum Beispiel so:

```cpp
[func = std::forward<TFunc>(func),
...
```

Der Ergebnistyp des Hüllenobjekts ließe sich vom Compiler mit *Automatic Type Deduction* herleiten,
zu Demonstationszwecken können wir ihn aber auch explizit hinschreiben:

```cpp
std::invoke_result<TFunc, TArgs...>::type
```

oder noch kürzer als

```cpp
std::invoke_result_t<TFunc, TArgs...>
```

Hier kommt das Template `std::invoke_result_t` zum Zuge, das genau für diesen Verwendungszweck in der STL vorhanden ist.


Wozu legen wir eigentlich ein `std::packaged_task`-Objekt an?
Für den von mir gewählten Lösungsansatz will ich Ergebnisse von den Thread-Prozeduren zurück erhalten,
sprich wir benötigen pro asynchroner Funktionsausführung ein `std::future`-Objekt.
Dieses erhalten wir wiederum von einem `std::packaged_task`-Objekt mit der Methode `get_future`:

```cpp
auto task = std::packaged_task<ReturnType()>{
    ...
};

auto future{ task.get_future() };
```

Noch sind wir nicht am Ziel:
Wir müssen die *Task*-Objekte in einer Warteschlange ablegen:

```cpp
std::queue<std::move_only_function<void()>> m_queue;
```

Der Hüllentyp `std::move_only_function<>` ist der Typ schechthin, um *Callable*-Objekte performant verschieben zu können.
Nur ist die Schnittstelle `void()>` wieder etwas &bdquo;eng gefasst&rdquo;, wir wollten doch Threadprozeduren 
mit variabler Anzahl von Parametern unterschiedlichen Datentyps verwalten können.

Okay, auf eine Hülle mehr oder weniger kommt es jetzt auch nicht mehr an:

```cpp
auto wrapper{ [task = std::move(task)] () mutable -> void { task(); } };
```

Ja, Sie haben es richtig gesehen: Mit dem Lambda aus dem letzten Code-Snippet definieren wir ein *Callable*,
dass die Signatur `void()` hat! 
Dieses Hüllenobjekt können wir nun in unsere Warteschlange für Threadprozeduren einreihen:

```cpp
m_queue.push(std::move(wrapper));
```

Damit haben wir die zentralen Stellen der Methode `addTask` betrachtet,
ein zugegebenermaßen nicht ganz leichtes Unterfangen.
Die Methode im Ganzen sieht so aus:

```cpp
01: template <typename TFunc, typename... TArgs>
02: auto ThreadPool::addTask(TFunc&& func, TArgs&&... args)
03:     -> std::future<typename std::invoke_result<TFunc, TArgs...>::type>
04: {
05:     using ReturnType = std::invoke_result<TFunc, TArgs...>::type;
06: 
07:     auto task = std::packaged_task<ReturnType()>{
08:         [func = std::forward<TFunc>(func),
09:         ... args = std::forward<TArgs>(args)]() mutable -> ReturnType
10:         {
11:             return std::invoke(std::move(func), std::move(args) ...);
12:         }
13:     };
14: 
15:     auto future{ task.get_future() };
16: 
17:     // generalized lambda capture
18:     auto wrapper{ [task = std::move(task)]() mutable -> void { task(); } };
19: 
20:     {
21:         std::lock_guard<std::mutex> guard{ m_mutex };
22:         m_queue.push(std::move(wrapper));
23:     }
24: 
25:     // wake up one waiting thread if any
26:     m_condition.notify_one();
27: 
28:     // return future from packaged_task
29:     return future;
30: }
```


Jetzt vollziehen wir einen Wechsel von der Warteschlange der Threadprozeduren zur Warteschlange der Workerthreads.
Jeder Worker Thread entnimmt, wenn er nichts zu tun hat, eine Task vom Anfang der Warteschlange der *Tasks* und führt die hierin gekapselte Funktion aus.
Nach der Ausführung der Funktion entnimmt der Worker Thread die nächste Task aus der Warteschlange
oder er begibt sich in einen *Idle*-Zustand, wenn die Warteschlange mit den *Tasks* leer ist.

Die Betrachtungen zur Warteschlange der Workerthreads fassen wir hier etwas kürzer,
da der Quellcode nicht so komplex geraten ist:

```cpp
01: void ThreadPool::worker()
02: {
03:     std::unique_lock<std::mutex> guard{ m_mutex };
04: 
05:     while (!m_shutdown_requested || (m_shutdown_requested && !m_queue.empty()))
06:     {
07:         m_busy_threads--;
08: 
09:         m_condition.wait(guard, [this] {
10:             return m_shutdown_requested || !m_queue.empty();
11:         });
12: 
13:         m_busy_threads++;
14: 
15:         if (!this->m_queue.empty())
16:         {
17:             auto func{ std::move(m_queue.front()) };
18:             m_queue.pop();
19: 
20:             guard.unlock();
21: 
22:             func();
23: 
24:             guard.lock();
25:         }
26:     }
27: }
```



---

### Ein zweiter Ansatz in der Realisierung der `addTask`-Methode





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
01: using ThreadPoolFunction = std::move_only_function<void()>;
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
