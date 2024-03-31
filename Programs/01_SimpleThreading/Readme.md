# Nebenläufige Ausführung: Klasse `std::thread`

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Thread-Klassen</ins>:

  * Klasse `std::thread`
  * Namensraum `std::this_thread`


<ins>Methoden</ins>:

  * Methoden `join` und `detach`
  * Methode `get_id`
  * Methode `sleep_for`

---

## Allgemeines

Es werden an einfachen Beispielen Grundlagen des C++&ndash;Multithreadings betrachtet.

Ferner wird auf die 4 verschiedenen Möglichkeiten eingegangen, einen Thread zu erzeugen:

  * Thread mit Funktionszeiger.
  * Thread mit aufrufbarem Objekt.
  * Thread mit Lambda-Objekt.
  * Thread mit einer Methode eines Objekts.

---

#### Klasse `std::thread`, Methoden `join` und `detach`

  * Ein Objekt der Klasse `std::thread` kann mit einem *Callable* und seinen Parametern instanziiert werden
    und beginnt sofort mit der Ausführung.
  * Danach befindet sich das Objekt im sogenannten *joinable* Zustand, d.h. es muss *ge-joined* oder *ge-detached* werden,
    bevor es den Gültigkeitsbereich verlässt.
  * Wird das Objekt zu irgendeinem Zeitpunkt *ge-joined* (Aufruf von `join`),
    wartet der erzeugende Thread an dieser Stelle darauf, dass der erzeugte Thread seine Aufgabe abschließt,
    also die ihm zugeordnete Funktion (*callable*) bis zum Ende ausführt (Blockade).
  * Wird das Objekt *ge-detached* (Aufruf von `detach`), läuft der Thread im Hintergrund weiter
    und der erzeugende Thread wartet nicht auf sein Ende.

Der kritische Teil dieser Beobachtungen besteht darin,
wenn das Objekt den Gültigkeitsbereich verlässt und zerstört wird.
Zu diesem Zeitpunkt darf es **nicht im Zustand *joinable* sein**,
andernfalls erfolgt ein Aufruf von `std::terminate()`!

---

#### Quellcode:

[*SimpleThreading_01.cpp*: `std::thread` Basics](SimpleThreading_01.cpp)<br />
[*SimpleThreading_02.cpp*: "*4 Ways to create a Thread*"](SimpleThreading_02.cpp)<br />
[*SimpleThreading_03.cpp*: `std::thread` und `std::move`](SimpleThreading_03.cpp)<br />

---

[Zurück](../../Readme.md)

---
