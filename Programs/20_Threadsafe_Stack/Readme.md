# Threadsicherer Stapel (Threadsafe Stack)

[Zurück](../../Readme.md)

---

# Verwendete Hilfsmittel:

  * `std::mutex`
  * `std::lock_guard`
  * `std::thread`

---

### Allgemeines

Die STL stellt mit der Klasse `std::stack<T>` einen leistungsstarken LIFO-Container bereit
(*Last-in, First-out* Datenstruktur).

Der Zugriff von mehreren Threads aus auf Objekte dieser Klasse ist jedoch nicht sicher.


### Race Conditions

Unter einer *Race Condition*
versteht man im Umfeld der *Concurrency* die Eigenschaft,
dass Ergebnisse einer Programmausführung davon abhängen,
in welcher Reihenfolge die Ausführung von Operationen in zwei oder mehreren Threads stattfindet.

Nicht immer muss diese Beobachtung fehlerhaft sein.
Wenn beispielsweise zwei Threads Elemente in einem Stapel zur
weiteren Verarbeitung ablegen,
spielt es keine Rolle, welches Element zuerst hinzugefügt wird.

Problematischer sind *Race Conditions* dann, wenn sie *Invarianten* brechen.
Eine Invariante am Beispiel eines Stapels betrachtet könnte sein,
dass dieser beispielsweise zu einem bestimmten Zeitpunkt &bdquo;nicht leer&rdquo; ist.
Dies könnte das Resultat eines Methodenaufrufs wie etwa `empty` sein.

Wird aber zum gleichen Zeitpunkt auf Grund der konkurrierenden Ausführung einer `pop`-Methode
in einem anderen Thread der Stapel tatsächlich geleert, kommt es in dem Thread,
der sich auf den Aufruf von `empty` verlassen möchte,
zu einem Bruch der Invariante. 


### Entwurf des APIs

Die von mir vorgestellt Klasse `ThreadsafeStack<T>`
besitzt folgende öffentliche Schnittstelle:

```cpp
01: template<typename T>
02: class ThreadsafeStack
03: {
04: public:
05:     void push(const T& value);
06:     void push(T&& value);
07:     void pop(T& value);
08:     T tryPop();
09:     std::optional<T> tryPopOptional();
10:     size_t size() const;
11:     bool empty() const;
12: };
```

Das Hinzufügen eines Elements zum Stapel (Methode `push`) kann als trivial betrachtet werden,
auch vor dem Hintergrund der *Concurrency*.

Problematischer sieht es mit der `pop`-Operation aus:
Hier ist es &ndash; auf Grund von *Race Conditions* &ndash; nicht ganz einfach,
entsprechende Methodensignaturen zu definieren:

Zwei mögliche Signaturen sind

```cpp
void pop(T& value);
```

oder 

```cpp
T tryPop();
```


Beide Methoden müssen allerdings eine Ausnahme werfen, wenn der Stapel leer ist.

Möchte man das Exception-Handling umgehen, wäre der Datentyp `std::optional<T>` eine Option:

```cpp
std::optional<T> tryPopOptional();
```

### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeStack<T>`,
so können wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &bdquo;umschlossene&rdquo; `std::stack<T>`-Objekt zu schützen:

```cpp
01: template<typename T>
02: class ThreadsafeStack
03: {
04: private:
05:     std::stack<T>      m_data;
06:     mutable std::mutex m_mutex;
07: 
08: public:
09:     // c'tors
10:     ThreadsafeStack() {}
11: 
12:     // prohibit assignment operator and move assignment
13:     ThreadsafeStack& operator= (const ThreadsafeStack&) = delete;
14:     ThreadsafeStack& operator= (ThreadsafeStack&&) noexcept = delete;
15: 
16:     // copy and move constructor may be useful
17:     ThreadsafeStack(const ThreadsafeStack& other) noexcept
18:     {
19:         std::lock_guard<std::mutex> lock{ other.m_mutex };
20:         m_data = other.m_data;
21:     }
22:         
23:     ThreadsafeStack(ThreadsafeStack&& other) noexcept
24:     {
25:         std::lock_guard<std::mutex> lock{ other.m_mutex };
26:         m_data = std::move(other.m_data);
27:     }
28: 
29:     // public interface
30:     void push(const T& value)
31:     {
32:         std::lock_guard<std::mutex> lock{ m_mutex };
33:         m_data.push(value);
34:     }
35: 
36:     void push(T&& value)
37:     {
38:         std::lock_guard<std::mutex> lock{ m_mutex };
39:         m_data.push(std::move(value));
40:     }
41: 
42:     template<class... TArgs>
43:     void emplace(TArgs&&... args)
44:     {
45:         std::lock_guard<std::mutex> lock{ m_mutex };
46:         m_data.emplace(std::forward<TArgs>(args) ...);
47:     }
48: 
49:     void pop(T& value)
50:     {
51:         std::lock_guard<std::mutex> lock{ m_mutex };
52:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
53:         value = m_data.top();
54:         m_data.pop();
55:     }
56: 
57:     T tryPop()
58:     {
59:         std::lock_guard<std::mutex> lock{ m_mutex };
60:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
61:         T value = m_data.top();
62:         m_data.pop();
63:         return value;
64:     }
65: 
66:     std::optional<T> tryPopOptional()
67:     {
68:         std::lock_guard<std::mutex> lock{ m_mutex };
69:         if (m_data.empty()) {
70:             return std::nullopt;
71:         }
72: 
73:         std::optional<T> result{ m_data.top() };
74:         m_data.pop();
75:         return result;
76:     }
77: 
78:     size_t size() const
79:     {
80:         std::lock_guard<std::mutex> lock{ m_mutex };
81:         return m_data.size();
82:     }
83: 
84:     bool empty() const
85:     {
86:         std::lock_guard<std::mutex> lock{ m_mutex };
87:         return m_data.empty();
88:     }
89: };
```

