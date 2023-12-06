# Threadsicherer Stapel (Threadsafe Stack)

[Zurück](../../Readme.md)

---


### Allgemeines

Die STL stellt mit der Klasse `std::queue<T>` einen leistungsstarken LIFO-Conainer
(*Last-in, First-out Data Structure*) bereit.

Der Zugriff von mehreren Threads aus auf Objekte dieser Klasse ist jedoch nicht sicher.



### Race Conditions

Unter einer *Race Condition*
versteht man im Umfeld der *Concurrency* die Eigenschaft,
dass Ergebnisse einer Programmausführung davon abhängen,
in welcher Reihenfolge die Ausführung von Operationen in zwei oder mehreren Threads stattfindet.

Nicht immer muss diese Beobachtnung fehlerhaft sein.
Wenn beispielsweise zwei Threads Elemente in einem Stapel zur
weiteren Verarbeitung ablegen,
spielt es keine Rolle, welches Element zuerst hinzugefügt wird.

Problematisch sind *Race Condition* dann, wenn sie *Invarianten* brechen.
Eine Invariante am Beispiel eines Stapels betrachtet könnte sein,
dass dieser beispielsweise zu einem bestimmten Zeitpunkt &ldquo;nicht leer&rdquo; ist.
Dies könnte das Resultat eines Methodenaufrufs wie etwa `empty` sein.
Wird aber zum gleichen Zeitpunkt auf Grund der konkurrierenden Ausführung einer `pop`-Methode
in einem anderen Thread der Stapel geleert, kommt es in dem Thread,
der sich auf den Aufruf von `empty` verlassen möchte,
zu einem Bruch der Invariante. 


### Entwurf des APIs

Die von mir vorgestellt Klasse `ThreadsafeStack<T>`
besitzt folgende öffentliche Schnittstelle:

```cpp
template<typename T>
class ThreadsafeStack
{
public:
    void push(T new_value);
    void pop(T& value);
    T tryPop();
    std::optional<T> tryPopOptional();
    size_t size() const;
    bool empty() const;
};
```

Das Hinzufügen eines Elements zum Stapel kann als trivial betrachtet werden,
auch vor dem Hintergrund der *Concurrency*.

Problematischer sieht es mit der Pop-Operation aus:
Hier ist es &ndash; auf Grund von *Race Conditions* &ndash; nicht ganz einfach,
entsprechende Methodensignaturen zu definieren:

Zwei mögliche Signaturen sind

```cpp
void pop(T& value);
T tryPop();
```

Beide Methoden müssen allerdings eine Ausnahme werfen, wenn der Stapel leer ist.

Möchte man das Exception-Handling umgehen, ist das Datentyp `std::optional<T>`
eine Option:

```cpp
std::optional<T> tryPopOptional()
```


### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeStack<T>`,
so können wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &ldquo;umschlossene&rdquo; `std::stack<T>`-Objekt zu schützen:


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
12:     // prohibit copy constructor, assignment operator and move assignment
13:     ThreadsafeStack(const ThreadsafeStack&) = delete;
14:     ThreadsafeStack& operator = (const ThreadsafeStack&) = delete;
15:     ThreadsafeStack& operator = (ThreadsafeStack&&) noexcept = delete;
16: 
17:     // move constructor may be useful
18:     ThreadsafeStack(const ThreadsafeStack&& other) noexcept
19:     {
20:         std::lock_guard<std::mutex> lock(other.m_mutex);
21:         m_data = other.m_data;
22:     }
23: 
24:     // public interface
25:     void push(T new_value)
26:     {
27:         std::lock_guard<std::mutex> lock{ m_mutex };
28:         m_data.push(new_value);
29:     }
30: 
31:     void pop(T& value)
32:     {
33:         std::lock_guard<std::mutex> lock{ m_mutex };
34:         if (m_data.empty()) throw empty_stack{};
35:         value = m_data.top();
36:         m_data.pop();
37:     }
38: 
39:     T tryPop()
40:     {
41:         std::lock_guard<std::mutex> lock{ m_mutex };
42:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
43:         T value = m_data.top();
44:         m_data.pop();
45:         return value;
46:     }
47: 
48:     std::optional<T> tryPopOptional()
49:     {
50:         std::lock_guard<std::mutex> lock{ m_mutex };
51:         if (m_data.empty()) {
52:             return std::nullopt;
53:         }
54: 
55:         std::optional<T> result{ m_data.top() };
56:         m_data.pop();
57:         return result;
58:     }
59: 
60:     size_t size() const
61:     {
62:         std::lock_guard<std::mutex> lock{ m_mutex };
63:         return m_data.size();
64:     }
65: 
66:     bool empty() const
67:     {
68:         std::lock_guard<std::mutex> lock{ m_mutex };
69:         return m_data.empty();
70:     }
71: };
```

Wir erkennen an der Realisierung,
dass alle Zugriffe auf das zugrunde liegende `std::stack<T>`-Objekt
mithilfe des `std:lock_guard`-Musters (RAII-Stil Mechanismus) zu schützen,
um sicherzustellen, dass die Sperre vom haltenden Thread
in allen möglichen Exit-Szenarien (einschließlich Ausnahme) aufgehoben wird.


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

Semaphore können in Sender-Empfänger-Abläufen verwendet werden.

Wird zum Beispiel eine Semaphore auf 0 initialisiert, blockiert der Empfänger-Aufruf `acquire`,
bis der Sender `release` ausführt.
Damit wartet der Empfänger auf die Benachrichtigung des Senders:

```cpp
01: class Worker
02: {
03: private:
04:     std::binary_semaphore m_semaphore;
05: 
06: public:
07:     Worker() : m_semaphore{ 0 } {}
08: 
09:     void scheduleJob() {
10: 
11:         using namespace std::chrono_literals;
12: 
13:         std::cout << "scheduleJob: Data preparing ...\n";
14: 
15:         std::this_thread::sleep_for(5000ms);
16: 
17:         std::cout << "scheduleJob: Data prepared.\n";
18: 
19:         m_semaphore.release();
20:     }
21: 
22:     void executeJob() {
23: 
24:         using namespace std::chrono_literals;
25: 
26:         std::cout << "executeJob:  Waiting for data ...\n";
27: 
28:         m_semaphore.acquire();
29: 
30:         std::cout << "executeJob:  Executing job ...\n";
31: 
32:         std::this_thread::sleep_for(3000ms);
33: 
34:         std::cout << "executeJob:  Done.\n";
35:     }
36: };
```

*Ausgabe*:


```
scheduleJob: Data preparing ...
executeJob:  Waiting for data ...
scheduleJob: Data prepared.
executeJob:  Executing job ...
executeJob:  Done.
```

Die einmalige Synchronisation von Threads lässt sich einfach mit Semaphoren umsetzen:

Das `std::binary_semaphore`-Objekt `m_semaphore` (Zeile 4) kann die Werte 0 oder 1 besitzen.

Im konkreten Anwendungsfall wird es auf `0` (Zeile 7) initialisiert.

Das heißt, dass der Aufruf `release` den Wert auf 1 (Zeile 19) setzt
und den Aufruf `acquire` in Zeile 28 entblockt.

---

#### Ein zweites Beispiel

Um die Verwendung von binären Semaphoren zu veranschaulichen, implementieren wir eine Druckerwarteschlange,
die zum gleichzeitigen Einstellen von Druckenaufträgen verwendet werden kann.

Die Druckerwarteschlange wird durch ein binäres Semaphor geschützt, sodass jeweils nur Druckauftrag
(ein Thread) drucken kann.

Studieren Sie den Quellcode und die Ausgabe des Programms:

```
Thread 4ADC: PrintingJob:  Going to enqueue a document
Thread 05B0: PrintingJob:  Going to enqueue a document
Thread 517C: PrintingJob:  Going to enqueue a document
Thread 5010: PrintingJob:  Going to enqueue a document
Thread 1650: PrintingJob:  Going to enqueue a document
Thread 2864: PrintingJob:  Going to enqueue a document
Thread 45C8: PrintingJob:  Going to enqueue a document
Thread 538C: PrintingJob:  Going to enqueue a document
Thread 454C: PrintingJob:  Going to enqueue a document
Thread 5068: PrintingJob:  Going to enqueue a document
Thread 4ADC: PrinterQueue: Printing a Job during 507 millseconds.
Thread 4ADC: PrinterQueue: The document has been printed
Thread 05B0: PrinterQueue: Printing a Job during 506 millseconds.
Thread 05B0: PrinterQueue: The document has been printed
Thread 517C: PrinterQueue: Printing a Job during 1112 millseconds.
Thread 517C: PrinterQueue: The document has been printed
Thread 5010: PrinterQueue: Printing a Job during 1921 millseconds.
Thread 5010: PrinterQueue: The document has been printed
Thread 1650: PrinterQueue: Printing a Job during 1807 millseconds.
Thread 1650: PrinterQueue: The document has been printed
Thread 2864: PrinterQueue: Printing a Job during 662 millseconds.
Thread 2864: PrinterQueue: The document has been printed
Thread 45C8: PrinterQueue: Printing a Job during 763 millseconds.
Thread 45C8: PrinterQueue: The document has been printed
Thread 538C: PrinterQueue: Printing a Job during 1737 millseconds.
Thread 538C: PrinterQueue: The document has been printed
Thread 454C: PrinterQueue: Printing a Job during 1006 millseconds.
Thread 454C: PrinterQueue: The document has been printed
Thread 5068: PrinterQueue: Printing a Job during 644 millseconds.
Thread 5068: PrinterQueue: The document has been printed
```

---


#### Quellcode:

[Teil 1: XXX](Semaphore_01.cpp).


---

[Zurück](../../Readme.md)

---
