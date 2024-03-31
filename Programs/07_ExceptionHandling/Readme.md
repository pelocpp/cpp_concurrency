# Exception Handling

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::future`
  * Klasse `std::exception_ptr`
  * Klasse `std::exception`
  * Klasse `std::runtime_error`
  * Klasse `std::out_of_range`

<ins>Funktionen</ins>:

  * Funktion `std::async`

---

An zwei Beispielen wird gezeigt, wie Ausnahmen, die in einem Thread auftreten,
an den Erzeugerthread propagiert werden:

  * Propagating Exception from `std::async` invocation
  * Propagating Exception from `std::thread` invocation

#### Quellcode:

[*Exception.cpp*](Exception.cpp).

---

[Zurück](../../Readme.md)

---
