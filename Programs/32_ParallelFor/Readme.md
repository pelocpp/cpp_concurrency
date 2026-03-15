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

Eine einfache Realisierung verwendet die so genannten *Execution Policies*, mit der man ab C++17 viele der STL-Algorithmen
parallelisieren kann. In unserem Fall bietet sich der Algorithmus `std::for_each` an.

Das einzige Problem in der Realisierung besteht darin, wie wir den Schleifenindex nachahmen.
Wenn wir `std::for_each` verwenden wollen, dann iterieren wir über einen Bereich &ndash; oder eine *View* bei Verwendung der *Ranges*-Bibliotkek &ndash;,
sprich wir haben keine Index-Variable zur Verfügung.

Des Rätsels Lösung besteht in einer Kombination des `std::for_each`-Algorithmus mit entweder `std::iota` (STL)
oder mit `std::views::iota` (*Ranges*). Wir befüllen vor der Ausführung des `std::for_each`-Algorithmus einen Bereich (*Range*)
mit den gewünschten Indices und können auf diesem Bereich den `std::for_each`-Algorithmus ausführen.
Der eigentliche Rumpf der  `for`-Wiederholungsschleife kann durch ein Callable oder ein Lambda-Objekt bereitgestellt werden.

Wenn wir für den Index verschiedene ganzzahlige Typen zulassen wollen (z. B. `int`, `std::size_t`),
dann müssen wir im Funktionstemplate einen entsprechenden Template-Parameter definieren:


```cpp
01: template <typename TIndex, typename TFunc>
02:     requires std::integral<TIndex>
03: void parallel_for(TIndex first, TIndex last, TFunc func) {
04: 
05:     std::vector<std::size_t> indices(last - first);
06:     std::iota(indices.begin(), indices.end(), first);
07: 
08:     std::for_each(
09:         std::execution::par,
10:         indices.begin(),
11:         indices.end(),
12:         std::move(func)
13:     );
14: }
```

Oder mit der STL-Ranges Bibliothek:

```cpp
01: template <typename TIndex, typename TFunc>
02:     requires std::integral<TIndex>
03: void parallel_for_ranges(TIndex first, TIndex last, TFunc func) {
04: 
05:     auto range{ std::views::iota(first, last) };
06: 
07:     std::for_each(
08:         std::execution::par,
09:         range.begin(),
10:         range.end(),
11:         std::move(func)
12:     );
13: }
```


An der Ausführung des Programms kann man gut erkennen, dass die `parallel_for`-Funktion
auf 22 Threads aufgeteilt wurde:

```
[1]:    Start: 1000000000000000001 => 1000000000000005001
[1]:    Elapsed time: 20302 [milliseconds]
[1]:    Threads Used: 22
[1]:    Found Primes: 114
```


---

#### Quellcode

[*ParallelFor01.h*](ParallelFor01.h).<br />
[*PrimeNumbers01.cpp*](PrimeNumbers01.cpp).<br />
[*PrimeNumbers.cpp*](PrimeNumbers.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Buch &bdquo;*C++ High Performance*&rdquo;
von Björn Andrist und Viktor Sehr.

Eine ältere Realisierung einer `parallel_for`-Funktion stammt im Wesentlichen aus einer *Stackoverflow*-Frage:

[Parallel Loops in C++](https://stackoverflow.com/questions/36246300/parallel-loops-in-c).

---

[Zurück](../../Readme.md)

---
