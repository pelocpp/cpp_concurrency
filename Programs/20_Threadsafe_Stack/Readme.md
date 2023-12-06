# Threadsicherer Stapel (Threadsafe Stack)

[Zur�ck](../../Readme.md)

---


### Allgemeines

Die STL stellt mit der Klasse `std::queue<T>` einen leistungsstarken LIFO-Conainer
(*Last-in, First-out Data Structure*) bereit.

Der Zugriff von mehreren Threads aus auf Objekte dieser Klasse ist jedoch nicht sicher.



### Race Conditions

Unter einer *Race Condition*
versteht man im Umfeld der *Concurrency* die Eigenschaft,
dass Ergebnisse einer Programmausf�hrung davon abh�ngen,
in welcher Reihenfolge die Ausf�hrung von Operationen in zwei oder mehreren Threads stattfindet.

Nicht immer muss diese Beobachtnung fehlerhaft sein.
Wenn beispielsweise zwei Threads Elemente in einem Stapel zur
weiteren Verarbeitung ablegen,
spielt es keine Rolle, welches Element zuerst hinzugef�gt wird.

Problematisch sind *Race Condition* dann, wenn sie *Invarianten* brechen.
Eine Invariante am Beispiel eines Stapels betrachtet k�nnte sein,
dass dieser beispielsweise zu einem bestimmten Zeitpunkt &ldquo;nicht leer&rdquo; ist.
Dies k�nnte das Resultat eines Methodenaufrufs wie etwa `empty` sein.
Wird aber zum gleichen Zeitpunkt auf Grund der konkurrierenden Ausf�hrung einer `pop`-Methode
in einem anderen Thread der Stapel geleert, kommt es in dem Thread,
der sich auf den Aufruf von `empty` verlassen m�chte,
zu einem Bruch der Invariante. 


### Entwurf des APIs

Die von mir vorgestellt Klasse `ThreadsafeStack<T>`
besitzt folgende �ffentliche Schnittstelle:

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

Das Hinzuf�gen eines Elements zum Stapel kann als trivial betrachtet werden,
auch vor dem Hintergrund der *Concurrency*.

Problematischer sieht es mit der Pop-Operation aus:
Hier ist es &ndash; auf Grund von *Race Conditions* &ndash; nicht ganz einfach,
entsprechende Methodensignaturen zu definieren:

Zwei m�gliche Signaturen sind

```cpp
void pop(T& value);
T tryPop();
```

Beide Methoden m�ssen allerdings eine Ausnahme werfen, wenn der Stapel leer ist.

M�chte man das Exception-Handling umgehen, ist das Datentyp `std::optional<T>`
eine Option:

```cpp
std::optional<T> tryPopOptional()
```

### Thread Sicherheit

Betrachten wir die Realisierung der Klasse `ThreadsafeStack<T>`,
so k�nnen wir erkennen,
dass nahezu alle Methoden ein `std::mutex`-Objekt verwenden,
um den Zugriff auf das &ldquo;umschlossene&rdquo; `std::stack<T>`-Objekt zu sch�tzen:


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
mithilfe des `std:lock_guard`-Musters (RAII-Stil Mechanismus) zu sch�tzen,
um sicherzustellen, dass die Sperre vom haltenden Thread
in allen m�glichen Exit-Szenarien (einschlie�lich Ausnahme) aufgehoben wird.


### Schl�sselwort `mutable`

In Zeile 6 der Realisierung wird das `std::mutex`-Objekt mit dem Schl�sselwort `mutable`
versehen. Prinzipiell ist dies erforderlich, um die beiden Methoden `size` und `empty`
wiederum mit `const` markieren zu k�nnen.

Interessanterweise l�sst sich die gezeigte Realisierung auch ohne Verwendung von `mutable` �bersetzen &ndash;
bei Gebrauch des *Visual C++* Compilers.

Der Gcc-Compiler reagiert an dieser Stelle empfindlicher:

```
error: binding reference of type 'std::lock_guard<std::mutex>::mutex_type&' {aka 'std::mutex&'} to 'const std::mutex' 
```

---

#### Ein erstes Beispiel

To be Done

Semaphore k�nnen in Sender-Empf�nger-Abl�ufen verwendet werden.

Wird zum Beispiel eine Semaphore auf 0 initialisiert, blockiert der Empf�nger-Aufruf `acquire`,
bis der Sender `release` ausf�hrt.
Damit wartet der Empf�nger auf die Benachrichtigung des Senders:

```cpp
```

*Ausgabe*:


```
```

Die einmalige Synchronisation von Threads l�sst sich einfach mit Semaphoren umsetzen:

Das `std::binary_semaphore`-Objekt `m_semaphore` (Zeile 4) kann die Werte 0 oder 1 besitzen.

Im konkreten Anwendungsfall wird es auf `0` (Zeile 7) initialisiert.

Das hei�t, dass der Aufruf `release` den Wert auf 1 (Zeile 19) setzt
und den Aufruf `acquire` in Zeile 28 entblockt.

#### Quellcode:

[TestPrimeNumbers.cpp](TestPrimeNumbers.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Buch

[C++ Concurrency in Action](https://www.cplusplusconcurrencyinaction.com/) von Anthony Williams.

Kleinere Erg�nzungen wurden in Abstimmung mit der Unterlage

[Concurrency with Modern C++](https://www.grimm-jaud.de/index.php/concurrency-with-modern-c/) von Rainer Grimm

vorgenommen.

---

[Zur�ck](../../Readme.md)

---
