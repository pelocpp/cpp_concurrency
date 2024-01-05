# Threadsichere Warteschlange (Threadsafe Queue)

[Zurück](../../Readme.md)

---

# Verwendete Hilfsmittel:

  * `std::mutex`
  * `std::lock_guard`, `std::unique_lock` und `std::scoped_lock`
  * `std::condition_variable`
  * `std::thread`

---

### Allgemeines

Die STL stellt mit der Klasse `std::queue<T>` einen leistungsstarken FIFO-Container bereit
(*First-in, First-out* Datenstruktur).

Der Zugriff von mehreren Threads aus auf Objekte dieser Klasse ist jedoch nicht sicher.


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
01: template<typename T>
02: class ThreadsafeQueue
03: {
04: public:
05:     ThreadsafeQueue() {}
06:     ThreadsafeQueue(const ThreadsafeQueue& other);
07:     ThreadsafeQueue(ThreadsafeQueue&& other) noexcept;
08:     ThreadsafeQueue& operator= (const ThreadsafeQueue& other);
09:     ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept;
10:     void push(const T& value);
11:     void push(T&& value);
12:     void wait_and_pop(T& value);
13:     bool try_pop(T& value);
14:     bool empty() const;
15:     size_t size() const;
16: };
```

Das Hinzufügen eines Elements zur Warteschlange (Methode `push`) kann als trivial betrachtet werden,
auch vor dem Hintergrund der *Concurrency*. Es gibt diese Methode in zwei Ausprägungen,
um das Verschieben von temporären Objekten laufzeitoptimal ausführen zu können.

Desweiteren gilt es in der Realisierung eine Feinheit zu beachten:

```cpp
01: void push(const T& value)
02: {
03:     std::unique_lock<std::mutex> lock{ m_mutex };
04:     m_data.push(value);
05:     lock.unlock();
06:     m_condition.notify_one();
07: }
```

Die kritische Datenstruktur `m_data` wird durch ein Hüllenobjekt des Typs `std::unique_lock` geschützt.
Das Benachrichtigen von Clients, die auf den Zustand &bdquo;Warteschlange ist nicht mehr leer&rdquo; warten,
muss aber *nicht* im kritischen Abschnitt stattfinden.

Aus diesem Grund ist das Hüllenobjekt vom Typ `std::unique_lock`,
es steht dann eine Methode `unlock` (siehe Zeile 5) zur Freigabe des kritischen Abschnitts zur Verfügung.

Die `pop`-Operation ist auf Grund von *Race Conditions* nicht ganz einfach zu definieren.
Zwei mögliche Signaturen sind

```cpp
void waitAndPop(T& value);
```

oder 

```cpp
bool tryPop(T& value);
```

Von der Laufzeit her gesehen bedeutet das zunächst, dass `tryPop` nicht wartet, bei Erfolg den Wert `true`
zurückliefert und der gesuchte Wert im Argument des Referenz-Parameters `value` abgelegt ist.

Die Methode `waitAndPop` hingegen blockiert solange, bis in der Warteschlange ein Wert vorhanden ist.


### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeQueue<T>`,
so können wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &bdquo;umschlossene&rdquo; `std::queue<T>`-Objekt zu schützen:


```cpp
01: template<typename T>
02: class ThreadsafeQueue
03: {
04: private:
05:     std::queue<T>            m_data;
06:     mutable std::mutex       m_mutex;
07:     std::condition_variable  m_condition;
08: 
09: public:
10:     ThreadsafeQueue() {}
11: 
12:     // copy and move constructor may be useful
13:     ThreadsafeQueue(const ThreadsafeQueue& other)
14:     {
15:         std::lock_guard<std::mutex> lock{ other.m_mutex };
16:         m_data = other.m_data;
17:     }
18: 
19:     ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
20:     {
21:         std::lock_guard<std::mutex> lock{ other.m_mutex };
22:         m_data = std::move(other.m_data);
23:     }
24: 
25:     ThreadsafeQueue& operator= (const ThreadsafeQueue& other)
26:     {
27:         if (&other == this)
28:             return *this;
29: 
30:         std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };
31:         m_data = other.m_data;
32:         return *this;
33:     }
34: 
35:     ThreadsafeQueue& operator= (ThreadsafeQueue&& other) noexcept
36:     {
37:         if (&other == this)
38:             return *this;
39: 
40:         std::scoped_lock<std::mutex> lock{ m_mutex, other.m_mutex };
41:         m_data = std::move (other.m_data);
42:         return *this;
43:     }
44: 
45:     void push(const T& value)
46:     {
47:         std::unique_lock<std::mutex> lock{ m_mutex };
48:         m_data.push(value);
49:         lock.unlock();
50:         m_condition.notify_one();
51:     }
52: 
53:     void push(T&& value)
54:     {
55:         std::unique_lock<std::mutex> lock{ m_mutex };
56:         m_data.push(std::move(value));
57:         lock.unlock();
58:         m_condition.notify_one();
59:     }
60: 
61:     void waitAndPop(T& value)
62:     {
63:         std::unique_lock<std::mutex> lock{ m_mutex };
64:         m_condition.wait(lock, [this] () {
65:             return !m_data.empty(); 
66:             }
67:         );
68: 
69:         value = m_data.front();
70:         m_data.pop();
71:     }
72: 
73:     bool tryPop(T& value)
74:     {
75:         std::lock_guard<std::mutex> lock{ m_mutex };
76:         if (m_data.empty()) {
77:             return false;
78:         }
79: 
80:         value = m_data.front();
81:         m_data.pop();
82:         return true;
83:     }
84: 
85:     bool empty() const
86:     {
87:         std::lock_guard<std::mutex> lock{ m_mutex };
88:         return m_data.empty();
89:     }
90: 
91:     size_t size() const
92:     {
93:         std::lock_guard<std::mutex> lock{ m_mutex };
94:         return m_data.size();
95:     }
96: };
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
01: void example()
02: {
03:     ThreadsafeQueue<int> queue;
04: 
05:     std::thread producer{ produce, std::ref(queue) };
06: 
07:     std::vector<std::thread> consumers;
08: 
09:     for (int i{}; i != NumConsumers; ++i) {
10: 
11:         std::thread consumer{ consume, std::ref(queue), i + 1 };
12:         consumers.push_back(std::move(consumer));
13:     }
14: 
15:     producer.join();
16: 
17:     for (auto& consumer : consumers) {
18:         consumer.join();
19:     }
20: }
```

---

#### Quellcode:

[ThreadsafeQueue.h](ThreadsafeQueue.h).<br />
[Examples.cpp](Examples.cpp).<br />

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
