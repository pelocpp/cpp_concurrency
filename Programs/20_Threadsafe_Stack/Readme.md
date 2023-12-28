# Threadsicherer Stapel (Threadsafe Stack)

[Zurück](../../Readme.md)

---

# Verwendete Hilfsmittel:

  * `std::mutex` und `std::lock_guard`
  * `std::thread`

---

### Allgemeines

Die STL stellt mit der Klasse `std::stack<T>` einen leistungsstarken LIFO-Container
(*Last-in, First-out* Datenstruktur) bereit.

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
05:     void push(T new_value);
06:     void pop(T& value);
07:     T tryPop();
08:     std::optional<T> tryPopOptional();
09:     size_t size() const;
10:     bool empty() const;
11: };
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
13:     ThreadsafeStack& operator = (const ThreadsafeStack&) = delete;
14:     ThreadsafeStack& operator = (ThreadsafeStack&&) noexcept = delete;
15: 
16:     // copy and move constructor may be useful
17:     ThreadsafeStack(const ThreadsafeStack& other) noexcept
18:     {
19:         std::lock_guard<std::mutex> lock(other.m_mutex);
20:         m_data = other.m_data;
21:     }
22:         
23:     ThreadsafeStack(const ThreadsafeStack&& other) noexcept
24:     {
25:         std::lock_guard<std::mutex> lock(other.m_mutex);
26:         m_data = std::move(other.m_data);
27:     }
28: 
29:     // public interface
30:     void push(T new_value)
31:     {
32:         std::lock_guard<std::mutex> lock{ m_mutex };
33:         m_data.push(new_value);
34:     }
35: 
36:     template<class... TArgs>
37:     void emplace(TArgs&&... args)
38:     {
39:         std::lock_guard<std::mutex> lock{ m_mutex };
40:         m_data.emplace(std::forward<TArgs>(args) ...);
41:     }
42: 
43:     void pop(T& value)
44:     {
45:         std::lock_guard<std::mutex> lock{ m_mutex };
46:         if (m_data.empty()) throw empty_stack{};
47:         value = m_data.top();
48:         m_data.pop();
49:     }
50: 
51:     T tryPop()
52:     {
53:         std::lock_guard<std::mutex> lock{ m_mutex };
54:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
55:         T value = m_data.top();
56:         m_data.pop();
57:         return value;
58:     }
59: 
60:     std::optional<T> tryPopOptional()
61:     {
62:         std::lock_guard<std::mutex> lock{ m_mutex };
63:         if (m_data.empty()) {
64:             return std::nullopt;
65:         }
66: 
67:         std::optional<T> result{ m_data.top() };
68:         m_data.pop();
69:         return result;
70:     }
71: 
72:     size_t size() const
73:     {
74:         std::lock_guard<std::mutex> lock{ m_mutex };
75:         return m_data.size();
76:     }
77: 
78:     bool empty() const
79:     {
80:         std::lock_guard<std::mutex> lock{ m_mutex };
81:         return m_data.empty();
82:     }
83: };
```

Wir erkennen an der Realisierung,
dass alle Zugriffe auf das zugrunde liegende `std::stack<T>`-Objekt
mithilfe des `std:lock_guard`-Musters (RAII-Stil Mechanismus) geschützt werden,
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

To be Done

Semaphore können in Sender-Empfänger-Abläufen verwendet werden.

Wird zum Beispiel eine Semaphore auf 0 initialisiert, blockiert der Empfänger-Aufruf `acquire`,
bis der Sender `release` ausführt.
Damit wartet der Empfänger auf die Benachrichtigung des Senders:

```cpp
```

*Ausgabe*:


```
```

Die einmalige Synchronisation von Threads lässt sich einfach mit Semaphoren umsetzen:

Das `std::binary_semaphore`-Objekt `m_semaphore` (Zeile 4) kann die Werte 0 oder 1 besitzen.

Im konkreten Anwendungsfall wird es auf `0` (Zeile 7) initialisiert.

Das heißt, dass der Aufruf `release` den Wert auf 1 (Zeile 19) setzt
und den Aufruf `acquire` in Zeile 28 entblockt.

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
