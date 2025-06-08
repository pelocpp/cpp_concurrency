# Threadsichere Warteschlange (Threadsafe Queue)

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

  * `std::mutex`
  * `std::lock_guard`, `std::unique_lock` und `std::scoped_lock`
  * `std::condition_variable`
  * `std::thread`

---

### Allgemeines

Die STL stellt mit der Klasse `std::queue<T>` einen leistungsstarken FIFO-Container bereit
(*First-in, First-out* Datenstruktur).

Der Zugriff von mehreren Threads auf Objekte dieser Klasse ist jedoch nicht sicher.


### Race Conditions

Unter einer *Race Condition*
versteht man im Umfeld der *Concurrency* die Eigenschaft,
dass Ergebnisse einer Programmausführung davon abhängen,
in welcher Reihenfolge die Ausführung von Operationen in zwei oder mehreren Threads stattfindet.

Nicht immer muss diese Beobachtung fehlerhaft sein.
Wenn beispielsweise zwei Threads Elemente in einer Warteschlange zur
weiteren Verarbeitung ablegen,
spielt es keine Rolle, welches Element zuerst hinzugefügt wird.

Problematischer sind *Race Conditions* dann, wenn sie *Invarianten* brechen.
Eine Invariante am Beispiel einer Warteschlange betrachtet könnte sein,
dass diese beispielsweise zu einem bestimmten Zeitpunkt &bdquo;nicht leer&rdquo; ist.
Dies könnte das Resultat eines Methodenaufrufs wie etwa `empty` sein.

Wird aber zum gleichen Zeitpunkt auf Grund der konkurrierenden Ausführung einer `pop`-Methode
in einem anderen Thread die Warteschlange tatsächlich geleert, kommt es in dem Thread,
der sich auf den Aufruf von `empty` verlassen möchte,
zu einem Bruch der Invariante. 


### Entwurf des APIs

Die von mir vorgestellt Klasse `ThreadsafeQueue<T>`
besitzt folgende öffentliche Schnittstelle:

```cpp
01: template <typename T>
02: class ThreadsafeQueue
03: {
04: public:
05:     ThreadsafeQueue();
06:     ThreadsafeQueue(const ThreadsafeQueue & other);
07:     ThreadsafeQueue(ThreadsafeQueue && other) noexcept;
08:     ThreadsafeQueue & operator= (const ThreadsafeQueue& other);
09:     ThreadsafeQueue & operator= (ThreadsafeQueue&& other) noexcept;
10:     void push(const T & value);
11:     void push(T && value);
12:     bool tryPop(T& value);
13:     std::optional<T> tryPop();
14:     void waitAndPop(T& value);
15:     bool empty() const;
16:     size_t size() const;
17: };
```

Das Hinzufügen eines Elements zur Warteschlange (Methode `push`) kann als trivial betrachtet werden,
auch vor dem Hintergrund der *Concurrency*. Es gibt diese Methode in zwei Ausprägungen,
um das Verschieben von temporären Objekten laufzeitoptimal ausführen zu können.

Desweiteren gilt es in der Realisierung eine Feinheit zu beachten:

```cpp
01: void push(const T& value)
02: {
03:     std::unique_lock<std::mutex> guard{ m_mutex };
04:     m_data.push(value);
05:     guard.unlock();
06:     m_condition.notify_one();
07: }
```

Die kritische Datenstruktur `m_data` wird durch ein Hüllenobjekt des Typs `std::unique_lock` geschützt.
Das Benachrichtigen von Clients, die auf den Zustand &bdquo;Warteschlange ist nicht mehr leer&rdquo; warten,
muss aber *nicht* im kritischen Abschnitt stattfinden.

Aus diesem Grund ist das Hüllenobjekt vom Typ `std::unique_lock`,
es steht dann eine Methode `unlock` (siehe Zeile 5) zur Freigabe des kritischen Abschnitts zur Verfügung.

Die `pop`-Operation ist auf Grund von *Race Conditions* nicht ganz einfach zu definieren.
Drei mögliche Signaturen sind

```cpp
bool tryPop(T& value);
```

oder 

```cpp
std::optional<T> tryPop();
```

oder 

```cpp
void waitAndPop(T& value);
```

Von der Laufzeit her gesehen bedeutet das zunächst, dass die beiden Überladungen der `tryPop`-Methode
nicht warten, bei Erfolg den Wert `true`
zurückliefern (auf direkte Weise oder in einem `std::optional<T>`-Objekt)
und der gesuchte Wert im Argument des Referenz-Parameters `value` bzw. im `std::optional<T>`-Objekt abgelegt ist.

Die Methode `waitAndPop` hingegen blockiert solange, bis in der Warteschlange ein Wert vorhanden ist.


### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeQueue<T>`,
so können wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &bdquo;umschlossene&rdquo; `std::queue<T>`-Objekt zu schützen:


