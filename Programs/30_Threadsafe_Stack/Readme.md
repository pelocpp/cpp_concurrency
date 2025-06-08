# Threadsicherer Stapel (Threadsafe Stack)

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

  * `std::mutex`
  * `std::lock_guard`
  * `std::thread`

---

### Allgemeines

Die STL stellt mit der Klasse `std::stack<T>` einen leistungsstarken LIFO-Container bereit
(*Last-in, First-out* Datenstruktur).

Der Zugriff von mehreren Threads auf Objekte dieser Klasse ist jedoch nicht sicher.


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
05:     ThreadsafeStack();
06:     ThreadsafeStack& operator= (const ThreadsafeStack&) = delete;
07:     ThreadsafeStack& operator= (ThreadsafeStack&&) noexcept = delete;
08:     ThreadsafeStack(const ThreadsafeStack& other);
09:     ThreadsafeStack(ThreadsafeStack&& other) noexcept;
10:     void push(const T& value);
11:     void push(T&& value);
12:     template<typename... TArgs> void emplace(TArgs&&... args);
13:     void pop(T& value);
14:     bool tryPop(T& value);
15:     std::optional<T> tryPop();
16:     std::optional<T> top() const;
17:     size_t size() const;
18:     bool empty() const;
19: };
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
bool tryPop(T& value);
```

Die erste Methode muss allerdings eine Ausnahme werfen, wenn der Stapel leer ist.
Die zweite Methode umgeht dies, indem sie einen `bool`-Rückgabetyp besitzt.
Ein mögliches Ergebnis wird dann über den Parameter `value` zurückgeliefert, es handelt sich um eine Referenz einer Variablen (eines Objekts),
die (das) beim Aufruf bereitzustellen ist.

Möchte man das Exception-Handling umgehen, wäre auch der Datentyp `std::optional<T>` eine Option:

```cpp
std::optional<T> tryPop();
```

### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeStack<T>`,
so können wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &bdquo;umschlossene&rdquo; `std::stack<T>`-Objekt zu schützen:

```cpp
001: template<typename T>
002: class ThreadsafeStack
003: {
004: private:
005:     std::stack<T>       m_data;
006:     mutable std::mutex  m_mutex;
007: 
008: public:
009:     // c'tors
010:     ThreadsafeStack() {}
011: 
012:     // prohibit assignment operator and move assignment
013:     ThreadsafeStack& operator= (const ThreadsafeStack&) = delete;
014:     ThreadsafeStack& operator= (ThreadsafeStack&&) noexcept = delete;
015: 
016:     // copy and move constructor may be useful
017:     ThreadsafeStack(const ThreadsafeStack& other)
018:     {
019:         std::lock_guard<std::mutex> guard{ other.m_mutex };
020:         m_data = other.m_data;
021:     }
022:         
023:     ThreadsafeStack(ThreadsafeStack&& other) noexcept
024:     {
025:         std::lock_guard<std::mutex> guard{ other.m_mutex };
026:         m_data = std::move(other.m_data);
027:     }
028: 
029:     // public interface
030:     void push(const T& value)
031:     {
032:         std::lock_guard<std::mutex> guard{ m_mutex };
033:         m_data.push(value);
034:     }
035: 
036:     void push(T&& value)
037:     {
038:         std::lock_guard<std::mutex> guard{ m_mutex };
039:         m_data.push(std::move(value));
040:     }
041: 
042:     template<typename... TArgs>
043:     void emplace(TArgs&&... args)
044:     {
045:         std::lock_guard<std::mutex> guard{ m_mutex };
046:         m_data.emplace(std::forward<TArgs>(args) ...);
047:     }
048: 
049:     void pop(T& value)
050:     {
051:         std::lock_guard<std::mutex> guard{ m_mutex };
052:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
053:         value = m_data.top();
054:         m_data.pop();
055:     }
056: 
057:     bool tryPop(T& value)
058:     {
059:         std::lock_guard<std::mutex> guard{ m_mutex };
060:         if (m_data.empty()) {
061:             return false;
062:         }
063:         else {
064:             value = std::move(m_data.top());
065:             m_data.pop();
066:             return true;
067:         }
068:     }
069: 
070:     std::optional<T> tryPop()
071:     {
072:         std::lock_guard<std::mutex> guard{ m_mutex };
073:         if (m_data.empty()) {
074:             return std::optional<T>(std::nullopt);
075:         }
076:         else {
077:             std::optional<T> result{ std::move(m_data.top()) };
078:             m_data.pop();
079:             return result;
080:         }
081:     }
082: 
083:     std::optional<T> top() const
084:     {
085:         std::lock_guard<std::mutex> guard{ m_mutex };
086:         if (m_data.empty()) {
087:             return std::optional<T>(std::nullopt);
088:         }
089:         else {
090:             std::optional<T> result(m_data.top());
091:             return result;
092:         }
093:     }
094: 
095:     size_t size() const
096:     {
097:         std::lock_guard<std::mutex> guard{ m_mutex };
098:         return m_data.size();
099:     }
100: 
101:     bool empty() const
102:     {
103:         std::lock_guard<std::mutex> guard{ m_mutex };
104:         return m_data.empty();
105:     }
106: };
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
Der GCC-Compiler reagiert an dieser Stelle empfindlicher:

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
sind in einem für alle `PrimeCalculator`-Objekte zur Verfügung stehenden Ergebniscontainer abzulegen.

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

#### Quellcode


[*ThreadsafeStack.h*](ThreadsafeStack.h).<br />
[*PrimeCalculator.h*](PrimeCalculator.h).<br />
[*TestPrimeNumbers.cpp*](TestPrimeNumbers.cpp).<br />

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
