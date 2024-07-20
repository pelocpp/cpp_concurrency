# Die Klasse `std::jthread`

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Verhalten der Klassen `std::jthread` und `std::jthread` bzgl. des `join`-Aufrufs](#link3)
  * [Ein `std::jthread`-Objekt ist kooperativ unterbrechbar](#link4)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::jthread`
  * Klasse `std::stop_token`
  * Klasse `std::stop_callback`
  * Klasse `std::stop_source`
  * Klasse `std::atomic<bool>`

---

#### Quellcode

[*JThread_01.cpp*: `std::jthread`-Objekt und `join()`-Aufruf](JThread_01.cpp).<br />
[*JThread_02.cpp*: Kooperative Unterbrechung eines Threads](JThread_02.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Die Implementierung der `std::jthread`-Klasse basiert auf der bereits vorhandenen Klasse `std::thread`.

Die `std::jthread`-Klasse ist eine Wrapper-Klasse um Klasse `std::thread` herum,
sie stellt dieselben öffentlichen Methoden bereit, die die Aufrufe einfach weiterleiten.

Es kann folglich jedes `std::thread`-Objekt in ein `std::jthread`-Objekt umgewandelt werden,
dieses verhält sich dann genau so, wie sich ein entsprechendes `std::thread`-Objekt verhalten würde.

---

#### Verhalten der Klassen `std::jthread` und `std::jthread` bzgl. des `join`-Aufrufs <a name="link3"></a>

Wenn ein `std::thread`-Objekt den Zustand *joinable* besitzt
und dieses auf Grund des Verlassens eines Blocks (Gültigkeitsbereichs) aufgeräumt wird,
wird in seinem Destruktor automatisch `std::terminate` aufgerufen.

Ein `std::thread`-Objekt besitzt den Zustand *joinable*,
wenn am Objekt keine der beiden Methoden `join()` oder `detach()` aufgerufen wurde.

Ein Objekt der Klasse `std::jthread` verhält sich hier anders: Der Destruktor des Objekts ruft automatisch `join` auf,
wenn sich der Thread noch im Zustand *joinable* befindet.

---

#### Ein `std::jthread`-Objekt ist kooperativ unterbrechbar <a name="link4"></a>

Wie die Überschrift vermuten lässt, ist ein `std::jthread`-Objekt unterbrechbar,
es gibt also eine Möglichkeit, den Thread von außen zu stoppen.

Wir legen dabei Wert auf die Beobachtung, dass der Thread *kooperativ* unterbrechbar ist.
Der beste Weg, dies zu verstehen, besteht darin, einen Blick auf die Funktion `request_stop()` zu werfen:
Der Name ist sehr sorgfältig ausgewählt,
wir betrachten dazu im Quellcode eine Reihe von Beispielen:

  * *Szenario* 1:
   Der Hauptthread erzeugt einen neuen Thread,
   der jede Sekunde wiederholend etwas tut (eine Ausgabe in der Konsole).
   Der Hauptthread fährt dann mit einem 5-Sekunden-Job fort und wartet anschließend
   auf den Abschluss des anderen Threads.<br /><br />
   Da dieser nie fertig wird, wartet auch der Hauptthread ewig.
  * *Szenario* 2:
    Wie Szenario 1, es wurde von der Klasse `std::thread` zur Klasse `std::jthread` gewechselt.<br /><br />
  * *Szenario* 3:
     Nach 5 Sekunden erfolgt ein Aufruf von `request_stop()`, dieser ändert aber nichts am Ablauf des
     Programms: Man kann nicht von &bdquo;außen&rdquo; einen Stopp beantragen, der Thread selbst hat das letzte Wort.<br /><br />
  * *Szenario* 4:
   Im Kontext des Threads ist nun ein `std::stop_token`-Objekt verfügbar:
   Dieses besitzt eine Methode `stop_requested` &ndash; im Zusammenspiel mit `request_stop()` kann nun
   kooperativ ein Ende des Threads veranlasst werden.<br /><br />
  * *Szenario* 5:
   Im Kontext des Threads ist nun ein `std::stop_token`-Objekt verfügbar:
   Mit diesem Objekt kann man ein `std::stop_callback`-Objekt erzeugen, welches aufgerufen wird, wenn wiederum
   die `request_stop()`-Methode aufgerufen wird.<br /><br />
  * *Szenario* 6:
   Dieses Szenario ist vergleichbar zum letzten Szenario mit dem Unterschied, 
   dass aufgezeigt wird, dass das `std::stop_source`-Objekt auch über die Instanz eines `std::jthread`-Objekts
   abgerufen werden kann.<br /><br />
     

*Bemerkung*:<br />
In den *Szenarien* 5 und 6 wird auf Grund des konkurrierenden Zugriffs
zum Schutze einer `bool`-Variablen die `std::atomic<bool>`-Klasse verwendet.
Für die häufig gestellte Frage &bdquo;*ist das wirklich erforderlich*&rdquo; möchte ich &ndash; mit dieser [Unterstützung](https://stackoverflow.com/questions/16320838/when-do-i-really-need-to-use-atomicbool-instead-of-bool) &ndash;
so antworten:

> No data type in C++ is &bdquo;Atomic by Nature&rdquo; unless it is an object of kind `std::atomic<T>`.
  **That's because the standard says so!**

---

[Zurück](../../Readme.md)

---