Wir erkennen an der Realisierung,
dass alle Zugriffe auf das zugrunde liegende `std::stack<T>`-Objekt
mithilfe des `std:lock_guard`-Objekts (RAII-Stil Mechanismus) geschützt werden,
um auf diese Weise sicherzustellen, dass vorhandene Sperren vom haltenden Thread
in allen möglichen Exit-Szenarien (einschließlich Ausnahmen) wieder aufgehoben werden.


### Schlüsselwort `mutable`

In Zeile 6 der Realisierung wird das `std::mutex`-Objekt mit dem Schlüsselwort `mutable`
versehen. Prinzipiell ist dies erforderlich, um die beiden Methoden `size` und `empty`
wiederum mit `const` markieren zu können.

Interessanterweise lässt sich die gezeigte Realisierung auch ohne Verwendung von `mutable` übersetzen &ndash;
bei Gebrauch des *Visual C++* Compilers.
Der Gcc-Compiler reagiert an dieser Stelle empfindlicher:

```
error: binding reference of type 'std::lock_guard<std::mutex>::mutex_type&' {aka 'std::mutex&'} to 'const std::mutex' 
```

---

#### Ein erstes Beispiel

Im Beispiel zu diesem Abschnitt finden Sie eine Klasse `PrimeCalculator` vor.
Eine Instanz dieser Klasse berechnet Primzahlen in einem bestimmten Zahlenbereich,
der bei der Erzeugung eines `PrimeCalculator`-Objekts anzugeben ist.

Mehrere Instanzen der `PrimeCalculator`-Klasse berechnen Primzahlen in unterschiedlichen Zahlenbereichen,
und können dies auch quasi-parallel im Kontext mehrerer Threads tun. Einzig und allein die berechneten Primzahlen
sind in einem für alle `PrimeCalculator`-Objekte gleichen Ergebniscontainer abzulegen.

An dieser Stelle setzen wir die `ThreadsafeStack`-Klasse ein.

Weitere Details entnehmen Sie bitte dem Sourcecode.

Das Protokoll zur Berechnung aller Primzahlen im Bereich von 1 bis  10.000.000 sieht so aus:

*Ausgabe*:


```
[1]:    Calcalating Prime Numbers from 2 up to 10000000:
[2]:    TID: 15416
[3]:    TID: 15468
[4]:    TID: 5192
[5]:    TID: 2008
[6]:    TID: 15456
[7]:    TID: 5964
[8]:    TID: 13392
[9]:    TID: 3172
[10]:   TID: 9824
[11]:   TID: 932
[12]:   TID: 13136
[13]:   TID: 3392
[14]:   TID: 16372
[15]:   TID: 13664
[16]:   TID: 4040
[17]:   TID: 3608
[1]:    Elapsed time: 1162 [milliseconds]
[1]:    Found: 664579 prime numbers.
[1]:    Done.
```

Zu dieser Ausgabe gehört das folgende Beispielprogramm:


```cpp
01: void test_primes ()
02: {
03:     constexpr bool Verbose{ false };
04: 
05:     using namespace Concurrency_ThreadsafeStack;
06:     using namespace Concurrency_PrimeCalculator;
07: 
08:     Logger::log(std::cout,
09:         "Calcalating Prime Numbers from ", 2,
10:         " up to ", Globals::UpperLimit, ':');
11: 
12:     using Callable = std::function<void()>;
13: 
14:     auto callableWrapper = [] (Callable callable) {
15: 
16:         if (Verbose) {
17:             Logger::log(std::cout, "TID: ", std::this_thread::get_id());
18:         }
19: 
20:         callable();
21:     };
22: 
23:     ThreadsafeStack<size_t> primes{};
24: 
25:     std::vector<std::thread> threads;
26:     threads.reserve(Globals::NumThreads);
27: 
28:     size_t range = (Globals::UpperLimit - 2) / Globals::NumThreads;
29:     size_t start = 2;
30:     size_t end = start + range;
31: 
32:     {
33:         ScopedTimer timer{};
34: 
35:         // setup threads
36:         for (size_t i{}; i != Globals::NumThreads - 1; ++i) {
37: 
38:             PrimeCalculator<size_t> calc{ primes, start, end };
39:             threads.emplace_back(callableWrapper, calc);
40: 
41:             start = end;
42:             end = start + range;
43:         }
44: 
45:         // setup last thread
46:         PrimeCalculator<size_t> calc{ primes, start, Globals::UpperLimit };
47:         threads.emplace_back(callableWrapper, calc);
48: 
49:         // wait for end of all threads
50:         for (size_t i{}; i != Globals::NumThreads; ++i) {
51:             threads[i].join();
52:         }
53:     }
54: 
55:     Logger::log(std::cout, "Found: ", primes.size(), " prime numbers.");
56:     Logger::log(std::cout, "Done.");
57: }
```


---

#### Quellcode:


[ThreadsafeStack.h](ThreadsafeStack.h).<br />
[PrimeCalculator.h](TPrimeCalculator.h).<br />
[TestPrimeNumbers.cpp](TestPrimeNumbers.cpp).<br />

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Buch

[C++ Concurrency in Action](https://www.cplusplusconcurrencyinaction.com/) von Anthony Williams.

Kleinere Ergänzungen wurden in Abstimmung mit der Unterlage

[Concurrency with Modern C++](https://www.grimm-jaud.de/index.php/concurrency-with-modern-c/) von Rainer Grimm

vorgenommen.

---

[Zurück](../../Readme.md)

---
