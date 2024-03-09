# STL und parallele Algorithmen

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::execution::sequenced_policy`
  * Klasse `std::execution::parallel_policy`
  * Klasse `std::execution::parallel_unsequenced_policy`


<ins>Funktionen</ins>:

  * Funktion `std::sort`


<ins>Konstanten</ins>:

  * Konstante `std::execution::seq`
  * Konstante `std::execution::par`
  * Konstante `std::execution::par_unseq`

---

Die Standard Template Library (STL) enthält sehr viele Algorithmen für das Suchen, Zählen, Manipulieren von STL-Containern.
Mit C++ 17 und spätestens ab C++ 20 wurden viele dieser Algorithmen überladen und neue Algorithmen hinzugefügt.
Diese überladenen und neuen Algorithmen können mit einer sogenannte *Execution Policy* aufgerufen werden.
Mit dieser *Execution Policy* lässt sich spezifizieren, ob der Algorithmus sequentiell,
parallel oder parallel und vektorisiert ausgeführt werden soll.

Im Detail ist der *Execution Policy* Parameter wie folgt definiert:

  * `std::execution::sequenced_policy` - legt fest, dass die Ausführung eines möglicherweise parallelisierbaren Algorithmus nicht parallelisiert wird.</br>
     Das entsprechende globale Objekt ist `std::execution::seq`.
  * `std::execution::parallel_policy` - legt fest, dass die Ausführung eines möglicherweise parallelisierbaren Algorithmus parallel ausgeführt werden soll.</br>
     Das entsprechende globale Objekt ist `std::execution::par`.
  * `std::execution::parallel_unsequenced_policy` - legt fest, dass die Ausführung eines parallelisierbaren Algorithmus parallelisiert und vektorisiert ausgeführt werden soll.</br>
    Das entsprechende globale Objekt ist `std::execution::par_unseq`.

Es gibt von den drei Klassennamen `std::execution::sequenced_policy`,`std::execution::parallel_policy`
und `std::execution::parallel_unsequenced_policy` Abkürzungen.
Damit kann man den Sachverhalt einfacher formulieren:

  * Verwenden Sie `std::execution::seq`, um einen Algorithmus sequentiell auszuführen.
  * Verwenden Sie `std::execution::par`, um einen Algorithmus parallel auszuführen (normalerweise mit einer Thread-Pool-Implementierung).
  * Verwenden Sie `std::execution::par_unseq`, um einen Algorithmus parallel auszuführen, möglicherweise werden dabei Vektoranweisungen wie SSE (*Streaming SIMD Extensions*) oder AVX (*Advanced Vector Extensions*) verwendet.

---

#### Quellcode:

[Siehe hier](STL_ParallelAlgorithms.cpp).

Die Ausführung des Programms - und vor allem die Ausführungszeit - sieht auf meinem Rechner wie folgt aus:

```
Testing with 1000000 doubles...
Serial: Lowest: 5409, Highest: 4.29497e+09, Time: 2181.39
Serial: Lowest: 5409, Highest: 4.29497e+09, Time: 2171.89
Serial: Lowest: 5409, Highest: 4.29497e+09, Time: 2141.35
Serial: Lowest: 5409, Highest: 4.29497e+09, Time: 2137.65
Serial: Lowest: 5409, Highest: 4.29497e+09, Time: 2144.52

Testing with 1000000 doubles...
Parallel: Lowest: 5765, Highest: 4.29497e+09, Time: 445.756
Parallel: Lowest: 5765, Highest: 4.29497e+09, Time: 437.602
Parallel: Lowest: 5765, Highest: 4.29497e+09, Time: 445.472
Parallel: Lowest: 5765, Highest: 4.29497e+09, Time: 454.151
Parallel: Lowest: 5765, Highest: 4.29497e+09, Time: 444.117
```

---

Die Anregungen zum Quellcode finden Sie unter

[Using C++17 Parallel Algorithms for Better Performance](https://devblogs.microsoft.com/cppblog/using-c17-parallel-algorithms-for-better-performance/)

und 

[Parallel Algorithms of the Standard Template Library](https://www.modernescpp.com/index.php/parallel-algorithm-of-the-standard-template-library)

vor.

---

[Zurück](../../Readme.md)

---