```cpp
001: template<typename T>
002: class ThreadsafeQueue
003: {
004: private:
005:     std::queue<T>            m_data;
006:     mutable std::mutex       m_mutex;
007:     std::condition_variable  m_condition;
008: 
009: public:
010:     ThreadsafeQueue() {}
011: 
012:     // copy and move constructor may be useful
013:     ThreadsafeQueue(const ThreadsafeQueue& other)
014:     {
015:         std::lock_guard<std::mutex> guard{ other.m_mutex };
016:         m_data = other.m_data;
017:     }
018: 
019:     ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
020:     {
021:         std::lock_guard<std::mutex> guard{ other.m_mutex };
022:         m_data = std::move(other.m_data);
023:     }
024: 
025:     ThreadsafeQueue& operator= (const ThreadsafeQueue& other)
026:     {
027:         if (&other == this)
028:             return *this;
029: 
030:         std::scoped_lock<std::mutex> guard{ m_mutex, other.m_mutex };
031:         m_data = other.m_data;
032:         return *this;
033:     }
034: 
035:     ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept
036:     {
037:         if (&other == this)
038:             return *this;
039: 
040:         std::scoped_lock<std::mutex> guard{ m_mutex, other.m_mutex };
041:         m_data = std::move (other.m_data);
042:         return *this;
043:     }
044: 
045:     void push(const T& value)
046:     {
047:         std::unique_lock<std::mutex> guard{ m_mutex };
048:         m_data.push(value);
049:         guard.unlock();
050:         m_condition.notify_one();
051:     }
052: 
053:     void push(T&& value)
054:     {
055:         std::unique_lock<std::mutex> guard{ m_mutex };
056:         m_data.push(std::move(value));
057:         guard.unlock();
058:         m_condition.notify_one();
059:     }
060: 
061:     bool tryPop(T& value)
062:     {
063:         std::lock_guard<std::mutex> guard{ m_mutex };
064:         if (m_data.empty()) {
065:             return false;
066:         }
067:         else {
068:             value = std::move(m_data.front());
069:             m_data.pop();
070:             return true;
071:         }
072:     }
073: 
074:     std::optional<T> tryPop()
075:     {
076:         std::lock_guard<std::mutex> guard{ m_mutex };
077:         if (m_data.empty()) {
078:             return std::optional<T>(std::nullopt);
079:         }
080:         else {
081:             std::optional<T> result{ std::move(m_data.front()) };
082:             m_data.pop();
083:             return result;
084:         }
085:     }
086: 
087:     void waitAndPop(T& value)
088:     {
089:         std::unique_lock<std::mutex> guard{ m_mutex };
090:         m_condition.wait(guard, [this]() {
091:             return !m_data.empty();
092:             }
093:         );
094: 
095:         value = std::move(m_data.front());
096:         m_data.pop();
097:     }
098: 
099:     bool empty() const
100:     {
101:         std::lock_guard<std::mutex> guard{ m_mutex };
102:         return m_data.empty();
103:     }
104: 
105:     size_t size() const
106:     {
107:         std::lock_guard<std::mutex> guard{ m_mutex };
108:         return m_data.size();
109:     }
110: };
```

Wir erkennen an der Realisierung,
dass alle Zugriffe auf das zugrunde liegende `std::queue<T>`-Objekt
mithilfe eines Hüllenobjekts (RAII-Stil Mechanismus) geschützt werden,
um auf diese Weise sicherzustellen, dass vorhandene Sperren vom haltenden Thread
in allen möglichen Exit-Szenarien (einschließlich Ausnahmen) wieder aufgehoben werden.

Wir demonstrieren in der Realisierung der `ThreadsafeQueue<T>`-Klasse gleich drei Hüllenklassen:


* `std::lock_guard`<br />Kommt dann zum Einsatz, wenn keine besonderen Anforderungen vorhanden sind.
* `std::unique_lock`<br />Wird dann benötigt, wenn die `wait`-Methode an einem `std::condition_variable`-Objekt aufgerufen werden soll. 
* `std::scoped_lock`<br />Wird in den Kopier-Konstruktoren und in den Wertzuweisungsoperatoren eingesetzt, um das Sperren von zwei Mutex-Objekten zu ermöglichen.


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

#### Ein Beispiel

Im Beispiel zu diesem Abschnitt finden Sie zwei Funktionen `produce` und `consume` vor.
Beide Funktionen greifen auf ein `ThreadsafeQueue<int>`-Objekt zu.
Führen wir beide Funktionen im Kontext unterschiedlicher Threads aus,
werden die Methoden `push` und `waitAndPop` nebenläufig ausgeführt.

  * Studieren Sie die Ausgaben des Programms.
  * Überzeugen Sie sich von der Korrektheit der Ergebnisse.


*Ausgabe*:


