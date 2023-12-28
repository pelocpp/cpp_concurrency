# Realisierung einer `parallel_for` Wiederholungsschleife

[Zurück](../../Readme.md)

---

### Allgemeines


Wir zeigen in diesem Abschnitt auf, wie man eine `for`-Wiederholungsschleife parallelisieren kann.
In vielen Bibliotheken gibt es eine solche Funktion unter dem Namen `parallel_for`.
In der STL gibt es eine derartige Funktion nicht, 
aber es bereitet keine große Mühe, sie selber zu schreiben.

---

# Verwendete Hilfsmittel:

  * Klassen `std::thread`, `std::function`, `std::mutex` und `std::lock_guard`
  * TBD



  
  * Klassen `std::thread` und `std::function`
  * Funktion `std::thread::hardware_concurrency`
  * Container `std::vector`, `std::array`
  * Utility-Funktion `std::mem_fn`


---

#### Quellcode:

[ParallelFor.h](ParallelFor.h).

[ParallelFor.cpp](ParallelFor.cpp).<br />
[PrimeNumbers.cpp](PrimeNumbers.cpp).<br />

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus einer *Stackoverflow*-Frage:

[Parallel Loops in C++](https://stackoverflow.com/questions/36246300/parallel-loops-in-c).

---

[Zurück](../../Readme.md)

---
