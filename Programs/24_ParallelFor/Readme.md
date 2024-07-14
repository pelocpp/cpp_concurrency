# Realisierung einer `parallel_for` Wiederholungsschleife

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

* Klasse `std::thread`
* Klasse `std::mutex`
* Klasse `std::lock_guard`
* Klasse `std::function`

<ins>Funktionen</ins>:

* Funktion `std::thread::hardware_concurrency`

<ins>Utility-Funktion</ins>:

* Funktion `std::mem_fn`

---

### Allgemeines


Wir zeigen in diesem Abschnitt, wie man eine `for`-Wiederholungsschleife parallelisieren kann.
In vielen Bibliotheken gibt es eine solche Funktion unter dem Namen `parallel_for`.
In der STL gibt es eine derartige Funktion nicht, 
aber es bereitet keine große Mühe, sie selber zu schreiben.

---

#### Quellcode

[*ParallelFor.h*](ParallelFor.h).
[*ParallelFor.cpp*](ParallelFor.cpp).<br />
[*PrimeNumbers.cpp*](PrimeNumbers.cpp).<br />

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus einer *Stackoverflow*-Frage:

[Parallel Loops in C++](https://stackoverflow.com/questions/36246300/parallel-loops-in-c).

---

[Zurück](../../Readme.md)

---