```
[1]:    --> 1
[1]:    --> 2
[1]:    --> 3
[2]:            1 <== Consumer [1]
[3]:            2 <== Consumer [3]
[1]:    --> 4
[4]:            3 <== Consumer [4]
[5]:            4 <== Consumer [5]
[1]:    --> 5
[1]:    --> 6
[6]:            5 <== Consumer [2]
[2]:            6 <== Consumer [1]
[1]:    --> 7
[1]:    --> 8
[3]:            7 <== Consumer [3]
[4]:            8 <== Consumer [4]
[1]:    --> 9
[1]:    --> 10
[1]:    --> 11
[5]:            9 <== Consumer [5]
[6]:            10 <== Consumer [2]
[1]:    --> 12
[1]:    --> 13
[3]:            12 <== Consumer [3]
[2]:            11 <== Consumer [1]
[1]:    --> 14
[1]:    --> 15
[4]:            13 <== Consumer [4]
[6]:            15 <== Consumer [2]
[5]:            14 <== Consumer [5]
[1]:    --> 16
[1]:    --> 17
[1]:    --> 18
[3]:            16 <== Consumer [3]
[2]:            17 <== Consumer [1]
[1]:    --> 19
[4]:            18 <== Consumer [4]
[1]:    --> 20
[6]:            19 <== Consumer [2]
[5]:            20 <== Consumer [5]
```

oder auch

```
[1]: 	--> 1
[1]: 	--> 2
[1]: 	--> 3
[1]: 	--> 4
[1]: 	--> 5
[1]: 	--> 6
[1]: 	--> 7
[1]: 	--> 8
[1]: 	--> 9
[1]: 	--> 10
[1]: 	--> 11
[1]: 	--> 12
[1]: 	--> 13
[1]: 	--> 14
[2]: 	        1 <== Consumer [2]
[2]: 	        4 <== Consumer [2]
[2]: 	        5 <== Consumer [2]
[2]: 	        6 <== Consumer [2]
[3]: 	        7 <== Consumer [4]
[3]: 	        9 <== Consumer [4]
[3]: 	        10 <== Consumer [4]
[4]: 	        2 <== Consumer [5]
[3]: 	        11 <== Consumer [4]
[1]: 	--> 15
[1]: 	--> 16
[4]: 	        12 <== Consumer [5]
[1]: 	--> 17
[4]: 	        13 <== Consumer [5]
[1]: 	--> 18
[4]: 	        14 <== Consumer [5]
[1]: 	--> 19
[1]: 	--> 20
[5]: 	        8 <== Consumer [3]
[5]: 	        15 <== Consumer [3]
[5]: 	        16 <== Consumer [3]
[6]: 	        3 <== Consumer [1]
[5]: 	        17 <== Consumer [3]
[6]: 	        18 <== Consumer [1]
[6]: 	        19 <== Consumer [1]
[6]: 	        20 <== Consumer [1]
```


Zu dieser Ausgabe gehört das folgende Beispielprogramm:


```cpp
01: void test()
02: {
03:     using namespace Concurrency_ThreadsafeQueue;
04: 
05:     ThreadsafeQueue<size_t> queue;
06: 
07:     auto produce = [&] () {
08:         for (int i{ 1 }; i <= NumToProduce; ++i) {
09:             Logger::log(std::cout, "--> ", i);
10:             queue.push(i);
11:         }
12:     };
13: 
14:     auto consume = [&](size_t id) {
15:         for (size_t i{}; i != NumToConsume; ++i) {
16:             size_t value{};
17:             queue.waitAndPop(value);
18:             Logger::log(std::cout, "        ", value, " <== Consumer [", id, ']');
19:         }
20:     };
21: 
22:     std::thread producer{ produce };
23: 
24:     std::vector<std::thread> consumers;
25: 
26:     for (size_t i{}; i != NumConsumers; ++i) {
27: 
28:         std::thread consumer{ consume, i + 1 };
29:         consumers.push_back(std::move(consumer));
30:     }
31:     
32:     producer.join();
33: 
34:     for (auto& consumer : consumers) {
35:         consumer.join();
36:     }
37: }
```

---

#### Quellcode

[*ThreadsafeQueue.h*](ThreadsafeQueue.h).<br />
[*Examples.cpp*](Examples.cpp).<br />

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Buch

[C++ Concurrency in Action](https://www.cplusplusconcurrencyinaction.com/) von Anthony Williams.

Kleinere Ergänzungen wurden in Abstimmung mit der Unterlage

[Concurrency with Modern C++](https://www.grimm-jaud.de/index.php/concurrency-with-modern-c/) von Rainer Grimm

vorgenommen.

Weitere Anregungen zur Entwicklung einer threadsicheren Warteschlange kann man auch in
[Modern C++: Writing a thread-safe Queue](https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue/comment-page-1/)
nachlesen.

Das Praxisbeispiel mit Produzenten und Konsumenten stammt aus
[Juan's C++ Block &ndash; Concurrent Queue &ndash; C++ 11](https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/).

---

[Zurück](../../Readme.md)

---
