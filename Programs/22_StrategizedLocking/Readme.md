# Strategisches Locking (Strategized Locking)

[Zurück](../../Readme.md)

---

### Allgemeines

Das *Strategized Locking*&ndash;Entwurfsmuster
schützt den kritischen Abschnitt einer Komponente vor gleichzeitigem (konkurriendem) Zugriff.

Mit anderen Worten: Wenn ein Objekt Daten benötigt, die in einem anderen Thread erstellt werden,
müssen die kritischen Abschnitte für den Datenzugriff gesperrt werden.

Hierbei können sich allerdings Unterschiede in der Notwendigkeit des Datenschutzes ergeben.

Laufen die kritischen Routinen (Abschnitte) in einer Single-Thread-Umgebung,
ist ein Datenschutz überhaupt nicht erforderlich und es entsteht ein Performanzproblem,
da ein laufzeitintensiver Synchronisationsmechanismus zum Einsatz kommt,
der an dieser Stelle unnötig ist.

Werden die kritischen Abschnitte hingegen im Kontext mehrerer Threads ausgeführt,
müssen die kritischen Abschnitte geschützt werden.

An dieser Stelle bietet sich das Strategized Locking Entwurfsmuster an:

Die Strategie des Lockings wird in einem Objekt untergebracht,
das Programm selbst greift nur über eine definierte Schnittstelle auf das Strategie-Objekt zu.


### *Runtime* versus *Compile-Time*

Das *Strategized Locking*&ndash;Entwurfsmuster kann sowohl  in einer *Runtime*-Ausprägung
(Vererbung, Polymorphismus) als auch in einer *Compile-Time*-Ausprägung umgesetzt werden.

### Wann sollte dieses Muster verwendet werden?

Verwenden Sie dieses Muster, wenn:

  * Sie müssen einen kritischen Abschnitt schützen
  * Sie möchten konsistent zur Verwendung eines `std::mutex-Objeks` sein
  * Der Quellcode soll sowohl in Single-Threaded- als auch in Multi-Threaded-Umgebungen laufen
  * Das Programm soll in jeder Umgebung performant ablaufen.


### Entwurf des APIs

Die von mir vorgestellt Klasse `ThreadsafeStack<T>`
besitzt folgende öffentliche Schnittstelle:

```cpp
01: class ILock
02: {
03: public:
04:     ~ILock() {}
05: 
06:     virtual void lock() const = 0;
07:     virtual void unlock() const = 0;
08: };
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

#### Quellcode:

[TestPrimeNumbers.cpp](TestPrimeNumbers.cpp).

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
