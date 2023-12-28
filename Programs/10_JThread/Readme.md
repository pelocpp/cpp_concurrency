# Die Klasse `std::jthread`

[Zurück](../../Readme.md)

---

Die Implementierung der `std::jthread`-Klasse basiert auf der bereits vorhandenen Klasse `std::thread`.

Die `std::jthread`-Klasse ist eine Wrapper-Klasse um Klasse `std::thread` herum,
sie stellt dieselben öffentlichen Methoden bereit, die die Aufrufe einfach weiterleiten.

Es kann folglich jedes `std::thread`-Objekt in ein `std::jthread`-Objekt umgewandelt werden,
dieses verhält sich dann genau so, wie sich ein entsprechendes `std::thread`-Objekt verhalten würde.

---

#### Verhalten der Klassen `std::jthread` und `std::jthread` bzgl. des `join`-Aufrufs

Wenn ein `std::thread`-Objekt den Zustand *joinable* besitzt
und dieses auf Grund des Verlassens eines Blocks (Gültigkeitsbereichs) aufgeräumt wird,
wird in seinem Destruktor automatisch `std::terminate` aufgerufen.

Ein `std::thread`-Objekt besitzt den Zustand *joinable*,
wenn am Objekt keine der beiden Methoden `join()` oder `detach()` aufgerufen wurde.

Ein Objekt der Klasse `std::jthread` verhält sich hier anders: Der Destruktor des Objekts ruft automatisch `join` auf,
wenn sich der Thread noch im Zustand *joinable* befindet.

---

#### Ein `std::jthread`-Objekt ist kooperativ unterbrechbar

Wie die Überschrift vermuten lässt, ist ein `std::thread`-Objekt unterbrechbar,
es gibt also eine Möglichkeit, den Thread von außen zu stoppen.

Wir legen dabei Wert auf die Beobachtung, dass der Thread *kooperativ* unterbrechbar ist.
Der beste Weg, dies zu verstehen, besteht darin, einen Blick auf die Funktion `request_stop()` zu werfen:
Der Name ist sehr sorgfältig ausgewählt,
wir betrachten dazu im Quellcode eine Reihe von Beispielen:

  * *Szenario* 1:
   Der Hauptthread erzeugt einen neuen Thread,
   der jede Sekunde wiederholend etwas tut (eine Ausgabe in der Konsole).
   Der Hauptthread fährt dann mit einem 5-Sekunden-Job fort und wartet anschließend
   auf den Abschluss des anderen Threads.
   Da dieser nie fertig wird, wartet auch der Hauptthread ewig.
  * *Szenario* 2:
    Wie Szenario 1, es wurde von der Klasse `std::thread` zur Klasse `std::jthread` gewechselt.
  * *Szenario* 3:
     Nach 5 Sekunden erfolgt ein Aufruf von `request_stop()`, dieser ändert aber nichts am Ablauf des
     Programms: Man kann nicht von &bdquo;außen&rdquo; einen Stopp beantragen, der Thread selbst hat das letzte Wort.
  * *Szenario* 4:
   Im Kontext des Threads ist nun ein `std::stop_token`-Objekt verfügbar:
   Dieses besitzt eine Methode `stop_requested` &ndash; im Zusammenspiel mit `request_stop()` kann nun
   kooperativ ein Ende des Threads veranlasst werden.

---

#### Quellcode:

[Teil 1: `std::jthread`-Objekt und `join()`-Aufruf](JThread_01.cpp).

[Teil 2: Kooperative Unterbrechung eines Threads](JThread_02.cpp).

---

[Zurück](../../Readme.md)

---
