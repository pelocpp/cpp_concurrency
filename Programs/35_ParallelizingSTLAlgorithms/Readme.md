# Parallelisierung von STL-Algorithmen

[Zur�ck](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::future`

<ins>Funktionen</ins>:

  * Funktion `std::async`

<ins>Aufz�hlungstypen</ins>:

  * Aufz�hlungstyp `enum class launch`

---

## Allgemeines

Das Zusammenspiel zwischen der Methode `std::async` und Objekten
des Typs `std::future` wird gezeigt.

Die `std::async`-Methode besitzt im Wesentlichen drei �berladungen:

  * `std::async` mit normaler C-Funktion.
  * `std::async` mit aufrufbarem Objekt.
  * `std::async` mit Lambda-Objekt.

#### Quellcode

[*Async_01.cpp*](Async_01.cpp)<br/>
[*Async_02.cpp*](Async_02.cpp)<br/>
[*Async_03.cpp*](Async_03.cpp)

---

---

[Zur�ck](../../Readme.md)

---
