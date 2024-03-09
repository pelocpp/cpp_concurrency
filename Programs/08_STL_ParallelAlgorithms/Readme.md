# STL und parallele Algorithmen

[Zur�ck](../../Readme.md)

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

Die Standard Template Library (STL) enth�lt sehr viele Algorithmen f�r das Suchen, Z�hlen, Manipulieren von STL-Containern.
Mit C++ 17 und sp�testens ab C++ 20 wurden viele dieser Algorithmen �berladen und neue Algorithmen hinzugef�gt.
Diese �berladenen und neuen Algorithmen k�nnen mit einer sogenannte *Execution Policy* aufgerufen werden.
Mit dieser *Execution Policy* l�sst sich spezifizieren, ob der Algorithmus sequentiell,
parallel oder parallel und vektorisiert ausgef�hrt werden soll.

Im Detail ist der *Execution Policy* Parameter wie folgt definiert:

  * `std::execution::sequenced_policy` - legt fest, dass die Ausf�hrung eines m�glicherweise parallelisierbaren Algorithmus nicht parallelisiert wird.</br>
     Das entsprechende globale Objekt ist `std::execution::seq`.
  * `std::execution::parallel_policy` - legt fest, dass die Ausf�hrung eines m�glicherweise parallelisierbaren Algorithmus parallel ausgef�hrt werden soll.</br>
     Das entsprechende globale Objekt ist `std::execution::par`.
  * `std::execution::parallel_unsequenced_policy` - legt fest, dass die Ausf�hrung eines parallelisierbaren Algorithmus parallelisiert und vektorisiert ausgef�hrt werden soll.</br>
    Das entsprechende globale Objekt ist `std::execution::par_unseq`.

Es gibt von den drei Klassennamen `std::execution::sequenced_policy`,`std::execution::parallel_policy`
und `std::execution::parallel_unsequenced_policy` Abk�rzungen.
Damit kann man den Sachverhalt einfacher formulieren:

  * Verwenden Sie `std::execution::seq`, um einen Algorithmus sequentiell auszuf�hren.
  * Verwenden Sie `std::execution::par`, um einen Algorithmus parallel auszuf�hren (normalerweise mit einer Thread-Pool-Implementierung).
  * Verwenden Sie `std::execution::par_unseq`, um einen Algorithmus parallel auszuf�hren, m�glicherweise werden dabei Vektoranweisungen wie SSE (*Streaming SIMD Extensions*) oder AVX (*Advanced Vector Extensions*) verwendet.

---

#### Quellcode:

[Siehe hier](STL_ParallelAlgorithms.cpp).

Die Ausf�hrung des Programms - und vor allem die Ausf�hrungszeit - sieht auf meinem Rechner wie folgt aus:

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

[Zur�ck](../../Readme.md)

---
