# Die Klasse `std::packaged_task`

[Zurück](../../Readme.md)

---

## `std::packaged_task`

Das charakteristische Merkmal der Klasse `std::packaged_task` ist, dass die betroffene Task (Thread)
nicht von selbst startet, sondern explizit zu starten ist.

Für das Arbeiten mit `std::packaged_task`-Objekten sind typischerweise vier Schritte notwendig:

  * Die Aufgabe in einem `std::packaged_task`-Objekt verpacken bzw. aufbereiten.
  * Ein `std::future`-Objekt erzeugen.
  * Die Berechnung (mit einem `std::thread`-Objekt) ausführen.
  * Das Ergebnis (mit `get` am `std::future`-Objekt) abholen.

## Ein einfaches Beispiel

Ein einfaches Beispiel skizziert den Ablauf eines Szenarios mit vier `std::packaged_task`-Objekten.

Es ist die Summe der ersten 400 natürlichen Zahlen von 1 bis einschließlich 400 - ohne den Algorithmus von Gauss - zu berechnen.
Dabei kommen vier `std::packaged_task`-Objekte zum Einsatz.
Insbesondere betrachte man, dass sowohl `std::packaged_task`- als auch `std::future`-Objekte
in einem `std::deque`-Objekt ablegt werden können.


#### Quellcode:

[Siehe hier](PackagedTask.cpp).

---

[Zurück](../../Readme.md)

---
