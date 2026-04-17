# Nochmals nebenläufige Ausführung: Funktion `std::async`

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::future`

<ins>Funktionen</ins>:

  * Funktion `std::async`

<ins>Aufzählungstypen</ins>:

  * Aufzählungstyp `enum class launch`

---

## Allgemeines

Das Zusammenspiel zwischen der Methode `std::async` und Objekten
des Typs `std::future` wird gezeigt.

Die `std::async`-Methode besitzt im Wesentlichen drei Überladungen:

  * `std::async` mit normaler C-Funktion.
  * `std::async` mit aufrufbarem Objekt.
  * `std::async` mit Lambda-Objekt.

#### Quellcode

[*Async_01.cpp*](Async_01.cpp)<br/>
[*Async_02.cpp*](Async_02.cpp)<br/>
[*Async_03.cpp*](Async_03.cpp)

---

## `std::async` und Thread-Pool

In Bezug auf `std::async` können wir die allgemeine Aussage treffen,
dass die Funktion eine Funktion in einem separaten Thread startet,
wenn keine *launch policy* spezifiziert ist.

Der C++-Standard gibt jedoch nicht an, ob der Thread neu ist (`std::thread`)
oder aus einem Thread-Pool wiederverwendet wird. 

In Bezug auf das Windows Betriebssystem können wir jedoch aus verfügbaren Quellen entnehmen,
dass das Erzeugen des Threads mit einem Aufruf von `::Concurrency::create_task` erfolgt.

Diese Funktion ist Teil der *Parallel Patterns Library* von Microsoft.
Laut MSDN verwendet die `task`-Klasse den Windows ThreadPool als Scheduler!

Weitere Details finden sich in einem Artikel *[Implementations of std::async](https://ddanilov.me/std-async-implementations/)*
von Dmitry Danilov.

In den Übungen zu diesem Repository finden Sie in der dritten Aufgabe einen Vergleich
zwischen dem Arbeiten mit &bdquo;normalen&bdquo; Threads und solchen Threads,
die aus einem Thread Pool stammen.

---

## *Launch Policies*: `launch::async` versus `launch::deferred`

Es werden zwei Threads mit `std::async` gestartet, die beide einen Zeitstempel zurückliefern.

Sie unterscheiden sich bzgl. der verwendeten *Launch Policy*:

  * `launch::async`<br/>Startet einen eigenen Thread im Hintergrund.
  * `launch::deferred`<br/>Wartet mit der Ausführung der Threadprozedur, bis der Aufrufer das Ergebnis mit `get` abholen möchte. Dazu muss nicht zwingend ein separater Thread verwendet werden, die Threadprozedur kann im Kontext des aktuellen Threads ausgeführt werden.


Analysieren Sie das Beispiel im Quellcode genau!
Die Ausführung des Programms lautet:

```
Preparing calculations ...
Now waiting for 4 seconds ...
launch::async thread done!
launch::deferred thread done!
asyncLazy evaluated after : 4.00645 seconds.
asyncEager evaluated after: 0.0006665 seconds.
```

#### Quellcode

[*Async_02.cpp*](Async_02.cpp).

---

## 4 Szenarien mit `std::async`

Es werden vier Vertiefungen der Funktionsschablone `std::async` studiert:

  * Parallele Ausführung mehrerer Threads - das Resultat wird mit `get` abgeholt.
  * Parallele Ausführung mehrerer Threads - es wird nur ein Resultat (mit `get` abgeholt). Welche Rolle spielt der Destruktor der beteiligten `Future`-Objekte?
  * Parallele Ausführung mehrerer Threads - es wird nur ein Resultat (mit `get` abgeholt). Welche Rolle spielt die *Launch Policy*?
  * Parallele Ausführung eines Threads - das Resultat muss nicht zwingend `get` abgeholt werden, mit der Methode `wait_for` an einem `Future`-Objekt und dem Aufzählungstyp `future_status` kann man den Status des `Future`-Objekts erfragen.

```cpp
enum class future_status    // names for timed wait function returns
{
    ready,
    timeout,
    deferred
};
```

Studieren Sie die Ausführung der vier Beispiele genau!

*Ausgabe Beispiel* 1:

```
Please wait ...
fib(40): 102334155
fib(41): 165580141
fib(42): 267914296
fib(43): 433494437
Done.
```

*Ausgabe Beispiel* 2:

```
Please wait ...
fib(40): 102334155
Done.   // <=== ???
```

*Ausgabe Beispiel* 3:

```
Preparing calculations ...
Retrieve single result of fib(42):
fib(42): 267914296
Done.   // <=== ???
```

*Ausgabe Beispiel* 4:

```
Calculation with wait:
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
not yet calculated ...
fib(40): 102334155
Elapsed time in milliseconds = 5655 [milliseconds]
```

---

[Zurück](../../Readme.md)

---
