# C++20: *Latches* und *Barrieren*

[Zurück](../../Readme.md)

---

#### Quellcode:

[Latches.cpp](Latches.cpp)<br/>
[Barriers.cpp](Barriers.cpp)

---

C++ hat weitere Klassen erhalten, um die Synchronisierung von Threads zu vereinfachen: *Latches* und *Barrieren*.

---

*Hinweis*: Windows-Systemprogrammierern dürften *Latches* und *Barrieren* in
gewisser Weise bekannt vorkommen: Sie haben viele Gemeinsamkeiten mit den
Win32-Betriebssystemfunktionen `WaitForSingleObject` und `WaitForMultipleObjects`.

---

## Einführung

Ein *Latch* ist eine Art &ldquo;Countdown&rdquo;-Zähler,
der mit einem Startwert initialisiert wird und beim Eintreten bestimmter Ereignisse heruntergezählt wird.
Erreicht der Zähler den Wert 0, meldet er auf eine bestimmte Art &ldquo;fertig&rdquo;.

So kann man beispielsweise 5 Threads beauftragen, etwas zu tun, und den Zähler
mit 5 vorbelegen. Wenn dann jeder Thread sein Ende dem Latch
als Ereignis mitteilt, meldet dieser nach 5 fertigen Threads, dass
die gesamte Ausführung abgeschlossen ist.

Beliebig viele Threads können ein *Latch*-Objekt zum Herunterzählen auf 0 verwenden.
Jeder Thread darf beliebig oft herunterzählen.
Steht der Wert des *Latch*-Objekts auf 0, kann es seinen Zustand nicht mehr verändern.
Es eignet sich also nur für einen einmaligen *Countdown*.

---

*Barrieren* sind so organisiert, dass sie mehrfach Ereignisse verschiedener Threads synchronisieren können.
Auch hier handelt es sich um einen Zähler, den jeder Thread aber nur einmal herunterzählen kann. Steht das *Barrieren*-Objekt auf 0,
wird eine vordefinierte Aktion aufgerufen und das *Barrieren*-Objekt lässt sich erneut verwenden.
Das ist hilfreich, wenn das Programm nach einem bestimmten Ereignis immer wieder darauf warten muss,
dass eine bestimmte Anzahl paralleler Reaktionen erfolgt ist.

---

## `std::latch`

Einen groben Überblick auf die wesentlichen Methoden der Klasse `std::latch` gibt *Tabelle* 1:

| Element | Beschreibung |
| :---- | :---- |
| Konstruktor | `explicit latch(std::ptrdiff_t expected);`<br/>Erzeugt ein `std::latch`-Objekt und initialisiert seinen internen Zähler. |
| Methode<br/>`count_down()` | `void count_down(std::ptrdiff_t n = 1);`<br/>Dekrementiert den internen Zähler atomar um den Wert *n*, ohne den Anrufer zu blockieren. |
| Methode<br/>`wait()` | `void wait() const;`<br/>Blockiert den aufrufenden Thread, bis der interne Zähler 0 erreicht. Wenn er bereits Null ist, wird sofort zurückgekehrt. |
| Methode<br/>`arrive_and_wait()` | `void arrive_and_wait(std::ptrdiff_t n = 1);`<br/>Dekrementiert den internen Zähler atomar um *n* und blockiert (falls erforderlich) den aufrufenden Thread, bis der Zähler Null erreicht. |

*Tabelle* 1: Wesentliche Elemente der Klasse `std::latch`.

Knapp formuliert kann man sagen, dass die Klasse `std::latch`
ein Abwärtszähler vom Typ `ptrdiff_t` ist, mit dem Threads synchronisiert werden können.
Der Wert des Zählers wird bei der Erstellung initialisiert.
Threads können an der Verriegelung (engl. &ldquo;*latch*&rdquo;)  blockieren,
bis der Zähler auf Null dekrementiert wird.
Es gibt keine Möglichkeit, den Zähler zwischendurch zu vergrößern oder zurückzusetzen.

---

## `std::barrier`

Einen groben Überblick auf die wesentlichen Methoden der Klasse `std::barrier` gibt *Tabelle* 2:

| Element | Beschreibung |
| :---- | :---- |
| Konstruktor | `explicit barrier(std::ptrdiff_t expected);`<br/>Erzeugt ein `std::barrier`-Objekt und initialisiert seinen internen Zähler. |
| Methode<br/>`wait()` | `void wait() const;`<br/>Blockiert den aufrufenden Thread, bis alle Threads den Synchronisationspunkt erreicht haben. |
| Methode<br/>`arrive_and_wait()` | `void arrive_and_wait();`<br/>Erreicht die Barriere und verringert den Zähler um 1, blockiert dann, bis die aktuelle Phase abgeschlossen ist. |
| Methode<br/>`arrive_and_drop()` | `void arrive_and_drop();`<br/>Dekrementiert sowohl den initial erwarteten Zähler für die nachfolgende Phase als auch die aktuelle Phase um 1. |

*Tabelle* 2: Wesentliche Elemente der Klasse `std::barrier`.

Die Klasse `std::barrier` bietet einen Thread-Koordinierungsmechanismus,
mit dem eine festgelegte Anzahl von Threads blockiert werden kann,
bis diese alle den Synchronisationspunkt (*Barriere*) erreicht haben.
Im Gegensatz zu `std::latch` sind Barrieren wiederverwendbar:
Sobald die ankommenden Threads am Synchronisationspunkt einer Barriere ihre Blockade verlassen,
kann dieselbe Barriere erneut verwendet werden.

---

[Zurück](../../Readme.md)

---
