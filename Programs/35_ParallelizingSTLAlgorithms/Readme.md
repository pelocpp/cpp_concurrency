# Parallelisierung von STL-Algorithmen

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

---

[Zurück](../../Readme.md)

---
