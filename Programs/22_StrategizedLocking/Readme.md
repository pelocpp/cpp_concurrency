# Strategisches Locking (*Strategized Locking*)

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [*Runtime* versus *Compile-Time*](#link3)
  * [Wann sollte dieses Muster verwendet werden?](#link4)
  * [Entwurf des APIs](#link5)
  * [Hüllenklasse `StrategizedLocking`](#link6)
  * [Mögliche Realisierungen der `ILock`-Schnittstelle](#link7)
  * [Ein Beispiel: Klasse `ThreadsafeStack<T>`](#link8)
  * [Literaturhinweise](#link9)

---

## Verwendete Werkzeuge <a name="link1"></a>

  * Klassen `std::mutex` und `std::recursive_mutex`
  * Klasse `std::thread`
  * Klasse `std::shared_lock`

---

#### Quellcode

[*ILock.h*](ILock.h).<br />
[*StrategizedLock.h*](StrategizedLock.h).<br />
[*ThreadsafeStack.h*](ThreadsafeStack.h).<br />
[*PrimeCalculator.h*](PrimeCalculator.h).<br />

[*StrategizedLock.cpp*](StrategizedLock.cpp).<br />
[*PrimeNumbers.cpp*](PrimeNumbers.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Das *Strategized Locking*&ndash;Entwurfsmuster
schützt den kritischen Abschnitt einer Komponente vor gleichzeitigem (konkurriendem) Zugriff.

Mit anderen Worten: Wenn ein Objekt Daten benötigt, die in einem anderen Thread erstellt werden,
müssen die kritischen Abschnitte für den Datenzugriff gesperrt werden.

Hierbei können sich allerdings Unterschiede in der Notwendigkeit des Datenschutzes ergeben.

Laufen die kritischen Routinen (Abschnitte) in einer *Single-Threaded*-Umgebung,
ist ein Datenschutz überhaupt nicht erforderlich und es entsteht ein Performanzproblem,
wenn laufzeitintensive Synchronisationsmechanismen zum Einsatz kommen,
die hier unnötig sind.

Werden die kritischen Abschnitte hingegen im Kontext mehrerer Threads ausgeführt (*Multi-Threaded*-Umgebung),
müssen diese geschützt werden.

An dieser Stelle bietet sich das *Strategized Locking*&ndash;Entwurfsmuster an:

> Die Strategie des Lockings wird in einem Objekt untergebracht,
das Programm selbst greift nur über eine definierte Schnittstelle auf das Strategie-Objekt zu.
Das Strategie-Objekt hat Kenntnisse über den Kontext, in dem das Programm abläuft.


## *Runtime* versus *Compile-Time* <a name="link3"></a>

Das *Strategized Locking*&ndash;Entwurfsmuster kann sowohl  in einer *Runtime*-Ausprägung
(Vererbung, Polymorphismus) als auch in einer *Compile-Time*-Ausprägung (Templates) umgesetzt werden.

## Wann sollte dieses Muster verwendet werden? <a name="link4"></a>

Verwenden Sie dieses Muster, wenn

  * Sie einen kritischen Abschnitt schützen müssen.
  * Sie konsistent zur Verwendung eines `std::mutex-Objeks` sein möchten.
  * das Programm sowohl in Single-Threaded- als auch in Multi-Threaded-Umgebungen performant ablaufen soll.


## Entwurf des APIs <a name="link5"></a>

Die Herstellung der Konsistenz in Bezug auf die Verwendung eines `std::mutex`-Objekts
erfolgt durch eine Schnittstelle `ILock`:


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

## Hüllenklasse `StrategizedLocking` <a name="link6"></a>

Die kritischen Abschnitte schützen sich nicht direkt mit einem `std::mutex`-Objekt,
sondern einer Wrapper-Klasse `StrategizedLocking`:


```cpp
01: class StrategizedLocking 
02: {
03: private:
04:     const ILock& m_lock;
05: 
06: public:
07:     StrategizedLocking(const ILock&) : m_lock(lock) {
08:         m_lock.lock();
09:     }
10: 
11:     ~StrategizedLocking() {
12:         m_lock.unlock();
13:     }
14: };
```

Wir erkennen, dass Aufrufe einer `lock`- und `unlock`-Methode im Konstruktor bzw. Destruktor der
`StrategizedLocking`-Klasse durchgeführt werden.

Wer führt diese Methodenaufrufe konkret durch?
Das entscheidet der Ersteller eines `StrategizedLocking`-Objekts, in dem er bei der Objekterzeugung (Konstruktor)
eine Realisierung der `ILock`-Schnittstelle bereitstellt.


## Mögliche Realisierungen der `ILock`-Schnittstelle <a name="link7"></a>

Wir betrachten zwei Realisierungen der `ILock`-Schnittstelle:


```cpp
01: struct NullObjectMutex
02: {
03:     void lock() {}
04:     void unlock() {}
05: };
06: 
07: class NoLock : public ILock
08: {
09: private:
10:     mutable NullObjectMutex m_nullMutex;
11: 
12: public:
13:     void lock() const const {
14:         m_nullMutex.lock();
15:     }
16: 
17:     void unlock() const const {
18:         m_nullMutex.unlock();
19:     }
20: };
21: 
22: class ExclusiveLock : public ILock
23: {
24: private:
25:     mutable std::mutex m_mutex;
26: 
27: public:
28:     void lock() const const {
29:         m_mutex.lock();
30:     }
31: 
32:     void unlock() const  const {
33:         m_mutex.unlock();
34:     }
35: };
```


## Ein Beispiel: Klasse `ThreadsafeStack<T>` <a name="link8"></a>

Betrachten wir eine Anpassung der Klasse `ThreadsafeStack<T>` an
das *Strategized Locking*&ndash;Entwurfsmuster:

```cpp
01: template<typename T>
02: class ThreadsafeStack
03: {
04: private:
05:     std::stack<T> m_data;
06:     const ILock& m_lock;
07: 
08: public:
09:     // c'tors
10:     ThreadsafeStack(const ILock& lock) : m_lock{lock} {}
11: 
12:     // public interface
13:     void push(const T& value)
14:     {
15:         StrategizedLocking m_guard{ m_lock };
16:         m_data.push(value);
17:     }
18: 
19:     void pop(T& value)
20:     {
21:         StrategizedLocking m_guard{ m_lock };
22:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
23:         value = m_data.top();
24:         m_data.pop();
25:     }
26: 
27:     T tryPop()
28:     {
29:         StrategizedLocking m_guard{ m_lock };
30:         if (m_data.empty()) throw std::out_of_range{ "Stack is empty!" };
31:         T value = m_data.top();
32:         m_data.pop();
33:         return value;
34:     }
35: 
36:     size_t size() const
37:     {
38:         StrategizedLocking m_guard{ m_lock };
39:         return m_data.size();
40:     }
41: 
42:     bool empty() const
43:     {
44:         StrategizedLocking m_guard{ m_lock };
45:         return m_data.empty();
46:     }
47: };
```


---

## Literaturhinweise <a name="link9"></a>

Die Anregungen zu diesen Erläuterungen stammen im Wesentlichen aus der Unterlage

[Concurrency with Modern C++](https://www.grimm-jaud.de/index.php/concurrency-with-modern-c/) von Rainer Grimm.

---

[Zurück](../../Readme.md)

---
